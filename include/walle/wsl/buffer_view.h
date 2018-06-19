#ifndef WALLE_WSL_BUFFER_VIEW_H_
#define WALLE_WSL_BUFFER_VIEW_H_

#include <walle/config/base.h>
#include <cstddef>

namespace wsl {

/** 
 * @brief  buffer_view do not hold the buffer,
 *         just a adopter to operater the buffer
 * @note   
 * @retval None
 */
template <typename T>
class buffer_view {
public:
    buffer_view():_data(nullptr), _size(0){}
    buffer_view(T *str, std::size_t s) : _data(str), _size(s)
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
        WALLE_ASSERT_MSG(n < size(), "index overflow");
        return _data[n];
    }

    T& operator[](size_t n) 
    {
        WALLE_ASSERT_MSG(n < size(), "index overflow");
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

    buffer_view<T> sub_buffer_ref(size_t from, size_t to)
    {
        return buffer_view<T>(_data + from, to - from);
    }
private:
    T            *_data;
    std::size_t   _size;
};

}
#endif //WALLE_WSL_BUFFER_VIEW_H_