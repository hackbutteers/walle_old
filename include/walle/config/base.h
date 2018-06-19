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

#ifndef WALLE_ASSERT_MSG
    #define WALLE_ASSERT_MSG(e, m) assert((e)&&(m))
#endif

#ifndef WALLE_ASSERT
    #define WALLE_ASSERT(e) assert((e))
#endif

#ifndef WALLE_THROW
    #define WALLE_THROW(x) WALLE_ASSERT_MSG(false, "not  throw")
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

#ifndef WALLE_CONSTEXPR
    #define WALLE_CONSTEXPR constexpr
#endif

#ifndef WALLE_CPP14_CONSTEXPR
	#if WALLE_COMPILER_CPP14_ENABLED
		#define WALLE_CPP14_CONSTEXPR constexpr
	#else
		#define WALLE_CPP14_CONSTEXPR
	#endif
#endif

#ifndef WALLE_CHAR16_NATIVE
	#if defined(WALLE_COMPILER_CLANG)
		#define WALLE_CHAR16_NATIVE 1
		#define WALLE_CHAR32_NATIVE 1
	#elif defined(WALLE_COMPILER_GNUC)
		#define WALLE_CHAR16_NATIVE 1
		#define WALLE_CHAR32_NATIVE 1
	#else
		#define WALLE_CHAR16_NATIVE 0
		#define WALLE_CHAR32_NATIVE 0
	#endif
#endif

#if WALLE_CHAR16_NATIVE || WALLE_CHAR32_NATIVE
	#define WALLE_WCHAR_UNIQUE 1
#else
	#define WALLE_WCHAR_UNIQUE 0
#endif


#ifndef CHAR8_T_DEFINED
	#define CHAR8_T_DEFINED
	#if WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
		#define char8_t char  
	#else
		typedef char char8_t;
	#endif
	
	#if WALLE_CHAR16_NATIVE
		#if defined(__GNUC__) && !defined(__GXX_EXPERIMENTAL_CXX0X__) && defined(__CHAR16_TYPE__) // If using GCC and compiling in C...
			typedef __CHAR16_TYPE__ char16_t;
			typedef __CHAR32_TYPE__ char32_t;
		#endif
	#elif (WALLE_WCHAR_SIZE == 2)
			typedef wchar_t  char16_t;
			typedef uint32_t char32_t;
	#else
		typedef uint16_t char16_t;
		#if defined(__cplusplus)
			typedef wchar_t  char32_t;
		#else
			typedef uint32_t char32_t;
		#endif
	#endif
#endif

#endif