#ifndef WALLE_CONFIG_DETAIL_COMPILER_TRAITS_H_
#define WALLE_CONFIG_DETAIL_COMPILER_TRAITS_H_
#include <walle/config/detail/plateform.h>
#include <walle/config/detail/compiler.h>
/** 
 * WALLE_USE_ALLOCATOR_TRAITS
 * WALLE_HAS_CPP_ATTRIBUTE
 * WALLE_USE_RVALUE_REFERENCES
 * WALLE_MAYBE_UNUSED
 * WALLE_NORETURN
 * WALLE_HAS_INCLUDE
 * WALLE_USE_VARIADIC_TEMPLATES
 * WALLE_EXTENSION
 * WALLE_PREPROCESSOR_JOIN(book_, 2)  book_2
 * WALLE_STRINGIF(x) "x"
 * WALLE_COMPILER_INTMAX_SIZE int
 * WALLE_OFFSETOF   size_t
 * WALLE_SIZEOF_MEMBER size_t 
 * WALLE_LIKELY
 * WALLE_UNLIKELY
 * WALLE_INIT_PROPERITY(x) for gcc init variable early
 * WALLE_MAY_ALIAS
 * WALLE_DISABLE_GCC_WARNING
 * WALLE_RESTORE_GCC_WARNING
 * WALLE_DISABLE_CLANG_WARNING
 * WALLE_RESTORE_CLANG_WARNING
 * WALLE_DISABLE_WARING
 * WALLE_RESTORE_WARING
 * WALLE_UNUSED
 * WALLE_EMPTY
 * WALLE_FORCE_INLINE
 * WALLE_NO_INLINE
 * WALLE_NON_COPYABLE
 * WALLE_OPTIMIZE_OFF
 * WALLE_OPTIMIZE_ON
 * WALLE_DISABLE_DEFAULT_CTOR
 * WALLE_DISABLE_COPY_CTOR
 * WALLE_DISABLE_MOVE_CTOR
 * WALLE_DISABLE_MOVE_OPERATOR
 * WALLE_DISABLE_ASSIGNMENT_OPERATOR
 * WALLE_NO_SANITIZE_ADDRESS
 * WALLE_NO_SANITIZE_THREAD
 * WALLE_NO_SANITIZE
 */


#if __cplusplus >= 201103L
    #define WALLE_USE_ALLOCATOR_TRAITS 1
#else
    #define WALLE_USE_ALLOCATOR_TRAITS 0
#endif

#ifdef __has_cpp_attribute
    #define WALLE_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define WALLE_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifndef WALLE_NORETURN
    #if WALLE_HAS_CPP_ATTRIBUTE(noreturn) && __cplusplus >= 201103L
        #define WALLE_NORETURN [[noreturn]]
    #else
        #define WALLE_NORETURN
    #endif
#endif

#if WALLE_HAS_CPP_ATTRIBUTE(maybe_unused)
# define WALLE_HAS_CXX17_ATTRIBUTE_MAYBE_UNUSED
#endif

#if WALLE_HAS_CPP_ATTRIBUTE(noreturn) && __cplusplus >= 201103L
    #define WALLE_NORETURN [[noreturn]]
#else
    #define WALLE_NORETURN
#endif

#ifdef WALLE_HAS_CXX17_ATTRIBUTE_MAYBE_UNUSED
# define WALLE_MAYBE_UNUSED [[maybe_unused]]
// g++/clang++ also support [[gnu::unused]]. However, we don't use it.
#elif defined(__GNUC__)
# define WALLE_MAYBE_UNUSED __attribute__((unused))
#else
# define WALLE_MAYBE_UNUSED
#endif

#ifndef WALLE_USE_RVALUE_REFERENCES
    #define WALLE_USE_RVALUE_REFERENCES
#endif
#ifndef WALLE_USE_VARIADIC_TEMPLATES
    #define WALLE_USE_VARIADIC_TEMPLATES 1
#endif

#if defined(__has_include)
# define WALLE_HAS_INCLUDE(x) __has_include(x)
#else
# define WALLE_HAS_INCLUDE(x) 0
#endif

#ifndef WALLE_EXTENSION
    #if defined(WALLE_COMPILER_GNUC)
        #define WALLE_EXTENSION __extension__
    #else
        #define WALLE_EXTENSION
    #endif
#endif

#ifndef WALLE_PREPROCESSOR_JOIN
    #define WALLE_PREPROCESSOR_JOIN(a, b) WALLE_PREPROCESSOR_JOIN1(a, b)
    #define WALLE_PREPROCESSOR_JOIN1(a, b) WALLE_PREPROCESSOR_JOIN2(a, b)
    #define WALLE_PREPROCESSOR_JOIN2(a, b) a##b
#endif

#ifndef WALLE_STRINGIF
    #define WALLE_STRINGIF(x) WALLE_STRINGIF_IMPL(x)
    #define WALLE_STRINGIF_IMPL(x) #x
#endif

#ifndef WALLE_COMPILER_INTMAX_SIZE
    #if defined(WALLE_COMPILER_GNUC) && defined(WALLE_PROCESSOR_X86_64)
        #define WALLE_COMPILER_INTMAX_SIZE 16
    #else 
        #define WALLE_COMPILER_INTMAX_SIZE 8
    #endif
#endif

#ifndef WALLE_OFFSETOF
    #if defined(WALLE_COMPILER_GNUC)
        /**  
         * @note   // We can't use GCC 4's __builtin_offsetof because 
         * it mistakenly complains about non-PODs that are really PODs.
         */
        #define WALLE_OFFSETOF(struct_, member_) ((size_t)(((uintptr_t)&reinterpret_cast<const volatile char&>((((struct_*)65536)->member_))) - 65536))
    #else
        #define WALLE_OFFSETOF(struct_, member_) offsetof(struct_, member_)
    #endif
#endif

#ifndef WALLE_COMPILER_NO_EXTENDED_SIZEOF
    #define WALLE_SIZEOF_MEMBER(struct_, member_) (sizeof(struct_::member_))
#else
    #define WALLE_SIZEOF_MEMBER(struct_, member_) (sizeof(((struct_*)0)->member_))
#endif

#ifndef WALLE_LIKELY
	#if defined(WALLE_COMPILER_GNUC) 
		#if defined(__cplusplus)
		    #define WALLE_LIKELY(x)   __builtin_expect(!!(x), true)
		    #define WALLE_UNLIKELY(x) __builtin_expect(!!(x), false) 
		#else
		    #define WALLE_LIKELY(x)   __builtin_expect(!!(x), 1)
		    #define WALLE_UNLIKELY(x) __builtin_expect(!!(x), 0) 
		#endif
	#else
	    #define WALLE_LIKELY(x)   (x)
	    #define WALLE_UNLIKELY(x) (x)
	#endif
#endif

#ifndef WALLE_INIT_PROPERITY
    #if defined(WALLE_COMPILER_GNUC)
        #define WALLE_INIT_PROPERITY(x) __attribute__ ((init_priority (x)))
    #else
        #define WALLE_INIT_PROPERITY(x)
    #endif
#endif

#ifndef WALLE_MAY_ALIAS
    #if defined(WALLE_COMPILER_GNUC)
        #define WALLE_MAY_ALIAS __attribute__((__may_alias__))
    #else
        #define  WALLE_MAY_ALIAS
    #endif
#endif

#ifndef WALLE_DISABLE_GCC_WARNING
    #if defined(WALLE_COMPILER_GNUC)
        #define WALLEGCCWHELP0(x) #x
        #define WALLEGCCWHELP1(x) WALLEGCCWHELP0(GCC diagnostic ignored x)
        #define WALLEGCCWHELP2(x) WALLEGCCWHELP1(#x)
    #endif

    #if defined(WALLE_COMPILER_GNUC) && (WALLE_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
        #define WALLE_DISABLE_GCC_WARNING(w)   \
            _Pragma("GCC diagnostic push")  \
            _Pragma(WALLEGCCWHELP2(w))
    #elif defined(WALLE_COMPILER_GNUC) && (WALLE_COMPILER_VERSION >= 4004)
        #define WALLE_DISABLE_GCC_WARNING(w)   \
            _Pragma(EAGCCWHELP2(w))
    #else
        #define WALLE_DISABLE_GCC_WARNING(w)
    #endif
#endif

#ifndef WALLE_RESTORE_GCC_WARNING
    #if defined(WALLE_COMPILER_GNUC) && (WALLE_COMPILER_VERSION >= 4006)
        #define WALLE_RESTORE_GCC_WARNING()    \
		    _Pragma("GCC diagnostic pop")
	#else
	    #define WALLE_RESTORE_GCC_WARNING()
	#endif
#endif

#ifndef WALLE_DISABLE_CLANG_WARNING
    #if defined(WALLE_COMPILER_CLANG)
        #define WALLECLANGWHELP0(x) #x
        #define WALLECLANGWHELP1(x) WALLECLANGWHELP0(clang diagnostic ignored x)
        #define WALLECLANGWHELP2(x) WALLECLANGWHELP1(#x)
        #define WALLE_DISABLE_CLANG_WARNING(w)   \
            _Pragma("clang diagnostic push")  \
            _Pragma(WALLECLANGWHELP2(w))
	#else
        #define WALLE_DISABLE_CLANG_WARNING(w)
    #endif
#endif

#ifndef WALLE_RESTORE_CLANG_WARNING
    #if defined(WALLE_COMPILER_CLANG)
		#define WALLE_RESTORE_CLANG_WARNING()    \
			_Pragma("clang diagnostic pop")
	#else
		#define WALLE_RESTORE_CLANG_WARNING()
	#endif
#endif

#ifndef WALLE_DISABLE_WARING
    #if WALLE_COMPILER_CLANG
        #define WALLE_DISABLE_WARING(w) WALLE_DISABLE_CLANG_WARNING(w)
    #elif WALLE_COMPILER_GNUC
        #define WALLE_DISABLE_WARING(w) WALLE_DISABLE_GCC_WARNING(w)
    #endif
#endif

#ifndef WALLE_RESTORE_WARNING
    #if WALLE_COMPILER_CLANG
        #define WALLE_RESTORE_WARNING() WALLE_RESTORE_CLANG_WARNING()
    #elif WALLE_COMPILER_GNUC
        #define WALLE_RESTORE_WARNING() WALLE_RESTORE_GCC_WARNING()
    #endif
#endif
#ifndef WALLE_UNUSED
    #define WALLE_UNUSED(x) (void)x
#endif

#ifndef WALLE_EMPTY
    #define WALLE_EMPTY (void)0
#endif

#ifndef WALLE_FORCE_INLINE
    #if defined(_cplusplus)
        #define WALLE_FORCE_INLINE inline __attribute__((always_inline))
    #else
        #define WALLE_FORCE_INLINE __inline__ __attribute__((always_inline))
    #endif 
#endif

#ifndef WALLE_NO_INLINE 
    #define WALLE_NO_INLINE __attribute__((noinline))
#endif

#if !defined(WALLE_NON_COPYABLE)
    #if defined(WALLE_COMPILER_NO_DELETED_FUNCTIONS)
	    #define WALLE_NON_COPYABLE(Class_)               \
		private:                                      \
			Class_(const Class_&);                  \
			void operator=(const Class_&)
	#else
		#define WALLE_NON_COPYABLE(Class_)               \
			Class_(const Class_&) = delete;         \
			void operator=(const Class_&) = delete
     #endif
#endif

#if !defined(WALLE_OPTIMIZE_OFF)
	#if defined(WALLE_COMPILER_GNUC) && (EA_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__)) // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
	    #define WALLE_OPTIMIZE_OFF()            \
		    _Pragma("GCC push_options")      \
			_Pragma("GCC optimize 0")
    #elif defined(WALLE_COMPILER_CLANG)
        #define WALLE_OPTIMIZE_OFF() \
				WALLE_DISABLE_CLANG_WARNING(-Wunknown-pragmas) \
				_Pragma("clang optimize off") \
				WALLE_RESTORE_CLANG_WARNING()
	#else
		#define WALLE_OPTIMIZE_OFF()
	#endif
#endif

#if !defined(WALLE_OPTIMIZE_ON)
    #if defined(WALLE_COMPILER_GNUC) && (WALLE_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__)) // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
			#define EA_OPTIMIZE_ON() _Pragma("GCC pop_options")
    #elif defined(WALLE_COMPILER_CLANG)
        #define WALLE_OPTIMIZE_ON() \
		    WALLE_DISABLE_CLANG_WARNING(-Wunknown-pragmas) \
			_Pragma("clang optimize on") \
			WALLE_RESTORE_CLANG_WARNING()
	#else
		#define WALLE_OPTIMIZE_ON()
	#endif
#endif


#if defined(WALLE_COMPILER_NO_DELETED_FUNCTIONS)
    #define WALLE_FUNCTION_DELETE
#else
    #define WALLE_FUNCTION_DELETE = delete
#endif

#ifndef WALLE_DISABLE_DEFAULT_CTOR
    #define WALLE_DISABLE_DEFAULT_CTOR(ClassName) ClassName() WALLE_FUNCTION_DELETE
#endif

#ifndef WALLE_DISABLE_COPY_CTOR
    #define WALLE_DISABLE_COPY_CTOR(ClassName) ClassName(const ClassName &) WALLE_FUNCTION_DELETE
#endif

#ifndef WALLE_DISABLE_MOVE_CTOR
    #define WALLE_DISABLE_MOVE_CTOR(ClassName) ClassName(ClassName&&) WALLE_FUNCTION_DELETE
#endif

#ifndef WALLE_DISABLE_ASSIGNMENT_OPERATOR
    #define WALLE_DISABLE_ASSIGNMENT_OPERATOR(ClassName) ClassName & operator=(const ClassName &) WALLE_FUNCTION_DELETE
#endif

#ifndef WALLE_DISABLE_MOVE_OPERATOR
    #define WALLE_DISABLE_MOVE_OPERATOR(ClassName) ClassName & operator=(ClassName&&) WALLE_FUNCTION_DELETE
#endif

#ifndef WALLE_OVERRIDE
    #define FMT_OVERRIDE override
#endif


#ifndef WALLE_NO_SANITIZE_ADDRESS
    #define WALLE_NO_SANITIZE_ADDRESS __attribute__ ((no_sanitize_address))
#endif

#ifndef WALLE_NO_SANITIZE_THREAD
    #if defined(__GNUC__) && __GNUC__ >= 5
        #define WALLE_NO_SANITIZE_THREAD __attribute__ ((no_sanitize_thread))
    #else
        #define WALLE_NO_SANITIZE_THREAD
    #endif
#endif

#ifndef WALLE_NO_SANITIZE
    #define WALLE_NO_SANITIZE WALLE_NO_SANITIZE_ADDRESS WALLE_NO_SANITIZE_THREAD
#endif
#endif //WALLE_CONFIG_DETAIL_COMPILER_TRAITS_H_