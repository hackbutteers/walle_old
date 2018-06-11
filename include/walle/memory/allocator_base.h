#ifndef WALLE_MEMORY_ALLOCATOR_BASE_H_
#define WALLE_MEMORY_ALLOCATOR_BASE_H_
#include <walle/config/base.h>
#include <walle/memory/manager.h>
#include <walle/memory/malloc_tracker.h>
#include <cstddef>
#include <memory>
#include <new>
#include <deque>
#include <type_traits>

namespace walle {
namespace mem_detail {
template <typename Type>
class AllocatorBase {
public:
    typedef Type   value_type;
    typedef Type * pointer;
    typedef const Type *  const_pointer;
    typedef Type &  reference;
    typedef const Type &  const_reference;
    typedef std::size_t   size_type;
    typedef std::ptrdiff_t difference_type;
    

    //! C++11 type flag
    using is_always_equal = std::true_type;
    //! C++11 type flag
    using propagate_on_container_move_assignment = std::true_type;

    //! Returns the address of x.
    pointer address(reference x) const WALLE_NOEXCEPT 
    {
        return std::addressof(x);
    }

    //! Returns the address of x.
    const_pointer address(const_reference x) const WALLE_NOEXCEPT 
    {
        return std::addressof(x);
    }

    //! Maximum size possible to allocate
    size_type max_size() const WALLE_NOEXCEPT 
    {
        return size_t(-1) / sizeof(Type);
    }

    //! Constructs an element object on the location pointed by p.
    void construct(pointer p, const_reference value) 
    {
        ::new ((void*)p)Type(value); // NOLINT
    }

    //! Destroys in-place the object pointed by p.
    void destroy(pointer p) const WALLE_NOEXCEPT 
    {
        p->~Type();
    }

    //! Constructs an element object on the location pointed by p.
    template <typename SubType, typename... Args>
    void construct(SubType* p, Args&& ... args) 
    {
        ::new ((void*)p)SubType(std::forward<Args>(args) ...); // NOLINT
    }

    //! Destroys in-place the object pointed by p.
    template <typename SubType>
    void destroy(SubType* p) const noexcept 
    {
        p->~SubType();
    }
};
}

namespace mem {

template <typename Type, walle::mem::ManagerPtr &_manager>
class FixedAllocator : public walle::mem_detail::AllocatorBase<Type> {
private:
    static constexpr bool debug = false;

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

    //! Return allocator for different type.
    template <typename U>
    struct rebind { using other = FixedAllocator<U, _manager>; };

    //! default constructor
    FixedAllocator() noexcept = default;

    //! copy-constructor
    FixedAllocator(const FixedAllocator&) noexcept = default;

    //! copy-constructor from a rebound allocator
    template <typename OtherType>
    FixedAllocator(const FixedAllocator<OtherType, _manager>&) noexcept
    { }


    FixedAllocator& operator = (FixedAllocator&) noexcept = default;

    //! move-assignment operator: default
    FixedAllocator& operator = (FixedAllocator&&) noexcept = default;

    //! Attempts to allocate a block of storage with a size large enough to
    //! contain n elements of member type value_type, and returns a pointer to
    //! the first element.
    pointer allocate(size_type n, const void* /* hint */ = nullptr) 
    {
        if (n > this->max_size())
            throw std::bad_alloc();

        const size_t size = n * sizeof(Type);
        _manager->add(size);

        if (debug) {
            printf("allocate() n=%zu sizeof(T)=%zu total=%zu\n",
                   n, sizeof(Type), _manager->total());
        }

        Type* r = static_cast<Type*>(bypass_malloc(size));
        while (r == nullptr) {
            // If malloc fails and there is a std::new_handler, call it to try
            // free up memory.
            std::new_handler nh = std::get_new_handler();
            if (!nh)
                throw std::bad_alloc();
            nh();
            r = static_cast<Type*>(bypass_malloc(size));
        }
        return r;
    }

    //! Releases a block of storage previously allocated with member allocate
    //! and not yet released.
    void deallocate(pointer p, size_type n) const noexcept 
    {

        _manager->sub(n * sizeof(Type));

        if (debug) {
            printf("deallocate() n=%zu sizeof(T)=%zu total=%zu\n",
                   n, sizeof(Type), _manager->total());
        }

        bypass_free(p, n * sizeof(Type));
    }

    //! Compare to another allocator of same type
    template <typename Other>
    bool operator == (const FixedAllocator<Other, _manager>&) const noexcept {
        return true;
    }

    //! Compare to another allocator of same type
    template <typename Other>
    bool operator != (const FixedAllocator<Other, _manager>&) const noexcept {
        return true;
    }
};

template <ManagerPtr& _manager>
class FixedAllocator<void, _manager>
{
public:
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    template <typename U>
    struct rebind { using other = FixedAllocator<U, _manager>; };
};

/******************************************************************************/
// BypassAllocator

//! global bypass memory manager
extern ManagerPtr g_bypass_manager;

//! instantiate FixedAllocator as BypassAllocator
template <typename Type>
using BypassAllocator = FixedAllocator<Type, g_bypass_manager>;

//! operator new with our Allocator
template <typename T, typename... Args>
T * by_new(Args&& ... args) 
{
    BypassAllocator<T> allocator;
    T* value = allocator.allocate(1);
    allocator.construct(value, std::forward<Args>(args) ...);
    return value;
}

//! operator delete with our Allocator
template <typename T>
void by_delete(T* value) 
{
    BypassAllocator<T> allocator;
    allocator.destroy(value);
    allocator.deallocate(value, 1);
}

using by_string = std::basic_string<
          char, std::char_traits<char>, BypassAllocator<char> >;

using by_stringbuf = std::basic_stringbuf<
          char, std::char_traits<char>, BypassAllocator<char> >;

template <typename T>
using by_vector = std::vector<T, BypassAllocator<T> >;

template <typename T>
using by_deque = std::deque<T, BypassAllocator<T> >;

}
} //namspace walle
#endif //WALLE_MEMORY_ALLOCATOR_BASE_H_