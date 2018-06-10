#include <walle/memory/malloc_tracker.h>
#include <walle/config/base.h>
#include <walle/common/back_trace.h>
#include <walle/sys/mutex.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <mutex>
#include <utility>
#include <dlfcn.h>
#include <string>
#include <iostream>

#if __APPLE__
    #define NOEXCEPT
    #define MALLOC_USABLE_SIZE malloc_size
    #include <malloc/malloc.h>
#elif __linux__
    #define NOEXCEPT noexcept
    #define MALLOC_USABLE_SIZE malloc_usable_size
    #include <malloc.h>
#endif


namespace walle {
namespace mem {
#define LEAK_CHECKER 0

static constexpr bool log_operations = 0;
static constexpr bool log_bypass_operations = 0;

static constexpr size_t log_operations_threshold = 100000;
static constexpr size_t log_bypass_operations_threshold = 100000;

// v-- set these to 1 for profiling output
static constexpr bool profile_operations = false;
static constexpr bool profile_bypass_operations = 0;

// enable checking of bypass_malloc() and bypass_free() pairing
#define BYPASS_CHECKER 0

// super-simple and super-slow leak detection

// variables of malloc tracker

//! In the generic hook implementation, we add to each allocation additional
//! data for bookkeeping.
static constexpr size_t padding = 16;    /* bytes (>= 2*sizeof(size_t)) */

//! function pointer to the real procedures, loaded using dlsym()
using malloc_type = void* (*)(size_t);
using free_type = void (*)(void*);
using realloc_type = void* (*)(void*, size_t);
using aligned_alloc_type = void* (*)(size_t, size_t);

static malloc_type real_malloc = nullptr;
static free_type real_free = nullptr;
static realloc_type real_realloc = nullptr;
static aligned_alloc_type real_aligned_alloc = nullptr;

//! a sentinel value prefixed to each allocation
static constexpr size_t sentinel = 0xDEADC0DE;

#define USE_ATOMICS 0

typedef std::atomic<ssize_t> CounterType;

#define COUNTER_ZERO { 0 }

WALLE_NO_SANITIZE
static inline ssize_t get(const CounterType& a) 
{
    return a.load();
}

WALLE_NO_SANITIZE
static inline ssize_t sync_add_and_fetch(CounterType& curr, ssize_t inc) 
{
    return (curr += inc);
}

WALLE_NO_SANITIZE
static inline ssize_t sync_sub_and_fetch(CounterType& curr, ssize_t dec) 
{
    return (curr -= dec);
}

#define INIT_HEAP_SIZE 1024 * 1024
static char init_heap[INIT_HEAP_SIZE];
static CounterType init_heap_use COUNTER_ZERO;
static constexpr int log_operations_init_heap = 0;

//! align allocations to init_heap to this number by rounding up allocations
static constexpr size_t init_alignment = sizeof(size_t);

//! output
#define PPREFIX "malloc_tracker ### "

static CounterType total_allocs COUNTER_ZERO;
static CounterType current_allocs COUNTER_ZERO;

static CounterType total_bytes COUNTER_ZERO;
static CounterType peak_bytes COUNTER_ZERO;

// free-floating memory allocated by malloc/free
static CounterType float_curr COUNTER_ZERO;

//base memory allocated by bypass_malloc/bypass_free
static CounterType base_curr COUNTER_ZERO;

bool memory_exceeded = false;
ssize_t memory_limit_indication = std::numeric_limits<ssize_t>::max();

WALLE_NO_SANITIZE
static void update_memprofile(ssize_t float_current, ssize_t base_current);

struct LocalStats {
    size_t  total_allocs;
    int64_t current_allocs;
    int64_t bytes;
};
__thread LocalStats tl_stats = { 0, 0, 0 };
__thread const ssize_t tl_delay_threshold = 1024 * 1024;

WALLE_NO_SANITIZE
void update_peak(ssize_t floatCurr, ssize_t baseCurr) 
{
    if (floatCurr + baseCurr > peak_bytes)
        peak_bytes = floatCurr + baseCurr;
}

WALLE_NO_SANITIZE
void flush_memory_statistics() 
{
    ssize_t mycurr = sync_add_and_fetch(float_curr, tl_stats.bytes);

    sync_add_and_fetch(total_bytes, tl_stats.bytes);
    sync_add_and_fetch(total_allocs, tl_stats.total_allocs);
    sync_add_and_fetch(current_allocs, tl_stats.current_allocs);
    update_peak(mycurr, base_curr);

    memory_exceeded = (mycurr >= memory_limit_indication);
    update_memprofile(mycurr, get(base_curr));

    tl_stats.bytes = 0;
    tl_stats.total_allocs = 0;
    tl_stats.current_allocs = 0;
}

WALLE_NO_SANITIZE
static void inc_count(size_t inc) 
{
    tl_stats.total_allocs++;
    tl_stats.current_allocs++;
    tl_stats.bytes += inc;

    if (tl_stats.bytes > tl_delay_threshold)
        flush_memory_statistics();
}

WALLE_NO_SANITIZE
static void dec_count(size_t dec) 
{
    tl_stats.current_allocs--;
    tl_stats.bytes -= dec;

    if (tl_stats.bytes < -tl_delay_threshold)
        flush_memory_statistics();
}

ssize_t malloc_tracker_current() 
{
    return float_curr;
}

ssize_t malloc_tracker_peak() 
{
    return peak_bytes;
}

void malloc_tracker_reset_peak() 
{
    peak_bytes = get(float_curr);
}

ssize_t malloc_tracker_total_allocs() 
{
    return total_allocs;
}

#if LEAK_CHECKER
#include <walle/sys/mutex.h>
static constexpr size_t kLeakCheckerSize = 1024 * 1024;
static constexpr size_t kLeakCheckerBacktrace = 32;
struct LeakCheckerEntry {
    void   * ptr;
    size_t size;
    size_t round;
    void   * addrlist[kLeakCheckerBacktrace];
};
static LeakCheckerEntry s_leak_checker[kLeakCheckerSize];
static walle::Mutex s_leak_mutex;
static size_t s_leak_round = 0;

static void leakchecker_malloc(void* ptr, size_t size) 
{
    walle::UniqueLock<walle::Mutex> lock(s_leak_mutex);
    size_t i;
    for (i = 0; i < kLeakCheckerSize; ++i) {
        if (s_leak_checker[i].ptr != nullptr) continue;
        s_leak_checker[i].ptr = ptr;
        s_leak_checker[i].size = size;
        s_leak_checker[i].round = s_leak_round;
        // retrieve current stack addresses
        lock.unlock();
        //backtrace(s_leak_checker[i].addrlist, kLeakCheckerBacktrace);
        break;
    }
    if (i == kLeakCheckerSize) abort();
}

static void leakchecker_free(void* ptr) {
    walle::UniqueLock<walle::Mutex> lock(s_leak_mutex);
    size_t i;
    for (i = 0; i < kLeakCheckerSize; ++i) {
        if (s_leak_checker[i].ptr == ptr) {
            s_leak_checker[i].ptr = nullptr;
            break;
        }
    }
    if (i == kLeakCheckerSize) {
        ::printf(PPREFIX "leak_free() checker: "
               "ptr = %p was not found\n", ptr);
        // abort();
    }
}
#endif

void malloc_tracker_print_status() 
{
    ::fprintf(stderr, PPREFIX "floating %zu, peak %zu, base %zu\n",
            get(float_curr), get(peak_bytes), get(base_curr));
}

void set_memory_limit_indication(ssize_t size) 
{
    ::fprintf(stderr, PPREFIX "set_memory_limit_indication %zu\n", size);
    memory_limit_indication = size;
}

static constexpr bool mp_enable = true;

static CounterType mp_next_bar COUNTER_ZERO;

struct OhlcBar {
    ssize_t high = 0, low = 0, close = 0;

    WALLE_NO_SANITIZE
    void init(ssize_t current) 
    {
        high = low = close = current;
    }

    WALLE_NO_SANITIZE
    void aggregate(ssize_t current) 
    {
        if (high < current) 
            high = current;
        if (low > current) 
            low = current;
        close = current;
    }
};
static OhlcBar mp_float;
static OhlcBar mp_base;

WALLE_NO_SANITIZE
static void update_memprofile(ssize_t float_current, ssize_t base_current) 
{
    if (!mp_enable) 
        return;
    if (mp_next_bar) {
        // start new OHLC bars
        mp_float.init(float_current);
        mp_base.init(base_current);
        mp_next_bar = false;
    } else {
        // aggregate into OHLC bars
        mp_float.aggregate(float_current);
        mp_base.aggregate(base_current);
    }
}

/**
 * @brief 
 * 
 */
WALLE_NO_SANITIZE
static __attribute__ ((constructor)) void init() { // NOLINT

    // try to use AddressSanitizer's malloc first.
    real_malloc = (malloc_type)dlsym(RTLD_DEFAULT, "__interceptor_malloc");
    if (real_malloc) {
        real_realloc = (realloc_type)dlsym(RTLD_DEFAULT, "__interceptor_realloc");
        if (!real_realloc) {
            ::fprintf(stderr, PPREFIX "dlerror %s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        real_free = (free_type)dlsym(RTLD_DEFAULT, "__interceptor_free");
        if (!real_free) {
            ::fprintf(stderr, PPREFIX "dlerror %s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        ::fprintf(stderr, PPREFIX "using AddressSanitizer's malloc\n");

        return;
    }

    real_malloc = (malloc_type)dlsym(RTLD_NEXT, "malloc");
    if (!real_malloc) {
        ::fprintf(stderr, PPREFIX "dlerror %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    real_realloc = (realloc_type)dlsym(RTLD_NEXT, "realloc");
    if (!real_realloc) {
        ::fprintf(stderr, PPREFIX "dlerror %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    real_aligned_alloc = (aligned_alloc_type)dlsym(RTLD_NEXT, "aligned_alloc");

    real_free = (free_type)dlsym(RTLD_NEXT, "free");
    if (!real_free) {
        ::fprintf(stderr, PPREFIX "dlerror %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

WALLE_NO_SANITIZE
static __attribute__ ((destructor)) void finish() 
{ 
    update_memprofile(get(float_curr), get(base_curr));
    ::fprintf(stderr, PPREFIX
            "exiting, total: %zu, peak: %zu, current: %zu / %zu, "
            "allocs: %zu, unfreed: %zu\n",
            get(total_bytes), get(peak_bytes),
            get(float_curr), get(base_curr),
            get(total_allocs), get(current_allocs));
}


/********************************************/
WALLE_NO_SANITIZE
void * bypass_malloc(size_t size) 
{
    void* ptr = real_malloc(size);
    if (!ptr) {
        ::fprintf(stderr, PPREFIX "bypass_malloc(%zu size) = %p   (current %zu / %zu)\n",
                size, ptr, get(float_curr), get(base_curr));
        return ptr;
    }

    if (log_bypass_operations && size >= log_bypass_operations_threshold) {
        ::fprintf(stderr, PPREFIX "bypass_malloc(%zu size) = %p   (current %zu / %zu)\n",
                size, ptr, get(float_curr), get(base_curr));
    }
    if (profile_bypass_operations) {
        std::string result;
        walle::print_raw_backtrace(
            result, 16, PPREFIX "bypass profile %zu", size);
        ::fprintf(stderr, "%s", result.c_str());
    }
    ssize_t mycurr = sync_add_and_fetch(base_curr, size);

    total_bytes += size;
    update_peak(float_curr, mycurr);

    sync_add_and_fetch(total_allocs, 1);
    sync_add_and_fetch(current_allocs, 1);

    update_memprofile(get(float_curr), mycurr);

    return ptr;
}

WALLE_NO_SANITIZE
void bypass_free(void* ptr, size_t size)  
{

    ssize_t mycurr = sync_sub_and_fetch(base_curr, size);

    sync_sub_and_fetch(current_allocs, 1);

    update_memprofile(get(float_curr), mycurr);
    return real_free(ptr);
}

WALLE_NO_SANITIZE
void * bypass_aligned_alloc(size_t alignment, size_t size) 
{
    void* ptr;
    if (real_aligned_alloc) {
        std::cout<<"real_aligned_alloc"<<std::endl;
        ptr = real_aligned_alloc(alignment, size);
    } else {
        // emulate alignment by wasting memory
        void* mem = real_malloc((alignment - 1) + sizeof(void*) + size);

        uintptr_t uptr = reinterpret_cast<uintptr_t>(mem) + sizeof(void*);
        uptr += alignment - (uptr & (alignment - 1));
        ptr = reinterpret_cast<void*>(uptr);

        // store original pointer for deallocation
        (reinterpret_cast<void**>(ptr))[-1] = mem;
    }
    if (!ptr) {
        ::fprintf(stderr, PPREFIX "bypass_aligned_alloc(%zu align %zu size) = %p   (current %zu / %zu)\n",
                alignment, size, ptr, get(float_curr), get(base_curr));
        return ptr;
    }

    ssize_t mycurr = sync_add_and_fetch(base_curr, size);

    total_bytes += size;
    update_peak(float_curr, mycurr);

    sync_add_and_fetch(total_allocs, 1);
    sync_add_and_fetch(current_allocs, 1);

    update_memprofile(get(float_curr), mycurr);
    return ptr;
}

WALLE_NO_SANITIZE
void bypass_aligned_free(void* ptr, size_t size) 
{
    
    ssize_t mycurr = sync_sub_and_fetch(base_curr, size);

    sync_sub_and_fetch(current_allocs, 1);

    update_memprofile(get(float_curr), mycurr);

    if (real_aligned_alloc) {
        return real_free(ptr);
    } else {
        real_free((reinterpret_cast<void**>(ptr))[-1]);
    }
}
/*******************************************/

WALLE_NO_SANITIZE
static void * preinit_malloc(size_t size) 
{

    size_t aligned_size = size + (init_alignment - size % init_alignment);

    size_t offset = (init_heap_use += (padding + aligned_size));

    if (offset > INIT_HEAP_SIZE) {
        ::fprintf(stderr, PPREFIX "init heap full !!!\n");
        exit(EXIT_FAILURE);
    }

    char* ret = init_heap + offset;

    //! prepend allocation size and check sentinel
    *reinterpret_cast<size_t*>(ret) = aligned_size;
    *reinterpret_cast<size_t*>(ret + padding - sizeof(size_t)) = sentinel;

    inc_count(aligned_size);

    if (log_operations_init_heap) {
        ::fprintf(stderr, PPREFIX "malloc(%zu / %zu) = %p   on init heap\n",
                size, aligned_size, static_cast<void*>(ret + padding));
    }

    return ret + padding;
}

WALLE_NO_SANITIZE
static void * preinit_realloc(void* ptr, size_t size) {

    if (log_operations_init_heap) {
        ::fprintf(stderr, PPREFIX "realloc(%p) = on init heap\n", ptr);
    }

    ptr = static_cast<char*>(ptr) - padding;

    if (*reinterpret_cast<size_t*>(
            static_cast<char*>(ptr) + padding - sizeof(size_t)) != sentinel) {
        ::fprintf(stderr, PPREFIX
                "realloc(%p) has no sentinel !!! memory corruption?\n",
                ptr);
    }

    size_t oldsize = *reinterpret_cast<size_t*>(ptr);

    if (oldsize >= size) {
        //! keep old area
        return static_cast<char*>(ptr) + padding;
    } else {
        //! allocate new area and copy data
        ptr = static_cast<char*>(ptr) + padding;
        void* newptr = malloc(size);
        memcpy(newptr, ptr, oldsize);
        free(ptr);
        return newptr;
    }
}

WALLE_NO_SANITIZE
static void preinit_free(void* ptr) 
{
    // don't do any real deallocation.

    ptr = static_cast<char*>(ptr) - padding;

    if (*reinterpret_cast<size_t*>(
            static_cast<char*>(ptr) + padding - sizeof(size_t)) != sentinel) {
        ::fprintf(stderr, PPREFIX
                "free(%p) has no sentinel !!! memory corruption?\n",
                ptr);
    }

    size_t size = *reinterpret_cast<size_t*>(ptr);
    dec_count(size);

    if (log_operations_init_heap) {
        ::fprintf(stderr, PPREFIX "free(%p) -> %zu   on init heap\n", ptr, size);
    }
}

void malloc_tracker_print_leaks() 
{
#if LEAK_CHECKER
    walle::UniqueLock<walle::Mutex> lock(s_leak_mutex);
   // ::printf("s_leak_round: %zu\n", s_leak_round);
    for (size_t i = 0; i < kLeakCheckerSize; ++i) {
        if (s_leak_checker[i].ptr == nullptr) 
            continue;

        if (s_leak_checker[i].round == s_leak_round) {
            void** addrlist = s_leak_checker[i].addrlist;
            ::printf(PPREFIX "leak checker: "
                   "ptr %p size %zu new unfreed allocation: "
                   "%p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p "
                   "%p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p\n",
                   s_leak_checker[i].ptr, s_leak_checker[i].size,
                   addrlist[0], addrlist[1], addrlist[2], addrlist[3],
                   addrlist[4], addrlist[5], addrlist[6], addrlist[7],
                   addrlist[8], addrlist[9], addrlist[10], addrlist[11],
                   addrlist[12], addrlist[13], addrlist[14], addrlist[15],
                   addrlist[16], addrlist[17], addrlist[18], addrlist[19],
                   addrlist[20], addrlist[21], addrlist[22], addrlist[23],
                   addrlist[24], addrlist[25], addrlist[26], addrlist[27],
                   addrlist[28], addrlist[29], addrlist[30], addrlist[31]);
        }
    }
    ++s_leak_round;
#endif
}

#if defined(MALLOC_USABLE_SIZE)

/*
 * This is a malloc() tracker implementation which uses an available system call
 * to determine the amount of memory used by an allocation (which may be more
 * than the allocated size). On Linux's glibc there is malloc_usable_size().
 */

//! exported malloc symbol that overrides loading from libc
WALLE_NO_SANITIZE
void * malloc(size_t size)  
{

    if (WALLE_UNLIKELY(!real_malloc))
        return preinit_malloc(size);

    //! call real malloc procedure in libc
    void* ret = (*real_malloc)(size);
    if (!ret) {
        ::fprintf(stderr, PPREFIX "malloc(%zu size) = %p   (current %zu / %zu)\n",
                size, ret, get(float_curr), get(base_curr));
        return nullptr;
    }

    size_t size_used = MALLOC_USABLE_SIZE(ret);
    inc_count(size_used);

    if (log_operations && size_used >= log_operations_threshold) {
        ::fprintf(stderr, PPREFIX "malloc(%zu size / %zu used) = %p   (current %zu / %zu)\n",
                size, size_used, ret, get(float_curr), get(base_curr));
    }

    if (profile_operations) {
        static __thread bool recursive = false;

        if (!recursive) {
            recursive = true;
            std::string result;
            walle::print_raw_backtrace(
                result, 16, PPREFIX "profile %zu", size);
            ::fprintf(stderr, "%s\n", result.c_str());

            recursive = false;
        }
    }

    {
#if LEAK_CHECKER
        static __thread bool recursive = false;
        if (!recursive) {
            recursive = true;
            leakchecker_malloc(ret, size);
            recursive = false;
        }
#endif
    }

    return ret;
}

WALLE_NO_SANITIZE
void free(void* ptr) 
{

    if (!ptr) return;   //! free(nullptr) is no operation

    if (WALLE_UNLIKELY(
            static_cast<char*>(ptr) >= init_heap &&
            static_cast<char*>(ptr) <= init_heap + get(init_heap_use)))
    {
        return preinit_free(ptr);
    }

    if (WALLE_UNLIKELY(!real_free)) {
        ::fprintf(stderr, PPREFIX
                "free(%p) outside init heap and without real_free !!!\n", ptr);
        return;
    }

    size_t size_used = MALLOC_USABLE_SIZE(ptr);
    dec_count(size_used);

    if (log_operations && size_used >= log_operations_threshold) {
        ::fprintf(stderr, PPREFIX "free(%p) -> %zu   (current %zu / %zu)\n",
                ptr, size_used, get(float_curr), get(base_curr));
    }

#if LEAK_CHECKER
    leakchecker_free(ptr);
#endif

    (*real_free)(ptr);
}

WALLE_NO_SANITIZE
void * calloc(size_t nmemb, size_t size)  
{
    size *= nmemb;
    void* ret = malloc(size);
    if (!ret) return ret;
    memset(ret, 0, size);
    return ret;
}

WALLE_NO_SANITIZE
void * realloc(void* ptr, size_t size)  
{

    if (static_cast<char*>(ptr) >= static_cast<char*>(init_heap) &&
        static_cast<char*>(ptr) <= static_cast<char*>(init_heap) + get(init_heap_use))
    {
        return preinit_realloc(ptr, size);
    }

    if (size == 0) { //! special case size == 0 -> free()
        free(ptr);
        return nullptr;
    }

    if (ptr == nullptr) { //! special case ptr == 0 -> malloc()
        return malloc(size);
    }

    size_t oldsize_used = MALLOC_USABLE_SIZE(ptr);
    dec_count(oldsize_used);

    void* newptr = (*real_realloc)(ptr, size);
    if (!newptr) return nullptr;

    size_t newsize_used = MALLOC_USABLE_SIZE(newptr);
    inc_count(newsize_used);

    if (log_operations && newsize_used >= log_operations_threshold)
    {
        if (newptr == ptr) {
            ::fprintf(stderr, PPREFIX
                    "realloc(%zu -> %zu / %zu) = %p   (current %zu / %zu)\n",
                    oldsize_used, size, newsize_used, newptr,
                    get(float_curr), get(base_curr));
        }
        else {
            ::fprintf(stderr, PPREFIX
                    "realloc(%zu -> %zu / %zu) = %p -> %p   (current %zu / %zu)\n",
                    oldsize_used, size, newsize_used, ptr, newptr,
                    get(float_curr), get(base_curr));
        }
    }
    if(profile_bypass_operations) {
       (void)0; 
    }
    return newptr;
}

#else
WALLE_NO_SANITIZE
void * malloc(size_t size)  
{
    if (!real_malloc)
        return preinit_malloc(size);

    //! call real malloc procedure in libc
    void* ret = (*real_malloc)(padding + size);

    inc_count(size);
    if (log_operations && size >= log_operations_threshold) {
        ::fprintf(stderr, PPREFIX "malloc(%zu) = %p   (current %zu / %zu)\n",
                size, static_cast<char*>(ret) + padding,
                get(float_curr), get(base_curr));
    }

    //! prepend allocation size and check sentinel
    *reinterpret_cast<size_t*>(ret) = size;
    *reinterpret_cast<size_t*>(
        static_cast<char*>(ret) + padding - sizeof(size_t)) = sentinel;

    return static_cast<char*>(ret) + padding;
}

WALLE_NO_SANITIZE
void free(void* ptr)  
{

    if (!ptr) return;   //! free(nullptr) is no operation

    if (static_cast<char*>(ptr) >= init_heap &&
        static_cast<char*>(ptr) <= init_heap + get(init_heap_use))
    {
        return preinit_free(ptr);
    }

    if (!real_free) {
        ::fprintf(stderr, PPREFIX
                "free(%p) outside init heap and without real_free !!!\n", ptr);
        return;
    }

    ptr = static_cast<char*>(ptr) - padding;

    if (*reinterpret_cast<size_t*>(
            static_cast<char*>(ptr) + padding - sizeof(size_t)) != sentinel) {
        ::fprintf(stderr, PPREFIX
                "free(%p) has no sentinel !!! memory corruption?\n", ptr);
    }

    size_t size = *reinterpret_cast<size_t*>(ptr);
    dec_count(size);

    if (log_operations && size >= log_operations_threshold) {
        ::fprintf(stderr, PPREFIX "free(%p) -> %zu   (current %zu / %zu)\n",
                ptr, size, get(float_curr), get(base_curr));
    }

    (*real_free)(ptr);
}

WALLE_NO_SANITIZE
void * calloc(size_t nmemb, size_t size)  
{
    size *= nmemb;
    if (!size) return nullptr;
    void* ret = malloc(size);
    if (!ret) return ret;
    memset(ret, 0, size);
    return ret;
}

WALLE_NO_SANITIZE
void * realloc(void* ptr, size_t size)  
{
    if (static_cast<char*>(ptr) >= static_cast<char*>(init_heap) &&
        static_cast<char*>(ptr) <=
        static_cast<char*>(init_heap) + get(init_heap_use))
    {
        return preinit_realloc(ptr, size);
    }

    if (size == 0) { //! special case size == 0 -> free()
        free(ptr);
        return nullptr;
    }

    if (ptr == nullptr) { //! special case ptr == 0 -> malloc()
        return malloc(size);
    }

    ptr = static_cast<char*>(ptr) - padding;

    if (*reinterpret_cast<size_t*>(
            static_cast<char*>(ptr) + padding - sizeof(size_t)) != sentinel) {
        ::fprintf(stderr, PPREFIX
                "free(%p) has no sentinel !!! memory corruption?\n", ptr);
    }

    size_t oldsize = *reinterpret_cast<size_t*>(ptr);

    dec_count(oldsize);
    inc_count(size);
    if(profile_bypass_operations) {
       (void)0; 
    }
    void* newptr = (*real_realloc)(ptr, padding + size);

    if (log_operations && size >= log_operations_threshold) {
        if (newptr == ptr){
            ::fprintf(stderr, PPREFIX
                    "realloc(%zu -> %zu) = %p   (current %zu / %zu)\n",
                    oldsize, size, newptr, get(float_curr), get(base_curr));
        } else {
            ::fprintf(stderr, PPREFIX
                    "realloc(%zu -> %zu) = %p -> %p   (current %zu / %zu)\n",
                    oldsize, size, ptr, newptr, get(float_curr), get(base_curr));
        }
    }

    *reinterpret_cast<size_t*>(newptr) = size;

    return static_cast<char*>(newptr) + padding;
}
#endif
} //namespace mem
} //namespace walle