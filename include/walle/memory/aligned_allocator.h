#ifndef WALLE_MEM_ALIGNED_ALLOCATOR_H_
#define WALLE_MEM_ALIGNED_ALLOCATOR_H_
#include <walle/memory/allocator_base.h>
#include <walle/common/inner_log.h>
#include <walle/memory/manager.h>
#include <cassert>
#include <cstdlib>
#include <memory>
namespace walle {
namespace mem {

#define WALLE_DEFAULT_ALIGN 4096
template <typename MustBeInt>
struct AlignedAllocatorSettings {
    static bool may_use_realloc;
};

template <typename MustBeInt>
bool AlignedAllocatorSettings<MustBeInt>::may_use_realloc = false;

template <typename Type = char,
          typename BaseAllocator = std::allocator<char>,
          size_t Alignment = WALLE_DEFAULT_ALIGN>
class AlignedAllocator : public walle::mem_detail::AllocatorBase<Type> {
    static constexpr bool debug = false;

    static_assert(sizeof(typename BaseAllocator::value_type) == 1,
                  "BaseAllocator must be a char/byte allocator");

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

    //! Return allocator for different type.
    template <typename U>
    struct rebind { using other = AlignedAllocator<U, BaseAllocator>; };

    //! Construct with base allocator
    explicit AlignedAllocator(const BaseAllocator& base = BaseAllocator())
        : _base(base) { }

    //! copy-constructor
    AlignedAllocator(const AlignedAllocator&) noexcept = default;

    //! copy-constructor from a rebound allocator
    template <typename OtherType>
    AlignedAllocator(const AlignedAllocator<OtherType>& other) noexcept
        : _base(other.base()) { }

    //! copy-assignment operator
    AlignedAllocator& operator = (const AlignedAllocator&) noexcept = default;

    //! Attempts to allocate a block of storage with a size large enough to
    //! contain n elements of member type value_type, and returns a pointer to
    //! the first element.
    pointer allocate(size_type n, const void* /* hint */ = nullptr) 
    {
        if (n > this->max_size())
            throw std::bad_alloc();

        return static_cast<pointer>(allocate_bytes(n * sizeof(Type)));
    }

    //! Releases a block of storage previously allocated with member allocate
    //! and not yet released.
    void deallocate(pointer p, size_type n) noexcept {
        deallocate_bytes(p, n * sizeof(Type));
    }

    //! Compare to another allocator of same type
    template <typename Other>
    bool operator == (const AlignedAllocator<Other>& other) const noexcept 
    {
        return (_base == other._base);
    }

    //! Compare to another allocator of same type
    template <typename Other>
    bool operator != (const AlignedAllocator<Other>& other) const noexcept {
        return (_base != other._base);
    }

    /**************************************************************************/

    void * allocate_bytes(size_t size, size_t meta_info_size = 0);
    void deallocate_bytes(void* ptr, size_t size, size_t meta_info_size = 0) noexcept;

    const BaseAllocator& base() const { return _base; }

private:
    //! base allocator
    BaseAllocator _base;
};

// meta_info_size > 0 is needed for array allocations that have overhead
//
//                      meta_info
//                          aligned begin of data   unallocated behind data
//                      v   v                       v
//  ----===============#MMMM========================------
//      ^              ^^                           ^
//      buffer          result                      result+m_i_size+size
//                     pointer to buffer
// (---) unallocated, (===) allocated memory

template <typename Type, typename BaseAllocator, size_t Alignment>
inline void* AlignedAllocator<Type, BaseAllocator, Alignment>::allocate_bytes(
    size_t size, size_t meta_info_size) 
{
    // malloc()/realloc() variant that frees the unused amount of memory
    // after the data area of size 'size'. realloc() from valgrind does not
    // preserve the old memory area when shrinking, so out-of-bounds
    // accesses can't be detected easily.
    // Overhead: about Alignment bytes.
    size_t alloc_size = Alignment + sizeof(char*) + meta_info_size + size;
    char* buffer = reinterpret_cast<char*>(_base.allocate(alloc_size));

    if (buffer == nullptr)
        return nullptr;

    char* reserve_buffer = buffer + sizeof(char*) + meta_info_size;
    char* result = reserve_buffer + Alignment -
                   (((size_t)reserve_buffer) % (Alignment)) - meta_info_size;

    // -tb: check that there is space for one char* before the "result" pointer
    // delivered to the user. this char* is set below to the beginning of the
    // allocated area.
    assert(long(result - buffer) >= long(sizeof(char*)));
    *((reinterpret_cast<char**>(result)) - 1) = buffer;
    return result;
}

template <typename Type, typename BaseAllocator, size_t Alignment>
inline void AlignedAllocator<Type, BaseAllocator, Alignment>::deallocate_bytes(
    void* ptr, size_t size, size_t meta_info_size) noexcept 
{
    if (!ptr)
        return;
    char* buffer = *((reinterpret_cast<char**>(ptr)) - 1);
    size_t alloc_size = Alignment + sizeof(char*) + meta_info_size + size;
    _base.deallocate(buffer, alloc_size);
}

/******************************************************************************/
// default aligned allocation methods

static inline
void * aligned_alloc(size_t size, size_t meta_info_size = 0) 
{
    return AlignedAllocator<>().allocate_bytes(size, meta_info_size);
}

static inline
void aligned_dealloc(void* ptr, size_t size, size_t meta_info_size = 0) 
{
    return AlignedAllocator<>().deallocate_bytes(ptr, size, meta_info_size);
}
}
}
#endif