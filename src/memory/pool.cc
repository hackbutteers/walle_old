#include <walle/memory/pool.h>
#include <walle/math/math.h>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>

namespace walle {
namespace mem {

Pool& GPool() 
{
    static Pool* pool = new Pool();
    return *pool;
}

#if defined(__clang__) || defined(__GNUC__)
static __attribute__ ((destructor)) void s_gpool_destroy() 
{
    // deallocate memory arenas but do not destroy the pool
    GPool().deallocate_all();
}
#endif

struct Pool::Slot {
    uint32_t size;
    uint32_t next;
};

struct Pool::Arena {
    size_t magic; //!< magic word
    size_t total_size; //!< total size of this Arena
    Arena  *next_arena; //!< next pointers for free list.
    Arena  *prev_arena; //!< prev pointers for free list.
    bool   oversize;  //!< oversize arena
    
    union {
        uint32_t free_size; //!< first sentinel Slot which is never used for payload data, instead size =
                            //!< remaining free size, and next = pointer to first free byte.
        Slot     head_slot;
    };
    uint32_t num_slots() const 
    {
        return static_cast<uint32_t>(
            (total_size - sizeof(Arena)) / sizeof(Slot));
    }

    Slot * begin() 
    { 
        return &head_slot + 1; 
    }

    Slot * end() 
    { 
        return &head_slot + 1 + num_slots(); 
    }
    Slot * slot(size_t i) 
    { 
        return &head_slot + 1 + i; 
    }

    void * find_free(size_t size);
};

struct Pool::ObjectArena {
    //! magic word
    size_t     magic;
    //! next and prev pointers for free list.
    ObjectArena *next_arena;
    ObjectArena *prev_arena;
    //! number of slots free
    size_t     free_slots;
    //! array of flag words: each bit in the flag words indicates if a slot is
    //! used (=0) for free (=1)
    size_t     flags[1];

    char * begin(size_t num_flags) 
    {
        return reinterpret_cast<char*>(flags + num_flags);
    }
};

class Pool::ObjectPool {
public:
    ObjectPool(size_t size);
    ~ObjectPool();

    //! allocate a new free arena
    void allocate_object_arena();

    //! allocate a slot in the object pool
    void * allocate();

    //! free a slot
    void deallocate(void* ptr);

    //! verify arena chains
    void self_verify();

private:
    //! object size in this pool
    size_t _size;
    //! arena chain with free slots
    ObjectArena* _free = nullptr;
    //! arenas completely full
    ObjectArena* _full = nullptr;

    static const size_t kDefaultArenaSize = 16384;

    //! number of slots in an ObjectArena
    size_t _num_slots;
    //! number of flag words in an ObjectArena
    size_t _num_flags;

    //! total number of object slots allocated
    size_t _total_slots = 0;
    //! total number of free object slots
    size_t _total_free = 0;
};

Pool::ObjectPool::ObjectPool(size_t size)
    : _size(size) 
{
    // calculate number of slots for given object size
    _num_slots =
        8 * (kDefaultArenaSize - sizeof(ObjectArena) + sizeof(size_t))
        / (8 * _size + 1);

    // calculate number of bit flag words
    _num_flags = (_num_slots + (8 * sizeof(size_t) - 1)) / (8 * sizeof(size_t));

    if (kDebug) {
        ::printf("ObjectPool() _size=%zu _num_slots=%zu _num_flags=%zu\n",
               _size, _num_slots, _num_flags);
    }

}

Pool::ObjectPool::~ObjectPool() 
{
    if (_total_slots != _total_free) {
        ::printf("~ObjectPool() _size=%zu _total_used=%zu\n",
               _size, _total_slots - _total_free);
    }
    ObjectArena *p = _free;
    while (p) {
        ObjectArena *q = p->next_arena;
        bypass_aligned_free(p, _size);
        p = q;
    }
}

void Pool::ObjectPool::allocate_object_arena() 
{
    // Allocate space for the new block
    ObjectArena* new_arena =
        reinterpret_cast<ObjectArena*>(
            bypass_aligned_alloc(kDefaultArenaSize, kDefaultArenaSize));
    if (!new_arena) {
        ::fprintf(stderr, "out-of-memory - mem::ObjectPool cannot allocate a new ObjectArena."
                " _size=%zu\n", _size);
        WALLE_ASSERT(false, "");
    }

    WALLE_ASSERT(
        new_arena == reinterpret_cast<ObjectArena*>(
                reinterpret_cast<uintptr_t>(new_arena) & ~(kDefaultArenaSize - 1))
            , "");

    new_arena->magic = 0xAEEA1111AEEA2222LLU + _size;
    new_arena->prev_arena = nullptr;
    new_arena->next_arena = _free;
    if (_free)
        _free->prev_arena = new_arena;
    _free = new_arena;

    new_arena->free_slots = _num_slots;
    for (size_t i = 0; i < _num_flags; ++i)
        _free->flags[i] = ~size_t(0);

    _total_slots += _num_slots;
    _total_free += _num_slots;
}

void* Pool::ObjectPool::allocate() 
{
    if (kDebug) {
        ::printf("ObjectPool::allocate() _size=%zu\n", _size);
    }

    // allocate arenas, keep at least one extra free arena
    while (_free == nullptr || _total_free <= _num_slots)
        allocate_object_arena();

    size_t slot = size_t(-1);
    for (size_t i = 0; i < _num_flags; ++i) {
        unsigned s = walle::math::ffs(_free->flags[i]);
        if (s != 0) {
            slot = i * 8 * sizeof(size_t) + (s - 1);
            _free->flags[i] &= ~(size_t(1) << (s - 1));
            break;
        }
    }

    void* ptr = _free->begin(_num_flags) + slot * _size;

    if (--_free->free_slots == 0) {
        ObjectArena* next_free = _free->next_arena;

        // put now full ObjectArena into _full list
        _free->next_arena = _full;
        if (_full)
            _full->prev_arena = _free;
        _full = _free;

        _free = next_free;
        if (next_free)
            next_free->prev_arena = nullptr;
    }

    --_total_free;

    return ptr;
}

void Pool::ObjectPool::deallocate(void* ptr) 
{
    if (kDebug) {
        ::printf("ObjectPool::deallocate() _size=%zu\n", _size);
    }

    // find arena containing ptr
    ObjectArena* const arena =
        reinterpret_cast<ObjectArena*>(
            reinterpret_cast<uintptr_t>(ptr) & ~(kDefaultArenaSize - 1));
    WALLE_ASSERT(arena->magic == 0xAEEA1111AEEA2222LLU + _size, "");

    if (!(ptr >= arena->begin(_num_flags) &&
          ptr < arena->begin(_num_flags) + _num_slots * _size)) {
        WALLE_ASSERT(false, "deallocate() of memory not in any arena.");
    }

    // calculate the slot directly
    size_t slot =
        (reinterpret_cast<char*>(ptr) - arena->begin(_num_flags)) / _size;

    size_t fa = slot / (8 * sizeof(size_t));
    size_t fb = slot % (8 * sizeof(size_t));
    size_t mask = (size_t(1) << fb);
    WALLE_ASSERT((arena->flags[fa] & mask) == 0, "");

    // set free bit
    arena->flags[fa] |= mask;
    if (arena->free_slots == 0) {
        if (kDebug)
            ::printf("ObjectPool::deallocate() splice free arena from _full list\n");

        // splice arena from doubly linked list (_full)
        if (arena->prev_arena)
            arena->prev_arena->next_arena = arena->next_arena;
        else {
            WALLE_ASSERT(_full == arena, "");
            _full = arena->next_arena;
        }
        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        // put ObjectArena with newly freed slot into free list
        if (_free)
            _free->prev_arena = arena;
        arena->next_arena = _free;
        arena->prev_arena = nullptr;
        _free = arena;
    }

    ++arena->free_slots;
    ++_total_free;

    if (arena->free_slots == _num_slots && _total_free > 16 * _num_slots) {
        if (kDebug)
            ::printf("ObjectPool::deallocate() splice empty arena from free_list\n");

        // splice arena from doubly linked list (_full)
        if (arena->prev_arena) {
            arena->prev_arena->next_arena = arena->next_arena;
        }else {
            WALLE_ASSERT(_free == arena, "");
            _free = arena->next_arena;
        }
        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        bypass_aligned_free(arena, kDefaultArenaSize);
        _total_free -= _num_slots;
        _total_slots -= _num_slots;
    }
}

void Pool::ObjectPool::self_verify() 
{
    if (kDebug) {
        ::printf("ObjectPool::print() _size=%zu\n", _size);
    }

    size_t total_slots = 0, total_free = 0, total_used = 0;

    for (ObjectArena* arena = _free; arena != nullptr;
         arena = arena->next_arena) {
        size_t arena_free = 0;

        for (size_t i = 0; i < _num_slots; ++i) {
            size_t fa = i / (8 * sizeof(size_t));
            size_t fb = i % (8 * sizeof(size_t));

            if ((arena->flags[fa] & (size_t(1) << fb)) == 0) {
                // slot is used
                total_used++;
            }
            else {
                // slot is free
                arena_free++;
                total_free++;
            }
        }

        WALLE_ASSERT(arena_free != 0, "");
        total_slots += _num_slots;

        if (arena->next_arena)
            WALLE_ASSERT(arena->next_arena->prev_arena == arena, "");
        if (arena->prev_arena)
            WALLE_ASSERT(arena->prev_arena->next_arena == arena, "");
    }

    for (ObjectArena* arena = _full; arena != nullptr;
         arena = arena->next_arena)
    {
        size_t arena_free = 0;

        for (size_t i = 0; i < _num_slots; ++i) {
            size_t fa = i / (8 * sizeof(size_t));
            size_t fb = i % (8 * sizeof(size_t));

            if ((arena->flags[fa] & (size_t(1) << fb)) == 0) {
                // slot is used
                total_used++;
            }
            else {
                // slot is free
                arena_free++;
                total_free++;
            }
        }

        WALLE_ASSERT(arena_free == 0u, "");
        total_slots += _num_slots;

        if (arena->next_arena)
            WALLE_ASSERT(arena->next_arena->prev_arena == arena, "");
        if (arena->prev_arena)
            WALLE_ASSERT(arena->prev_arena->next_arena == arena, "");
    }
    WALLE_ASSERT(total_slots == _total_slots, "");
    WALLE_ASSERT(total_free == _total_free, "");
    WALLE_ASSERT(total_used == _total_slots - _total_free, "");
}

static inline size_t calc_bin_for_size(size_t size) 
{
    if (size == 0)
        return 0;
    else
        return 1 + walle::math::integer_log2_floor_template(size);
}

//! lowest size still in bin
static inline size_t bin_lower_bound(size_t bin) 
{
    if (bin == 0)
        return 0;
    else
        return (size_t(1) << (bin - 1));
}

Pool::Pool(size_t default_arena_size) noexcept
    : _default_arena_size(default_arena_size) 
{
   walle::UniqueLock<walle::Mutex> lock(_mutex);

    for (size_t i = 0; i < KnumBins + 1; ++i)
        _arena_bin[i] = nullptr;

    if (kDebugCheckPairing)
        _allocs.resize(kCheckLimit);

    while (_free < _min_free)
        allocate_free_arena(_default_arena_size);

    _object_32 = new ObjectPool(32);
    _object_64 = new ObjectPool(64);
    _object_128 = new ObjectPool(128);
    _object_256 = new ObjectPool(256);
}


Pool::~Pool() noexcept 
{
    walle::UniqueLock<walle::Mutex> lock(_mutex);
    if (_size != 0) {
        std::cout << "~Pool() pool still contains "
                  << sizeof(Slot) * _size << " bytes" << std::endl;

        for (size_t i = 0; i < _allocs.size(); ++i) {
            if (_allocs[i].first == nullptr) 
                continue;
            std::cout << "~Pool() has ptr=" << _allocs[i].first
                      << " size=" << _allocs[i].second << std::endl;
        }
    }
    assert(_size == 0);

    delete _object_32;
    delete _object_64;
    delete _object_128;
    delete _object_256;
    int_deallocate_all();
}


void* Pool::arena_find_free(Arena* arena, size_t bin, size_t n, size_t bytes) 
{
    // iterative over free areas to find a possible fit
    Slot* prev_slot = &arena->head_slot;
    Slot* curr_slot = arena->begin() + prev_slot->next;

    while (curr_slot != arena->end() && curr_slot->size < n) {
        prev_slot = curr_slot;
        curr_slot = arena->begin() + curr_slot->next;
    }

    // if curr_slot == end, then no suitable continuous area was found.
    if (WALLE_UNLIKELY(curr_slot == arena->end()))
        return nullptr;

    arena->free_size -= n;

    prev_slot->next += n;
    _size += n;
    _free -= n;

    if (curr_slot->size > n) {
        // splits free area, since it is larger than needed
        Slot* next_slot = arena->begin() + prev_slot->next;
        assert(next_slot != arena->end());

        next_slot->size = curr_slot->size - n;
        next_slot->next = curr_slot->next;
    } else {
        // join used areas
        prev_slot->next = curr_slot->next;
    }

    if (arena->free_size < bin_lower_bound(bin) && !arena->oversize) {
        // recategorize bin into smaller chain.

        size_t new_bin = calc_bin_for_size(arena->free_size);

        if (kDebug) {
            std::cout << "Recategorize arena, previous free "
                      << arena->free_size + n
                      << " now free " << arena->free_size
                      << " from bin " << bin
                      << " to bin " << new_bin
                      << std::endl;
        }
        assert(bin != new_bin);

        // splice out arena from current bin
        if (arena->prev_arena)
            arena->prev_arena->next_arena = arena->next_arena;
        else
            _arena_bin[bin] = arena->next_arena;

        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        // insert at top of new bin
        arena->prev_arena = nullptr;
        arena->next_arena = _arena_bin[new_bin];
        if (_arena_bin[new_bin])
            _arena_bin[new_bin]->prev_arena = arena;
        _arena_bin[new_bin] = arena;
    }

    // allocate more sparse memory
    while (_free < _min_free) {
        if (!allocate_free_arena(_default_arena_size, false)) 
            break;
    }

    if (kDebugCheckPairing) {
        size_t i;
        for (i = 0; i < _allocs.size(); ++i) {
            if (_allocs[i].first == nullptr) {
                _allocs[i].first = curr_slot;
                _allocs[i].second = bytes;
                break;
            }
        }
        if (i == _allocs.size()) {
            assert(!"Increase allocs array in Pool().");
            abort();
        }
    }

    return reinterpret_cast<void*>(curr_slot);
}


Pool::Arena* Pool::allocate_free_arena(size_t arena_size, bool die_on_failure) 
{

    if (kDebug) {
        std::cout << "allocate_free_arena()"
                  << " arena_size=" << arena_size
                  << " die_on_failure=" << die_on_failure << std::endl;
    }

    // Allocate space for the new block
    Arena* new_arena =
        reinterpret_cast<Arena*>(
            bypass_aligned_alloc(_default_arena_size, arena_size));
    if (!new_arena) {
        if (!die_on_failure) {
            return nullptr;
        }
        ::fprintf(stderr, "out-of-memory - mem::Pool cannot allocate a new Arena."
                " _size=%zu\n", _size);
        abort();
    }

    WALLE_ASSERT(
        new_arena ==
        reinterpret_cast<Arena*>(
            reinterpret_cast<uintptr_t>(new_arena) & ~(_default_arena_size - 1)), "");

    new_arena->magic = 0xAEEAAEEAAEEAAEEALLU;
    new_arena->total_size = arena_size;

    // put new area into right chain at the front
    Arena** root;
    if (arena_size <= _default_arena_size) {
        size_t bin = calc_bin_for_size(new_arena->num_slots());
        WALLE_ASSERT(bin < KnumBins, "");
        root = &_arena_bin[bin];
        new_arena->oversize = false;
    } else {
        root = &_arena_bin[KnumBins];
        new_arena->oversize = true;
    }

    new_arena->prev_arena = nullptr;
    new_arena->next_arena = *root;
    if (*root)
        (*root)->prev_arena = new_arena;
    *root = new_arena;

    new_arena->free_size = new_arena->num_slots();
    new_arena->head_slot.next = 0;

    new_arena->slot(0)->size = new_arena->num_slots();
    new_arena->slot(0)->next = new_arena->num_slots();

    _free += new_arena->num_slots();

    Arena* check_arena =
        reinterpret_cast<Arena*>(
            reinterpret_cast<uintptr_t>(new_arena) & ~(_default_arena_size - 1));
    WALLE_ASSERT(check_arena->magic == 0xAEEAAEEAAEEAAEEALLU, "");

    return new_arena;
}

void Pool::deallocate_all() 
{
    walle::UniqueLock<walle::Mutex> lock(_mutex);
    int_deallocate_all();
}

void Pool::int_deallocate_all() 
{
    for (size_t i = 0; i <= KnumBins; ++i) {
        Arena* curr_arena = _arena_bin[i];
        while (curr_arena != nullptr) {
            Arena* next_arena = curr_arena->next_arena;
            bypass_aligned_free(curr_arena, curr_arena->total_size);
            curr_arena = next_arena;
        }
    }
    _min_free = 0;
}

size_t Pool::max_size() const noexcept 
{
    return sizeof(Slot) * std::numeric_limits<uint32_t>::max();
}

size_t Pool::bytes_per_arena(size_t arena_size) 
{
    return arena_size - sizeof(Arena);
}


void* Pool::allocate(size_t bytes) {
    // return malloc(bytes);

    walle::UniqueLock<walle::Mutex> lock(_mutex);

    if (kDebug) {
        std::cout << "Pool::allocate() bytes " << bytes
                  << std::endl;
    }

    if (bytes <= 32)
        return _object_32->allocate();
    if (bytes <= 64)
        return _object_64->allocate();
    if (bytes <= 128)
        return _object_128->allocate();
    if (bytes <= 256)
        return _object_256->allocate();

    // round up to whole slot size, and divide by slot size
    uint32_t n =
        static_cast<uint32_t>((bytes + sizeof(Slot) - 1) / sizeof(Slot));

    // check whether n is too large for allocation in our default Arenas, then
    // allocate a special larger one.
    if (n * sizeof(Slot) > bytes_per_arena(_default_arena_size)) {
        if (kDebug) {
            std::cout << "Allocate overflow arena of size "
                      << n * sizeof(Slot) << std::endl;
        }
        Arena* sp_arena = allocate_free_arena(sizeof(Arena) + n * sizeof(Slot));

        void* ptr = arena_find_free(sp_arena, KnumBins, n, bytes);
        if (ptr != nullptr)
            return ptr;
    }

    // find bin for n slots
    size_t bin = calc_bin_for_size(n);
    while (bin < KnumBins) {
        if (kDebug)
            std::cout << "Searching in bin " << bin << std::endl;

        Arena* curr_arena = _arena_bin[bin];

        while (curr_arena != nullptr) {
            // find an arena with at least n free slots
            if (curr_arena->free_size >= n) {
                void* ptr = arena_find_free(curr_arena, bin, n, bytes);
                if (ptr != nullptr)
                    return ptr;
            }

            // advance to next arena in free list order
            curr_arena = curr_arena->next_arena;
        }

        // look into larger bin
        ++bin;
    }

    // allocate new arena with default size
    Arena* curr_arena = allocate_free_arena(_default_arena_size);
    bin = calc_bin_for_size(curr_arena->num_slots());

    // look into new arena
    void* ptr = arena_find_free(curr_arena, bin, n, bytes);
    if (ptr != nullptr)
        return ptr;

    WALLE_ASSERT(false, "Pool::allocate() failed, no memory available.");
}

void Pool::deallocate(void* ptr, size_t bytes) 
{
    // return free(ptr);

    if (ptr == nullptr) 
        return;

    walle::UniqueLock<walle::Mutex> lock(_mutex);
    if (kDebug) {
        std::cout << "Pool::deallocate() ptr " << ptr
                  << " bytes " << bytes << std::endl;
    }

    if (bytes <= 32){
        return _object_32->deallocate(ptr);
    }
    if (bytes <= 64) {
        return _object_64->deallocate(ptr);
    }

    if (bytes <= 128) {
        return _object_128->deallocate(ptr);
    }
    if (bytes <= 256) {
        return _object_256->deallocate(ptr);
    }

    if (kDebugCheckPairing) {
        size_t i;
        for (i = 0; i < _allocs.size(); ++i) {
            if (_allocs[i].first != ptr) 
                continue;
            if (bytes != _allocs[i].second) {
                assert(!"Mismatching deallocate() size in Pool().");
                abort();
            }
            _allocs[i].first = nullptr;
            _allocs[i].second = 0;
            break;
        }
        if (i == _allocs.size()) {
            assert(!"Unknown deallocate() in Pool().");
            abort();
        }
    }

    // round up to whole slot size, and divide by slot size
    uint32_t n
        = static_cast<uint32_t>((bytes + sizeof(Slot) - 1) / sizeof(Slot));
    assert(n <= _size);

    // find arena containing ptr
    Arena* arena =
        reinterpret_cast<Arena*>(
            reinterpret_cast<uintptr_t>(ptr) & ~(_default_arena_size - 1));
    WALLE_ASSERT(arena->magic == 0xAEEAAEEAAEEAAEEALLU, "");

    if (!(ptr >= arena->begin() && ptr < arena->end())) {
        assert(!"deallocate() of memory not in any arena.");
        abort();
    }

    Slot* prev_slot = &arena->head_slot;
    Slot* ptr_slot = reinterpret_cast<Slot*>(ptr);

    // advance prev_slot until the next jumps over ptr.
    while (arena->begin() + prev_slot->next < ptr_slot) {
        prev_slot = arena->begin() + prev_slot->next;
    }

    // fill deallocated slot with free information
    ptr_slot->next = prev_slot->next;
    ptr_slot->size = n;

    prev_slot->next = static_cast<uint32_t>(ptr_slot - arena->begin());

    // defragment free slots, but exempt the head_slot
    if (prev_slot == &arena->head_slot)
        prev_slot = arena->begin() + arena->head_slot.next;

    while (prev_slot->next != arena->num_slots() &&
           prev_slot->next == prev_slot - arena->begin() + prev_slot->size)
    {
        // join free slots
        Slot* next_slot = arena->begin() + prev_slot->next;
        prev_slot->size += next_slot->size;
        prev_slot->next = next_slot->next;
    }

    arena->free_size += n;
    _size -= n;
    _free += n;

    // always deallocate oversize arenas
    if (arena->oversize) {
        if (kDebug)
            std::cout << "destroy special arena" << std::endl;

        // splice out arena from current bin
        if (arena->prev_arena)
            arena->prev_arena->next_arena = arena->next_arena;
        else
            _arena_bin[KnumBins] = arena->next_arena;

        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        _free -= arena->num_slots();
        bypass_aligned_free(arena, arena->total_size);
        return;
    }

    // check if this arena is empty and free_ space is beyond our _min_free
    // limit, then simply deallocate it.
    if (arena->free_size == arena->num_slots() &&
        _free >= _min_free + arena->num_slots())
    {
        if (kDebug)
            std::cout << "destroy empty arena" << std::endl;

        size_t bin = calc_bin_for_size(arena->free_size - n);

        // splice out arena from current bin
        if (arena->prev_arena)
            arena->prev_arena->next_arena = arena->next_arena;
        else
            _arena_bin[bin] = arena->next_arena;

        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        // free arena
        _free -= arena->num_slots();
        bypass_aligned_free(arena, arena->total_size);
        return;
    }

    if (calc_bin_for_size(arena->free_size - n) !=
        calc_bin_for_size(arena->free_size))
    {
        // recategorize arena into larger chain.
        if (kDebug)
            std::cout << "recategorize arena into larger chain." << std::endl;

        size_t bin = calc_bin_for_size(arena->free_size - n);
        size_t new_bin = calc_bin_for_size(arena->free_size);

        if (kDebug) {
            std::cout << "Recategorize arena, previous free "
                      << arena->free_size
                      << " now free " << arena->free_size + n
                      << " from bin " << bin
                      << " to bin " << new_bin
                      << std::endl;
        }

        // splice out arena from current bin
        if (arena->prev_arena)
            arena->prev_arena->next_arena = arena->next_arena;
        else
            _arena_bin[bin] = arena->next_arena;

        if (arena->next_arena)
            arena->next_arena->prev_arena = arena->prev_arena;

        // insert at top of new bin
        arena->prev_arena = nullptr;
        arena->next_arena = _arena_bin[new_bin];
        if (_arena_bin[new_bin])
            _arena_bin[new_bin]->prev_arena = arena;
        _arena_bin[new_bin] = arena;
    }
}

void Pool::print(bool debug) {
    if (debug) {
        std::cout << "Pool::print()"
                  << " _size=" << _size << " _free=" << _free << std::endl;
    }

    size_t total_free = 0, total_size = 0;

    for (size_t bin = 0; bin < KnumBins; ++bin)
    {
        for (Arena* curr_arena = _arena_bin[bin]; curr_arena != nullptr;
             curr_arena = curr_arena->next_arena)
        {
            std::ostringstream oss;

            size_t arena_bin = calc_bin_for_size(curr_arena->free_size);
            WALLE_ASSERT(arena_bin == bin, "");

            size_t slot = curr_arena->head_slot.next;
            size_t size = 0, free = 0;

            // used area at beginning
            size += slot;

            while (slot != curr_arena->num_slots()) {
                if (debug)
                    oss << " slot[" << slot
                        << ",size=" << curr_arena->slot(slot)->size
                        << ",next=" << curr_arena->slot(slot)->next << ']';

                if (curr_arena->slot(slot)->next <= slot) {
                    std::cout << "invalid chain:" << oss.str() << std::endl;
                    abort();
                }

                free += curr_arena->slot(slot)->size;
                size += curr_arena->slot(slot)->next - slot - curr_arena->slot(slot)->size;
                slot = curr_arena->slot(slot)->next;
            }

            if (debug) {
                std::cout << "arena[" << bin << ':' << curr_arena << "]"
                          << " free_size=" << curr_arena->free_size
                          << " head_slot.next=" << curr_arena->head_slot.next
                          << oss.str()
                          << std::endl;
            }

            WALLE_ASSERT(curr_arena->head_slot.size == free, "");

            total_free += free;
            total_size += size;

            if (curr_arena->next_arena)
                WALLE_ASSERT(curr_arena->next_arena->prev_arena == curr_arena, "");
            if (curr_arena->prev_arena)
                WALLE_ASSERT(curr_arena->prev_arena->next_arena == curr_arena, "");
        }
    }

    WALLE_ASSERT(total_size == _size, "");
    WALLE_ASSERT(total_free == _free, "");
}

void Pool::self_verify() 
{
    print(false);
}
} //namespace mem
} //namespace walle