#ifndef WALLE_WSL_INTERNAL_CHAR_TRAITS_H_
#define WALLE_WSL_INTERNAL_CHAR_TRAITS_H_
#include <walle/config/base.h>
#include <walle/wsl/ascii.h>
#include <walle/wsl/type_traits.h>
#include <cstring>

namespace wsl {
namespace internal {
bool decode_part(const char8_t*&  pSrc, const char8_t*  pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);
bool decode_part(const char8_t*&  pSrc, const char8_t*  pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
bool decode_part(const char8_t*&  pSrc, const char8_t*  pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

bool decode_part(const char16_t*& pSrc, const char16_t* pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);
bool decode_part(const char16_t*& pSrc, const char16_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
bool decode_part(const char16_t*& pSrc, const char16_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

bool decode_part(const char32_t*& pSrc, const char32_t* pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);
bool decode_part(const char32_t*& pSrc, const char32_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
bool decode_part(const char32_t*& pSrc, const char32_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

bool decode_part(const int*&      pSrc, const int*      pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);
bool decode_part(const int*&      pSrc, const int*      pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
bool decode_part(const int*&      pSrc, const int*      pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

#if WALLE_WCHAR_UNIQUE
bool decode_part(const wchar_t*& pSrc, const wchar_t*   pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);
bool decode_part(const wchar_t*& pSrc, const wchar_t*   pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
bool decode_part(const wchar_t*& pSrc, const wchar_t*   pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

bool decode_part(const char8_t*&  pSrc, const char8_t*  pSrcEnd, wchar_t*&  pDest, wchar_t*  pDestEnd);
bool decode_part(const char16_t*& pSrc, const char16_t* pSrcEnd, wchar_t*&  pDest, wchar_t*  pDestEnd);
bool decode_part(const char32_t*& pSrc, const char32_t* pSrcEnd, wchar_t*&  pDest, wchar_t*  pDestEnd);
#endif


#if WALLE_WCHAR_UNIQUE
    inline bool decode_part(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
        #endif
    }

    inline bool decode_part(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
        #endif
    }

    inline bool decode_part(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
        #endif
    }

    inline bool decode_part(const char8_t*& pSrc, const char8_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
        #endif
    }

    inline bool decode_part(const char16_t*& pSrc, const char16_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
        #endif
    }

    inline bool decode_part(const char32_t*& pSrc, const char32_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
    {
        #if (WALLE_WCHAR_SIZE == 2)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
        #elif (WALLE_WCHAR_SIZE == 4)
            return decode_part(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
        #endif
    }
#endif

inline char8_t char_to_lower(char8_t c)
{ 
    return (char8_t)wsl::ascii::to_lower((uint8_t)c); 
}

inline char16_t char_to_lower(char16_t c)
{ 
	if((unsigned)c <= 0xff) 
		return (char16_t)wsl::ascii::to_lower((uint8_t)c); 
	return c; 
}

inline char32_t char_to_lower(char32_t c)
{ 
	if((unsigned)c <= 0xff) 
		return (char32_t)wsl::ascii::to_lower((uint8_t)c); 
	return c; 
}

#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
	inline wchar_t char_to_lower(wchar_t c)
	{ 
		if((unsigned)c <= 0xff) 
			return (wchar_t)wsl::ascii::to_lower((uint8_t)c); 
		return c; 
	}
#endif


inline char8_t char_to_upper(char8_t c)
{ 
	return (char8_t)wsl::ascii::to_upper((uint8_t)c); 
}

inline char16_t char_to_upper(char16_t c)
{ 
	if((unsigned)c <= 0xff) 
		return (char16_t)wsl::ascii::to_upper((uint8_t)c); 
	return c; 
}

inline char32_t char_to_upper(char32_t c)
{ 
	if((unsigned)c <= 0xff) 
		return (char32_t)wsl::ascii::to_upper((uint8_t)c); 
	return c; 
}

#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
inline wchar_t char_to_upper(wchar_t c)
{ 
	if((unsigned)c <= 0xff) 
		return (wchar_t)wsl::ascii::to_upper((uint8_t)c); 
	return c; 
}
#endif


template <typename T>
int compare(const T* p1, const T* p2, size_t n)
{
	for(; n > 0; ++p1, ++p2, --n) {
		if(*p1 != *p2)
			return (static_cast<typename std::make_unsigned<T>::type>(*p1) < 
					static_cast<typename std::make_unsigned<T>::type>(*p2)) ? -1 : 1;
	}
	return 0;
}

inline int compare(const char8_t* p1, const char8_t* p2, size_t n)
{
	return std::memcmp(p1, p2, n);
}

template <typename T>
inline int compare_i(const T* p1, const T* p2, size_t n)
{
	for(; n > 0; ++p1, ++p2, --n) {
		const T c1 = char_to_lower(*p1);
		const T c2 = char_to_lower(*p2);

		if(c1 != c2)
			return (static_cast<typename std::make_unsigned<T>::type>(c1) < 
					static_cast<typename std::make_unsigned<T>::type>(c2)) ? -1 : 1;
	}
	return 0;
}


inline const char8_t* find(const char8_t* p, char8_t c, size_t n)
{
	return (const char8_t*)std::memchr(p, c, n);
}

inline const char16_t* find(const char16_t* p, char16_t c, size_t n)
{
	for(; n > 0; --n, ++p) {
		if(*p == c)
			return p;
	}

	return NULL;
}

inline const char32_t* find(const char32_t* p, char32_t c, size_t n)
{
	for(; n > 0; --n, ++p) {
		if(*p == c)
			return p;
	}

	return NULL;
}

#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
	inline const wchar_t* find(const wchar_t* p, wchar_t c, size_t n)
	{
		for(; n > 0; --n, ++p) {
			if(*p == c)
				return p;
		}

		return NULL;
	}
#endif

	inline size_t char_strlen(const char8_t* p)
	{
		#if defined(_MSC_VER) || defined(__GNUC__) 
			return strlen(p);
		#else
			const char8_t* pCurrent = p;
			while(*pCurrent)
				++pCurrent;
			return (size_t)(pCurrent - p);
		#endif
	}

	inline size_t char_strlen(const char16_t* p)
	{
		const char16_t* pCurrent = p;
		while(*pCurrent)
			++pCurrent;
		return (size_t)(pCurrent - p);
	}

	inline size_t char_strlen(const char32_t* p)
	{
		const char32_t* pCurrent = p;
		while(*pCurrent)
			++pCurrent;
		return (size_t)(pCurrent - p);
	}

	#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
		inline size_t char_strlen(const wchar_t* p)
		{
			const wchar_t* pCurrent = p;
			while(*pCurrent)
				++pCurrent;
			return (size_t)(pCurrent - p);
		}
	#endif

	template <typename T>
	inline T* char_string_uninitialized_copy(const T* pSource, const T* pSourceEnd, T* pDestination)
	{
		std::memmove(pDestination, pSource, (size_t)(pSourceEnd - pSource) * sizeof(T));
		return pDestination + (pSourceEnd - pSource);
	}

	template <typename T>
	const T* char_type_string_find_end(const T* pBegin, const T* pEnd, T c)
	{
		const T* pTemp = pEnd;
		while(--pTemp >= pBegin)
		{
			if(*pTemp == c)
				return pTemp;
		}

		return pEnd;
	}
    
	template <typename T>
	const T* char_type_string_rsearch(const T* p1Begin, const T* p1End, 
								   const T* p2Begin, const T* p2End)
	{
		// Test for zero length strings, in which case we have a match or a failure, 
		// but the return value is the same either way.
		if((p1Begin == p1End) || (p2Begin == p2End))
			return p1Begin;

		// Test for a pattern of length 1.
		if((p2Begin + 1) == p2End)
			return char_type_string_find_end(p1Begin, p1End, *p2Begin);

		// Test for search string length being longer than string length.
		if((p2End - p2Begin) > (p1End - p1Begin))
			return p1End;

		// General case.
		const T* pSearchEnd = (p1End - (p2End - p2Begin) + 1);
		const T* pCurrent1;
		const T* pCurrent2;

		while(pSearchEnd != p1Begin)
		{
			// Search for the last occurrence of *p2Begin.
			pCurrent1 = char_type_string_find_end(p1Begin, pSearchEnd, *p2Begin);
			if(pCurrent1 == pSearchEnd) // If the first char of p2 wasn't found, 
				return p1End;           // then we immediately have failure.

			// In this case, *pTemp == *p2Begin. So compare the rest.
			pCurrent2 = p2Begin;
			while(*pCurrent1++ == *pCurrent2++)
			{
				if(pCurrent2 == p2End)
					return (pCurrent1 - (p2End - p2Begin));
			}

			// A smarter algorithm might know to subtract more than just one,
			// but in most cases it won't make much difference anyway.
			--pSearchEnd;
		}

		return p1End;
	}

	template <typename T>
	inline const T* char_type_string_find_first_of(const T* p1Begin, const T* p1End, const T* p2Begin, const T* p2End)
	{
		for (; p1Begin != p1End; ++p1Begin)
		{
			for (const T* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*p1Begin == *pTemp)
					return p1Begin;
			}
		}
		return p1End;
	}

	template <typename T>
	inline const T* char_type_string_rfind_first_not_of(const T* p1RBegin, const T* p1REnd, const T* p2Begin, const T* p2End)
	{
		for (; p1RBegin != p1REnd; --p1RBegin)
		{
			const T* pTemp;
			for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*(p1RBegin - 1) == *pTemp)
					break;
			}
			if (pTemp == p2End)
				return p1RBegin;
		}
		return p1REnd;
	}

	template <typename T>
	inline const T* char_type_string_find_first_not_of(const T* p1Begin, const T* p1End, const T* p2Begin, const T* p2End)
	{
		for (; p1Begin != p1End; ++p1Begin)
		{
			const T* pTemp;
			for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*p1Begin == *pTemp)
					break;
			}
			if (pTemp == p2End)
				return p1Begin;
		}
		return p1End;
	}

	template <typename T>
	inline const T* char_type_string_rfind_first_of(const T* p1RBegin, const T* p1REnd, const T* p2Begin, const T* p2End)
	{
		for (; p1RBegin != p1REnd; --p1RBegin)
		{
			for (const T* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*(p1RBegin - 1) == *pTemp)
					return p1RBegin;
			}
		}
		return p1REnd;
	}

	template <typename T>
	inline const T* char_type_string_rfind(const T* pRBegin, const T* pREnd, const T c)
	{
		while (pRBegin > pREnd)
		{
			if (*(pRBegin - 1) == c)
				return pRBegin;
			--pRBegin;
		}
		return pREnd;
	}


	inline char8_t* char_string_uninitialized_fill_n(char8_t* pDestination, size_t n, const char8_t c)
	{
		if(n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
			std::memset(pDestination, (uint8_t)c, (size_t)n);
		return pDestination + n;
	}

	inline char16_t* char_string_uninitialized_fill_n(char16_t* pDestination, size_t n, const char16_t c)
	{
		char16_t* pDest16          = pDestination;
		const char16_t* const pEnd = pDestination + n;
		while(pDest16 < pEnd)
			*pDest16++ = c;
		return pDestination + n;
	}

	inline char32_t* char_string_uninitialized_fill_n(char32_t* pDestination, size_t n, const char32_t c)
	{
		char32_t* pDest32          = pDestination;
		const char32_t* const pEnd = pDestination + n;
		while(pDest32 < pEnd)
			*pDest32++ = c;
		return pDestination + n;
	}

	#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
		inline wchar_t* char_string_uninitialized_fill_n(wchar_t* pDestination, size_t n, const wchar_t c)
		{
			wchar_t* pDest32          = pDestination;
			const wchar_t* const pEnd = pDestination + n;
			while(pDest32 < pEnd)
				*pDest32++ = c;
			return pDestination + n;
		}
	#endif

	inline char8_t* char_type_assign_n(char8_t* pDestination, size_t n, char8_t c)
	{
		if(n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
			return (char8_t*)std::memset(pDestination, c, (size_t)n);
		return pDestination;
	}

	inline char16_t* char_type_assign_n(char16_t* pDestination, size_t n, char16_t c)
	{
		char16_t* pDest16          = pDestination;
		const char16_t* const pEnd = pDestination + n;
		while(pDest16 < pEnd)
			*pDest16++ = c;
		return pDestination;
	}

	inline char32_t* char_type_assign_n(char32_t* pDestination, size_t n, char32_t c)
	{
		char32_t* pDest32          = pDestination;
		const char32_t* const pEnd = pDestination + n;
		while(pDest32 < pEnd)
			*pDest32++ = c;
		return pDestination;
	}

	#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
		inline wchar_t* char_type_assign_n(wchar_t* pDestination, size_t n, wchar_t c)
		{
			wchar_t* pDest32          = pDestination;
			const wchar_t* const pEnd = pDestination + n;
			while(pDest32 < pEnd)
				*pDest32++ = c;
			return pDestination;
		}
	#endif
}
} //namespace wsl
#endif //WALLE_WSL_INTERNAL_CHAR_TRAITS_H_
