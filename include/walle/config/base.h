#ifndef WALLE_CONFIG_BASE_H_
#define WALLE_CONFIG_BASE_H_
#include <walle/config/detail/plateform.h>
#include <walle/config/detail/compiler.h>
#include <walle/config/detail/compiler_traits.h>
#include <cassert>
#include <cstddef>
/**  
 * @brief  
 * @note   
 * @retval 
 * WALLE_STATIC_ASSERT
 * WALLE_ARRAY_SIZE
 * WALLE_KILO_BYTE
 * WALLE_MEGA_BYTE
 * WALLE_GIGA_BYTE
 * WALLE_TERA_BYTE
 * WALLE_EXA_BYTE
 * WALLE_KIBI_BYTE
 * WALLE_MEBI_BYTE
 * WALLE_GIBI_BYTE
 * WALLE_TEBI_BYTE
 * WALLE_EXBI_BYTE
 * WALLE_ASSERT
 * WALLE_ABORT
 */

#ifndef WALLE_ATTRIBUTE_FORMAT_PRINTF
    #if defined(__GNUC__) || defined(__clang__)
        #define WALLE_ATTRIBUTE_FORMAT_PRINTF(X, Y) \
            __attribute__ ((format(printf, X, Y)))
    #else
        #define WALLE_ATTRIBUTE_FORMAT_PRINTF(X, Y)
    #endif
#endif

#ifndef WALLE_STATIC_ASSERT
    #define WALLE_STATIC_ASSERT(e,m) static_assert(e, m)
#endif

#ifndef WALLE_ARRAY_SIZE
    template <typename T, size_t N>
	char (&WALLEArraySizeHelper(T (&x)[N]))[N];

	template <typename T, size_t N>
	char (&WALLEArraySizeHelper(T (&&x)[N]))[N];

	#define WALLE_ARRAY_SIZE(x) (sizeof(WALLEArraySizeHelper(x)))
#endif

#ifndef WALLE_KILO_BYTE
    #define WALLE_KILO_BYTE(x) size_t(x) * 1000
    #define WALLE_MEGA_BYTE(x) size_t(x) * 1000 * 1000
    #define WALLE_GIGA_BYTE(x) size_t(x) * 1000 * 1000 * 1000
    #define WALLE_TERA_BYTE(x) size_t(x) * 1000 * 1000 * 1000 * 1000
    #define WALLE_EXA_BYTE(x) size_t(x) * 1000 * 1000 *1000 * 1000 * 1000

    #define WALLE_KIBI_BYTE(x) size_t(x) * 1024
    #define WALLE_MEBI_BYTE(x) size_t(x) * 1024 * 1024
    #define WALLE_GIBI_BYTE(x) size_t(x) * 1024 * 1024 * 1024
    #define WALLE_TEBI_BYTE(x) size_t(x) * 1024 * 1024 * 1024 * 1024
    #define WALLE_EXBI_BYTE(x) size_t(x) * 1024 * 1024 * 1024 * 1024 * 1024
#endif

#ifndef WALLE_ASSERT
    #define WALLE_ASSERT(e, m) assert((e)&&(m))
#endif

#ifndef WALLE_THROW
    #define WALLE_THROW(x) WALLE_ASSERT(false, "not  throw")
#endif

#ifndef WALLE_ABORT
    #define WALLE_ABORT() assert(false)
#endif

#ifndef WALLE_NULL
    #define WALLE_NULL nullptr
#endif
#ifndef WALLE_GNUC_LIBSTD_VERSION
    #ifdef __GNUC_LIBSTD__
        #define WALLE_GNUC_LIBSTD_VERSION (__GNUC_LIBSTD__ * 100 + __GNUC_LIBSTD_MINOR__)
    #else
        #define WALLE_GNUC_LIBSTD_VERSION 0
    #endif
#endif

#ifndef WALLE_NOEXCEPT
    #define WALLE_NOEXCEPT noexcept
#endif

#ifndef WALLE_OVERRIDE
    #define WALLE_OVERRIDE override
#endif

#endif