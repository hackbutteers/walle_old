#ifndef WALLE_MEMORY_POOL_H_
#define WALLE_MEMORY_POOL_H_
#include <walle/memory/allocator_base.h>
#include <walle/config/base.h>
#include <walle/sys/mutex.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace walle {
namespace mem {

class Pool {
public:
    explicit Pool(size_t default_arena_size = 16384) WALLE_NOEXCEPT;

    ~Pool() WALLE_NOEXCEPT;

    /** 
     * @brief  allocate a continuous segment of n bytes in the arenas
     * @note   
     * @param  bytes: 
     * @retval 
     */
    void * allocate(size_t bytes);

    /** 
     * @brief   Deallocate a continuous segment of n bytes in the arenas, the size n
     *          *MUST* match the allocation.
     * @note   
     * @param  ptr: 
     * @param  bytes: 
     * @retval None
     */
    void deallocate(void* ptr, size_t bytes);

    /** 
     * @brief  Allocate and construct a single item of given Type using memory from the
     * Pool.
     * @note   
     * @param  args: 
     * @retval 
     */
    template <typename Type, typename... Args>
    Type * make(Args&& ... args) 
    {
        Type* t = reinterpret_cast<Type*>(allocate(sizeof(Type)));
        ::new (t)Type(std::forward<Args>(args) ...);
        return t;
    }

    /** 
     * @brief  Destroy and deallocate a single item of given Type.
     * @note   
     * @param  t: 
     * @retval None
     */
    template <typename Type>
    void destroy(Type* t) 
    {
        t->~Type();
        deallocate(t, sizeof(Type));
    }

    void print(bool debug = true);

    /** 
     * @brief  
     * @note   
     * @retval None
     */
    void self_verify();

    /** 
     * @brief  maximum size possible to allocate
     * @note   
     * @retval 
     */
    size_t max_size() const WALLE_NOEXCEPT;

    /** 
     * @brief  deallocate all Arenas
     * @note   
     * @retval None
     */
    void deallocate_all();
private:
    struct Slot;

    /** 
     * @brief  header of an Arena, used to calculate number of slots
     * @note   
     * @retval None
     */
    struct Arena;

    /** 
     * @brief  header of an ObjectArena containing equally sized items 
     * @note   
     * @retval None
     */
    struct ObjectArena;

    /** 
     * @brief pool of equally sized items 
     * @note   
     * @retval None
     */
    class ObjectPool;
private:
    /** 
     * @brief  calculate maximum bytes fitting into an Arena with given size.
     * @note   
     * @param  arena_size: 
     * @retval 
     */
    size_t bytes_per_arena(size_t arena_size);

    /** 
     * @brief  allocate a new Arena blob
     * @note   
     * @param  arena_size: 
     * @param  die_on_failure: 
     * @retval 
     */
     Arena * allocate_free_arena(size_t arena_size, bool die_on_failure = true);

    /** 
     * @brief  find free area inside an Arena
     * @note   
     * @param  curr_arena: 
     * @param  bin: 
     * @param  n: 
     * @param  bytes: 
     * @retval 
     */
    void * arena_find_free(Arena* curr_arena, size_t bin, size_t n, size_t bytes);

    /** 
     * @brief  deallocate all Arenas
     * @note   
     * @retval None
     */
    void int_deallocate_all();
private:
    static const bool kDebug = false;
    static const bool kDebugVerify = false;
    //! debug flag to check pairing of allocate()/deallocate() client calls
    static const bool kDebugCheckPairing = false;
    static const size_t kCheckLimit = 4 * 1024 * 1024;
    //! log_2(16384) = 14 log_2(sizeof(Slot))
    static const size_t KnumBins = 14 - 3 + 1;
    WALLE_NON_COPYABLE(Pool);
private:
    walle::Mutex    _mutex;
    Arena          *_arena_bin[KnumBins + 1];
    size_t          _free = 0;
    size_t          _size = 0;
    size_t          _default_arena_size;
    size_t          _min_free = 1024 * 1024 / 8;

    ObjectPool     *_object_32;
    ObjectPool     *_object_64;
    ObjectPool     *_object_128;
    ObjectPool     *_object_256;
    std::vector<std::pair<void*, size_t> > _allocs;

};


Pool& GPool();

template <typename Type>
class PoolAllocator : public walle::mem_detail::AllocatorBase<Type> {
public:
    using value_type = Type;
    using pointer = Type *;
    using const_pointer = const Type *;
    using reference = Type &;
    using const_reference = const Type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //! C++11 type flag
    using is_always_equal = std::false_type;

    template <typename U>
    struct rebind { using other = PoolAllocator<U>; };

    explicit PoolAllocator(Pool& pool) WALLE_NOEXCEPT
        : _pool(&pool) 
    { 

    }

    PoolAllocator(const PoolAllocator&) WALLE_NOEXCEPT = default;

    template <typename OtherType>
    PoolAllocator(const PoolAllocator<OtherType>& other) WALLE_NOEXCEPT
        : _pool(other._pool) { }

    PoolAllocator& operator = (const PoolAllocator&) WALLE_NOEXCEPT = default;

    size_type max_size() const WALLE_NOEXCEPT 
    {
        return _pool->max_size();
    }

    pointer allocate(size_type n, const void* /* hint */ = nullptr) 
    {
        return static_cast<Type*>(_pool->allocate(n * sizeof(Type)));
    }

    void deallocate(pointer p, size_type n) const WALLE_NOEXCEPT 
    {
        _pool->deallocate(p, n * sizeof(Type));
    }

    Pool *_pool;

    template <typename Other>
    bool operator == (const PoolAllocator<Other>& other) const WALLE_NOEXCEPT 
    {
        return (_pool == other._pool);
    }

    template <typename Other>
    bool operator != (const PoolAllocator<Other>& other) const WALLE_NOEXCEPT 
    {
        return (_pool != other._pool);
    }
};

template <typename Type, Pool& (* _pool)()>
class FixedPoolAllocator : public walle::mem_detail::AllocatorBase<Type>{
public:
    using value_type = Type;
    using pointer = Type *;
    using const_pointer = const Type *;
    using reference = Type &;
    using const_reference = const Type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //! C++11 type flag
    using is_always_equal = std::true_type;

    /** 
     * @brief  Return allocator for different type.
     * @note   
     * @retval None
     */
    template <typename U>
    struct rebind { 
        using other = FixedPoolAllocator<U, _pool>; 
    };

    /** 
     * @brief construct FixedPoolAllocator with Pool object 
     * @note   
     * @retval 
     */
    FixedPoolAllocator() WALLE_NOEXCEPT = default;

    FixedPoolAllocator(const FixedPoolAllocator&) WALLE_NOEXCEPT = default;

    /** 
     * @brief copy-constructor from a rebound allocator 
     * @note   
     * @param  FixedPoolAllocator<OtherType: 
     * @retval 
     */
    template <typename OtherType>
    FixedPoolAllocator(const FixedPoolAllocator<OtherType, _pool>&) WALLE_NOEXCEPT 
    { 

    }

    /** 
     * @brief copy-assignment operator 
     * @note   
     * @param  FixedPoolAllocator&: 
     * @retval None
     */
    FixedPoolAllocator& operator = (const FixedPoolAllocator&) WALLE_NOEXCEPT = default;

    /** 
     * @brief  maximum size possible to allocate
     * @note   
     * @retval 
     */
    size_type max_size() const WALLE_NOEXCEPT 
    {
        return _pool().max_size();
    }

    /** 
     * @brief  attempts to allocate a block of storage with a size large enough to 
     *         contain n elements of member type value_type, and returns a pointer to
     *         the first element.
     * @note   
     * @param  n: 
     * @param  void*: 
     * @retval 
     */
    pointer allocate(size_type n, const void* /* hint */ = nullptr) 
    {
        return static_cast<Type*>(_pool().allocate(n * sizeof(Type)));
    }

    /** 
     * @brief releases a block of storage previously allocated with member allocate 
     *        and not yet released.
     * @note   
     * @param  p: 
     * @param  n: 
     * @retval None
     */
    void deallocate(pointer p, size_type n) const WALLE_NOEXCEPT 
    {
        _pool().deallocate(p, n * sizeof(Type));
    }

    /** 
     * @brief compare to another allocator of same type
     * @note   
     * @param  FixedPoolAllocator<Other: 
     * @retval None
     */
    template <typename Other>
    bool operator == (const FixedPoolAllocator<Other, _pool>&) const WALLE_NOEXCEPT 
    {
        return true;
    }

    /** 
     * @brief compare to another allocator of same type 
     * @note   
     * @param  FixedPoolAllocator<Other: 
     * @retval None
     */
    template <typename Other>
    bool operator != (const FixedPoolAllocator<Other, _pool>&) const WALLE_NOEXCEPT 
    {
        return true;
    }

};

template <typename Type>
using GPoolAllocator = FixedPoolAllocator<Type, GPool>;

template <typename T>
class GPoolDeleter {
public:
    //! free the pointer
    void operator () (T* ptr) const noexcept {
        GPool().destroy(ptr);
    }
};

template <typename T>
using safe_unique_ptr = std::unique_ptr<T, GPoolDeleter<T> >;

template <typename T, typename... Args>
safe_unique_ptr<T> safe_make_unique(Args&& ... args) 
{
    return safe_unique_ptr<T>(
        GPool().make<T>(std::forward<Args>(args) ...));
}

using safe_string = std::basic_string<
          char, std::char_traits<char>, GPoolAllocator<char> >;

} //namespace mem
} //namespace walle
#endif //WALLE_MEMORY_POOL_H_