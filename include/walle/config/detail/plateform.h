#ifndef WALLE_CONFIG_DETAIL_PLATEFORM_H_
#define WALLE_CONFIG_DETAIL_PLATEFORM_H_

/** 
 * @note
 * 
 * WALLE_PLATEFORM int
 * WALLE_PLATEFORM_NAME string
 * WALLE_PROCESSOR string
 * WALLE_SYSTEM_LITTLE_ENDIAN int
 * WALLE_PTR_SIZE int
 * WALLE_WORD_SIZE int
 * WALLE_MIN_MALLOC_ALIGNMENT int
 * WALLE_CACHE_LINE_SIZE int
 */
#define WALLE_PLATEFORM_LINUX 1
#define WALLE_PLATEFORM_OSX   WALLE_PLATEFORM_LINUX+1

#if (defined(__linux) || defined(__linux__))
    #define WALLE_PLATEFORM WALLE_PLATEFORM_LINUX
    #define WALLE_PLATEFORM_NAME "linux"
    #if defined(__i386__) || defined(__intel__) || defined(_M_IX86)
        #define WALLE_SYSTEM_LITTLE_ENDIAN 1
        #define WALLE_PROCESSOR            "x86"
        #define WALLE_PROCESSOR_X86         1
    #elif defined(__x86_64__)
        #define WALLE_SYSTEM_LITTLE_ENDIAN 1
        #define WALLE_PROCESSOR            "x86_64"
        #define WALLE_PROCESSOR_X86_64     1
    #endif
#elif defined(__MACH__) || (defined(__MSL__) && (__dest_os == __mac_os_x))
    #define WALLE_PLATEFORM WALLE_PLATEFORM_OSX
    #define WALLE_SYSTEM_LITTLE_ENDIAN 1
    #define WALLE_PLATEFORM_NAME "osx"
    #if defined(__i386__) || defined(__intel__)
        #define WALLE_PROCESSOR            "x86"
        #define WALLE_PROCESSOR_X86         1
    #elif defined(__x86_64) || defined(__amd64)
         #define WALLE_PROCESSOR            "x86_64"
         #define WALLE_PROCESSOR_X86_64         1
    #endif
#else
    #error "unknown plateform"
#endif

#ifndef WALLE_PTR_SIZE
    #if defined(__WORDSIZE) // Defined by some variations of GCC.
		#define WALLE_PTR_SIZE ((__WORDSIZE) / 8)
	#elif defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__aarch64__) || defined(__mips64__) || defined(__64BIT__) || defined(__Ptr_Is_64)
		#define WALLE_PTR_SIZE 8
	#elif defined(__CC_ARM) && (__sizeof_ptr == 8)
		#define WALLE_PTR_SIZE 8
	#else
		#define WALLE_PTR_SIZE 4
	#endif
#endif

#ifndef WALLE_WORD_SIZE
    #if (WALLE_PTR_SIZE == 8)
	  #define WALLE_WORD_SIZE 8
    #else
	  #define WALLE_WORD_SIZE WALLE_PTR_SIZE
    #endif
#endif

#ifndef WALLE_MIN_MALLOC_ALIGNMENT
	#if WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
		#define WALLE_MIN_MALLOC_ALIGNMENT 16
	#else
		#define WALLE_MIN_MALLOC_ALIGNMENT (WALLE_PTR_SIZE * 2)
	#endif
#endif

#ifndef WALLE_CACHE_LINE_SIZE
	#if   defined(WALLE_PROCESSOR_X86)      
		#define WALLE_CACHE_LINE_SIZE 32    // This is the minimum possible value.
	#elif defined(WALLE_PROCESSOR_X86_64)  
		#define WALLE_CACHE_LINE_SIZE 64    // This is the minimum possible value
	#elif (WALLE_WORD_SIZE == 4)
		#define EA_CACHE_LINE_SIZE 32    // This is the minimum possible value
	#else
		#define EA_CACHE_LINE_SIZE 64    // This is the minimum possible value
	#endif
#endif

#endif //WALLE_CONFIG_DETAIL_PLATEFORM_H_