#ifndef WALLE_COMMON_BASIC_BUFFER_H_
#define WALLE_COMMON_BASIC_BUFFER_H_
#include <walle/config/base.h>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <iostream>

namespace walle {

template <typename T>
class BasicBuffer {
public:
    virtual ~BasicBuffer()
    {

    }

    /**  
     * @brief returns the size of this buffer. 
     * @note   
     * @retval 
     */
    std::size_t size() const
    {
        return _size;
    }

    /** Returns the capacity of this buffer.
     * @brief  
     * @note   
     * @retval 
     */
    std::size_t capacity() const
    {
        return _capacity;
    }


    void resize(std::size_t new_size)
    {
        if (new_size > _capacity)
            grow(new_size);
        _size = new_size;
    }


    void reserve(std::size_t capacitySize)
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

    size_t avaible()
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
    void append(const U *begin, size_t len);

    T &operator[](std::size_t index)
    {
        WALLE_ASSERT(index < _size, "index overflow the buffer memory");
        return _ptr[index];
    }
    const T &operator[](std::size_t index) const
    {
        WALLE_ASSERT(index < _size, "index overflow the buffer memory");
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
    std::size_t    _size;
    std::size_t    _capacity;

    BasicBuffer(T *ptr = 0, std::size_t capacitySize = 0)
        : _ptr(ptr), 
          _size(0), 
          _capacity(capacitySize)
    {

    }

    virtual void grow(std::size_t size) = 0;
};

template <typename T>
template <typename U>
void BasicBuffer<T>::append(const U *begin, const U *end)
{
    std::size_t new_size = _size + end - begin;
    if (new_size > _capacity)
        grow(new_size);
    std::uninitialized_copy(begin, end,
                            _ptr + _size);
    _size = new_size;
}

template <typename T>
template <typename U>
void BasicBuffer<T>::append(const U *begin, size_t len)
{
    std::size_t new_size = _size + len;
    if (new_size > _capacity)
        grow(new_size);
    std::uninitialized_copy(begin, begin + len,
                            _ptr + _size);
    _size = new_size;
}

} //namespace walle
#endif //WALLE_COMMON_BASIC_BUFFER_H_
