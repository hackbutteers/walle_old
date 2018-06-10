#ifndef WALLE_BUFFER_BUFFER_REF_H_
#define WALLE_BUFFER_BUFFER_REF_H_
#include <walle/config/base.h>
#include <cstddef>

namespace walle {

/** 
 * @brief  BufferRef do not hold the buffer,
 *         just a adopter to operater the buffer
 * @note   
 * @retval None
 */
template <typename T>
class BufferRef {
public:
    BufferRef():_data(nullptr), _size(0){}
    BufferRef(T *str, std::size_t s) : _data(str), _size(s)
    {}

    /** 
     * @brief  return the date pointer
     * @note   
     * @retval 
     */
    T* data() const
    {
        return _data;
    }

    /** 
     * @brief  return the buffer size.
     * @note   
     * @retval 
     */
    std::size_t size() const
    {
        return _size;
    }

    T operator[](size_t n) const 
    {
        WALLE_ASSERT(n < size(), "index overflow");
        return _data[n];
    }

    T& operator[](size_t n) 
    {
        WALLE_ASSERT(n < size(), "index overflow");
        return _data[n];
    }
    
    bool empty()
    {
        return _size == 0;
    }

    T& start()
    {
        return _data[0];
    }

    T& last()
    {
        return _data[_size - 1];
    }

    BufferRef<T> sub_buffer_ref(size_t from, size_t to)
    {
        return BufferRef<T>(_data + from, to - from);
    }
private:
    T            *_data;
    std::size_t   _size;
};

} //namespace walle
#endif //WALLE_BUFFER_BUFFER_REF_H_
