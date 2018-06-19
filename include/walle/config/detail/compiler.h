#ifndef WALLE_CONFIG_DETAIL_COMPILER_H_
#define WALLE_CONFIG_DETAIL_COMPILER_H_
/**
 * @brief 
 * WALLE_COMPILER_ID
 * WALLE_COMPILER_NAME string
 * WALLE_COMPILER_GNUC bool
 * WALLE_COMPILER_CLANG bool
 * WALLE_COMPILER_VERSION int
 * WALLE_COMPILER_HAS_FEATURE(x)
 * WALLE_COMPILER_HAS_BUILDIN(x)
 * WALLE_COMPILER_CPP11_ENABLE bool
 * WALLE_COMPILER_CPP14_ENABLED  bool
 * WALLE_COMPILER_CPP17_ENABLED  bool
 * WALLE_COMPILER_NO_ALIGNED_NEW bool
 * 
 * 
 */
#include <walle/config/detail/plateform.h>

#ifndef WALLE_COMPILER_HAS_FEATURE
    #if defined(__clang__)
	    #define WALLE_COMPILER_HAS_FEATURE(x) __has_feature(x)
	#else
	    #define WALLE_COMPILER_HAS_FEATURE(x) 0
	#endif
#endif

#ifndef WALLE_COMPILER_HAS_BUILDIN
    #if defined(__clang__)
        #define WALLE_COMPILER_HAS_BUILDIN(x) 0
    #else
        #define WALLE_COMPILER_HAS_BUILDIN(x) 0
    #endif
#endif

#if !defined(WALLE_COMPILER_CPP11_ENABLED) && defined(_cplusplus)
    #if (_cplusplus >= 201103L)
        #define WALLE_COMPILER_CPP11_ENABLED 1
    #elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
        #define WALLE_COMPILER_CPP11_ENABLED 1
    #endif
#endif

#if !defined(WALLE_COMPILER_CPP14_ENABLED) && defined(_cplusplus)
    #if (_cplusplus >= 201402)
        #define WALLE_COMPILER_CPP14_ENABLED 1
    #endif
#endif

#if !defined(WALLE_COMPILER_CPP17_ENABLED) && defined(_cplusplus)
    #if (_cplusplus >= 201702)
        #define WALLE_COMPILER_CPP17_ENABLED 1
    #endif
#endif

#if defined(__clang__)
    #define WALLE_COMPILER_ID 1
    #define WALLE_COMPILER_NAME "clang"
    #define WALLE_COMPILER_CLANG  1
    #define WALLE_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__)
    #define WALLE_COMPILER_ID 2
    #define WALLE_COMPILER_NAME "gnuc"
    #define WALLE_COMPILER_GNUC 1
    #define WALLE_COMPILER_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)  
#else
    #error "not known"
#endif

#if !defined(WALLE_COMPILER_NO_ALIGNED_NEW) && !defined(WALLE_COMPILER_CPP17_ENABLEDD)
    #define WALLE_COMPILER_NO_ALIGNED_NEW 1
#endif


#endif //WALLE_CONFIG_DETAIL_COMPILER_H_