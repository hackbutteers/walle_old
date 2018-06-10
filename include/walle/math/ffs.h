#ifndef WALLE_MATH_FFS_H_
#define WALLE_MATH_FFS_H_
#include <walle/config/base.h>
namespace walle {
namespace math {

template <typename Integral>
static WALLE_FORCE_INLINE unsigned ffs_template(Integral x) 
{
    if (x == 0) 
        return 0u;
    unsigned r = 1;
    while ((x & 1) == 0)
        x >>= 1, ++r;
    return r;
}   

#if defined(__GNUC__) || defined(__clang__)

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(int i) 
{
    return static_cast<unsigned>(__builtin_ffs(i));
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned i) 
{
    return ffs(static_cast<int>(i));
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(long i) 
{
    return static_cast<unsigned>(__builtin_ffsl(i));
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned long i) 
{
    return ffs(static_cast<long>(i));
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(long long i) 
{
    return static_cast<unsigned>(__builtin_ffsll(i));
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned long long i) 
{
    return ffs(static_cast<long long>(i));
}

#else

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(int i) 
{
    return ffs_template(i);
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned int i) 
{
    return ffs_template(i);
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(long i) 
{
    return ffs_template(i);
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned long i) {
    return ffs_template(i);
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(long long i) {
    return ffs_template(i);
}

/** 
 * @brief  find first set bit in integer, or zero if none are set.
 * @note   
 * @param  i: 
 * @retval 
 */
static WALLE_FORCE_INLINE
unsigned ffs(unsigned long long i) 
{
    return ffs_template(i);
}
#endif
    
} //namespace math
} //namespace walle
#endif