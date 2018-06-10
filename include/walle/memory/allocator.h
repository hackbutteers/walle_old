#ifndef WALLE_MEMORY_ALLOCATOR_H_
#define WALLE_MEMORY_ALLOCATOR_H_
#include <walle/memory/allocator_base.h>
#include <walle/memory/manager.h>
#include <walle/config/base.h>
#include <deque>
#include <iosfwd>
#include <memory>
#include <new>
#include <string>
#include <type_traits>
#include <vector>

namespace walle {
namespace mem {
template <typename Type>
class Allocator : public walle::mem_detail::AllocatorBase<Type> {
public:
    static bool debug;
    typedef Type   value_type;
    typedef Type * pointer;
    typedef const Type *  const_pointer;
    typedef Type &  reference;
    typedef const Type &  const_reference;
    typedef std::size_t   size_type;
    typedef std::ptrdiff_t difference_type;
    using is_always_equal = std::false_type;

    template <typename U>
    struct rebind { using other = Allocator<U>; };

    explicit Allocator(const ManagerPtr &manager) noexcept
        : _manager(manager) { }

    Allocator(const Allocator&) noexcept = default;

    template <typename OtherType>
    Allocator(const Allocator<OtherType>& other) noexcept
        : _manager(other._manager) { }

    Allocator& operator = (const Allocator&) noexcept = default;

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
        while (r == nullptr)
        {
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
    
    void deallocate(pointer p, size_type n) const noexcept {

        _manager->sub(n * sizeof(Type));

        if (debug) {
            printf("deallocate() n=%zu sizeof(T)=%zu total=%zu\n",
                   n, sizeof(Type), _manager->total());
        }

        bypass_free(p, n * sizeof(Type));
    }

    template <typename Other>
    bool operator == (const Allocator<Other>& other) const noexcept 
    {
        return (_manager == other._manager);
    }

    //! Compare to another allocator of same type
    template <typename Other>
    bool operator != (const Allocator<Other>& other) const noexcept 
    {
        return (_manager != other._manager);
    }
public:
    ManagerPtr _manager;
};
template <typename Type>
bool Allocator<Type>::debug = false;
template <>
class Allocator<void> {
public:
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    //! C++11 type flag
    using is_always_equal = std::false_type;

    template <typename U>
    struct rebind { using other = Allocator<U>; };

    explicit Allocator(const ManagerPtr &manager) noexcept
        : _manager(manager) { }

    Allocator(const Allocator&) noexcept = default;

    template <typename OtherType>
    Allocator(const Allocator<OtherType>& other) noexcept
        : _manager(other._manager) 
    {

    }
    template <typename Other>
    bool operator == (const Allocator<Other>& other) const noexcept 
    {
        return (_manager == other._manager);
    }

    template <typename Other>
    bool operator != (const Allocator<Other>& other) const noexcept 
    {
        return (_manager != other._manager);
    }
public:
    ManagerPtr _manager;
};

//! operator new with our Allocator
template <typename T, typename... Args>
T * mm_new(ManagerPtr& manager, Args&& ... args) 
{
    Allocator<T> allocator(manager);
    T* value = allocator.allocate(1);
    allocator.construct(value, std::forward<Args>(args) ...);
    return value;
}

//! operator delete with our Allocator
template <typename T>
void mm_delete(ManagerPtr& manager, T* value) 
{
    Allocator<T> allocator(manager);
    allocator.destroy(value);
    allocator.deallocate(value, 1);
}

//! std::default_deleter with Manager tracking
template <typename T>
class Deleter {
public:
    //! constructor: need reference to Manager
    explicit Deleter(Manager& manager) noexcept
        : _allocator(manager) 
    {

    }

    //! free the pointer
    void operator () (T* ptr) const noexcept 
    {
        _allocator.destroy(ptr);
        _allocator.deallocate(ptr, 1);
    }

private:
    Allocator<T> _allocator;
};

template <typename T>
using unique_ptr = std::unique_ptr<T, Deleter<T> >;

template <typename T, typename... Args>
unique_ptr<T> make_unique(ManagerPtr& manager, Args&& ... args) 
{
    return unique_ptr<T>(
        mm_new<T>(manager, std::forward<Args>(args) ...),
        Deleter<T>(manager));
}

//! string with Manager tracking
using string = std::basic_string<
          char, std::char_traits<char>, Allocator<char> >;

//! stringbuf with Manager tracking
using stringbuf = std::basic_stringbuf<
          char, std::char_traits<char>, Allocator<char> >;

//! vector with Manager tracking
template <typename T>
using vector = std::vector<T, Allocator<T> >;

//! deque with Manager tracking
template <typename T>
using deque = std::deque<T, Allocator<T> >;
} //namespace mem
} //namespace walle
#endif //WALLE_MEMORY_ALLOCATOR_H_