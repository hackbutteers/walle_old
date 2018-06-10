#ifndef WALLE_CONFIG_MATH_H_ 
#define WALLE_CONFIG_MATH_H_
#include <walle/config/base.h>
#include <walle/math/abs.h>
#include <walle/math/clz.h>
#include <walle/math/log2.h>
#include <walle/math/power2.h>
#include <walle/math/ffs.h>
#include <cstdint>

#if WALLE_COMPILER_GNUC || WALLE_COMPILER_HAS_BUILDIN(__builtin_clz)
    #define WALLE_BUILDIN_CLZ(n) __builtin_clz(n)
    #define WALLE_BUILDIN_CLZLL(n) __builtin_clzll(n)
#endif

/** 
 * @brief  
 * @note The following macro works on both 32 and 64-bit platforms.
 * Usage: instead of writing 0x1234567890123456
 * write UINT64_2PART_C(0x12345678,90123456);
 * @retval 
 */
#ifndef WALLE_UINT64_2PART_C
    #define WALLE_UINT64_2PART_C(a, b) (((static_cast<uint64_t>(a) << 32) + 0x##b##u))
#endif
#endif //WALLE_BASE_MATH_H_