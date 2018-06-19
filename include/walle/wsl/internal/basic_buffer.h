#ifndef WALLE_WSL_INTERNAL_BASIC_BUFFER_H_
#define WALLE_WSL_INTERNAL_BASIC_BUFFER_H_
#include <walle/config/base.h>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>

namespace wsl {
namespace internal {

template <typename T>
class basic_buffer {
public:
    typedef basic_buffer<T>                             this_type;
    typedef T 											value_type;
    typedef T* 											pointer;
    typedef const T* 									const_pointer;
    typedef T& 											reference;
    typedef const T& 									const_reference;
    typedef T* 											iterator;
    typedef const T* 									const_iterator;
    typedef std::reverse_iterator<iterator> 			reverse_iterator;
    typedef std::reverse_iterator<const_iterator> 	    const_reverse_iterator;
    typedef size_t 										size_type;
    typedef ptrdiff_t 									difference_type;

    static const WALLE_CONSTEXPR size_type npos = size_type(-1); 
public:
    virtual ~basic_buffer()
    {

    }

    /**  
     * @brief returns the size of this buffer. 
     * @note   
     * @retval 
     */
    size_type size() const
    {
        return _size;
    }

    /** Returns the capacity of this buffer.
     * @brief  
     * @note   
     * @retval 
     */
    size_type capacity() const
    {
        return _capacity;
    }

    iterator begin() WALLE_NOEXCEPT { return _ptr; }
    const_iterator begin() const WALLE_NOEXCEPT { return _ptr; }
    iterator end() WALLE_NOEXCEPT { return _ptr + _size; }
    const_iterator end() const WALLE_NOEXCEPT { return _ptr + _size; }
    iterator rbegin() WALLE_NOEXCEPT { return _ptr + _size  - 1; }
    iterator rend() WALLE_NOEXCEPT { return _ptr - 1; }
    const_iterator rbegin() const WALLE_NOEXCEPT { return _ptr + _size  - 1; }
    const_iterator rend() const WALLE_NOEXCEPT { return _ptr - 1; }


    void resize(size_type new_size)
    {
        if (new_size > _capacity)
            grow(new_size);
        _size = new_size;
    }


    void reserve(size_type capacitySize)
    {
        if (capacitySize > _capacity)
            grow(capacitySize);
    }

    void clear()
    {
        _size = 0;
    }

    void bzero()
    {
        memset(_ptr, 0, _capacity * sizeof(T));
        _size = 0;
    }

    void push_back(const T &value)
    {
        if (_size == _capacity)
            grow(_size + 1);
        _ptr[_size++] = value;
    }

    void pop_back()
    {
        if(_size > 0) {
            _size--;
        }
    }

    T* current()
    {
        return _ptr + _size;
    }

    size_type avaible()
    {
        return _capacity - _size;
    }

    /**  Appends data to the end of the buffer. 
     * @brief  
     * @note   
     * @param  *begin: 
     * @param  *end: 
     * @retval None
     */
    template <typename U>
    void append(const U *begin, const U *end);
    template <typename U>
    void append(const U *begin, size_type len);

    T &operator[](size_type index)
    {
        WALLE_ASSERT_MSG(index < _size, "index overflow the buffer memory");
        return _ptr[index];
    }
    const T &operator[](size_type index) const
    {
        WALLE_ASSERT_MSG(index < _size, "index overflow the buffer memory");
        return _ptr[index];
    }

    const T* data() const
    {
        return _ptr;
    }

    T* data()
    {
        return _ptr;
    }
protected:
    T             *_ptr;
    size_type     _size;
    size_type     _capacity;
protected:
    basic_buffer(T *ptr = WALLE_NULL, size_type buffer_size = 0, size_type capacitySize = 0)
        : _ptr(ptr), 
          _size(buffer_size), 
          _capacity(capacitySize)
    {

    }

    void set(value_type *buf_data, size_type buf_capacity) WALLE_NOEXCEPT 
    {
        _ptr = buf_data;
        _capacity = buf_capacity;
    }

    virtual void grow(size_type size) = 0;
};

template <typename T>
template <typename U>
void basic_buffer<T>::append(const U *begin, const U *end)
{
    basic_buffer<T>::size_type new_size = _size + end - begin;
    if (new_size > _capacity)
        grow(new_size);
    std::uninitialized_copy(begin, end,
                            _ptr + _size);
    _size = new_size;
}

template <typename T>
template <typename U>
void basic_buffer<T>::append(const U *begin, basic_buffer<T>::size_type len)
{
    basic_buffer<T>::size_type new_size = _size + len;
    if (new_size > _capacity)
        grow(new_size);
    std::uninitialized_copy(begin, begin + len,
                            _ptr + _size);
    _size = new_size;
}

typedef basic_buffer<char> buffer;
typedef basic_buffer<wchar_t> wbuffer;

template <typename Container>
class container_buffer : public basic_buffer<typename Container::value_type> {
private:
    Container &_container;

 protected:
    void grow(std::size_t capacity) WALLE_OVERRIDE 
    {
        _container.resize(capacity);
        this->set(&_container[0], capacity);
    }

public:
    explicit container_buffer(Container &c)
        : basic_buffer<typename Container::value_type>(&c[0], c.size(), c.size()),
            _container(c) {}
};

}
}

#endif //WALLE_WSL_INTERNAL_BASIC_BUFFER_H_