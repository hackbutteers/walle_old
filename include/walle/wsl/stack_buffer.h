#ifndef WALLE_WSL_STACK_BUFFER_H_
#define WALLE_WSL_STACK_BUFFER_H_
#include <walle/wsl/internal/basic_buffer.h>

namespace wsl {

template<typename T, size_t SIZE, typename Allocator = std::allocator<T>>
class stack_buffer :public wsl::internal::basic_buffer<T>{
public:
    typedef wsl::internal::basic_buffer<T>              base_type;
    typedef stack_buffer<T, SIZE, Allocator>		    this_type;
    typedef T 											value_type;
    typedef T* 											pointer;
    typedef const T* 									const_pointer;
    typedef T& 											reference;
    typedef const T& 									const_reference;
    typedef T* 											iterator;
    typedef const T* 									const_iterator;
    typedef std::reverse_iterator<iterator> 			reverse_iterator;
    typedef std::reverse_iterator<const_iterator> 	const_reverse_iterator;
    typedef size_t 										size_type;
    typedef ptrdiff_t 									difference_type;
    typedef Allocator                                   allocator_type;

    static const WALLE_CONSTEXPR size_type npos = size_type(-1); 
public:
    stack_buffer():base_type(_data, 0, SIZE)
    {

    }
    stack_buffer(stack_buffer &&other)
    {
        move(std::forward(other));
    }

    stack_buffer &operator=(stack_buffer &&other)
    {
        WALLE_ASSERT_MSG(this != &other, "this = this");
        deallocate();
        move(std::forward(other));
        return *this;
    }

    size_t stack_size() const
    {
        return SIZE;
    }

    bool is_stack() const 
    {
        return this->_ptr == _data;
    }

protected:
    void grow(std::size_t size);

private:
    void deallocate()
    {
        if (this->_ptr != _data) {
            _alloc.deallocate(this->_ptr, this->_capacity);
        }
    }

    void move(stack_buffer &&other)
    {
        allocator_type &this_alloc = _alloc;
        allocator_type &other_alloc = other._alloc;
        this_alloc = std::move(other_alloc);
        this->_size = other._size;
        this->_capacity = other._capacity;
        if (other._ptr == other._data) {
            this->_ptr = _data;
            std::uninitialized_copy(other._data, other._data + this->_size, _data);
        } else {
            this->_ptr = other._ptr;
            // Set pointer to the inline array so that delete is not called
            // when deallocating.
            other._ptr = other._data;
        }
    }

private:
    allocator_type  _alloc;
    T               _data[SIZE];

};

template <typename T, std::size_t SIZE, typename Allocator>
void stack_buffer<T, SIZE, Allocator>::grow(std::size_t len)
{
    std::size_t new_capacity = this->_capacity + this->_capacity / 2;
    if (len > new_capacity)
        new_capacity = len;
    T *new_ptr = this->_alloc.allocate(new_capacity);
    // The following code doesn't throw, so the raw pointer above doesn't leak.
    std::uninitialized_copy(this->_ptr, this->_ptr + this->_size, new_ptr);
    std::size_t old_capacity = this->_capacity;
    T *old_ptr = this->_ptr;
    this->_capacity = new_capacity;
    this->_ptr = new_ptr;
    // deallocate may throw (at least in principle), but it doesn't matter since
    // the buffer already uses the new storage and will deallocate it in case
    // of exception.
    if (old_ptr != _data) {
        _alloc.deallocate(old_ptr, old_capacity);
    }
}

}
#endif //WALLE_WSL_STACK_BUFFER_H_
