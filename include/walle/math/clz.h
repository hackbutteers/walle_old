#ifndef WALLE_MATH_CLZ_H_
#define WALLE_MATH_CLZ_H_
#include <walle/config/base.h>

namespace walle {
namespace math {

template <typename Integral>
static WALLE_FORCE_INLINE unsigned clz_template(Integral x) 
{
    if (x == 0) 
        return 8 * sizeof(x);
    unsigned r = 0;
    while ((x & (static_cast<Integral>(1) << (8 * sizeof(x) - 1))) == 0)
        x <<= 1, ++r;
    return r;
}

template <typename Integral>
WALLE_FORCE_INLINE unsigned clz(Integral x);

#if defined(__GNUC__) || defined(__clang__)

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned>(unsigned i) 
{
    return static_cast<unsigned>(__builtin_clz(i));
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<int>(int i) 
{
    return clz(static_cast<unsigned>(i));
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned long>(unsigned long i) 
{
    return static_cast<unsigned>(__builtin_clzl(i));
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<long>(long i) 
{
    return clz(static_cast<unsigned long>(i));
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned long long>(unsigned long long i) 
{
    return static_cast<unsigned>(__builtin_clzll(i));
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<long long>(long long i) 
{
    return clz(static_cast<unsigned long long>(i));
}
#else

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<int>(int i) 
{
    return clz_template(i);
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned>(unsigned i) 
{
    return clz_template(i);
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<long>(long i) 
{
    return clz_template(i);
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned long>(unsigned long i) 
{
    return clz_template(i);
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<long long>(long long i) 
{
    return clz_template(i);
}

/** 
 * @brief clz (count leading zeros) 
 * @note   
 * @param  i: 
 * @retval 
 */
template <>
WALLE_FORCE_INLINE unsigned clz<unsigned long long>(unsigned long long i) 
{
    return clz_template(i);
}

#endif

} //namespace math
} //namespace walle
#endif