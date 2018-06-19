#ifndef WALLE_WSL_STRING_VIEW_H_
#define WALLE_WSL_STRING_VIEW_H_
#include <walle/config/base.h>
#include <walle/wsl/internal/char_traits.h>
#include <walle/wsl/algorithm.h>
#include <iterator>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iosfwd>
#include <limits>
#include <string>


namespace wsl {

template <typename T>
class basic_string_view {
public:
    typedef basic_string_view<T>						this_type;
    typedef T 											value_type;
    typedef T* 											pointer;
    typedef const T* 									const_pointer;
    typedef T& 											reference;
    typedef const T& 									const_reference;
    typedef T* 											iterator;
    typedef const T* 									const_iterator;
    typedef std::reverse_iterator<iterator> 			reverse_iterator;
    typedef std::reverse_iterator<const_iterator> 	const_reverse_iterator;
    typedef size_t 										size_type;
    typedef ptrdiff_t 									difference_type;

    static const WALLE_CONSTEXPR size_type npos = size_type(-1);

protected:
    const_pointer _begin = nullptr;
    size_type     _count = 0;
public:
    WALLE_CPP14_CONSTEXPR basic_string_view() WALLE_NOEXCEPT : _begin(nullptr), _count(0) 
    {

    }

    WALLE_CPP14_CONSTEXPR basic_string_view(const basic_string_view& other) WALLE_NOEXCEPT = default;
    WALLE_CPP14_CONSTEXPR basic_string_view(const T* s, size_type count) : _begin(s), _count(count) 
    {

    }
    WALLE_CPP14_CONSTEXPR basic_string_view(const T* s) : _begin(s), _count(wsl::internal::char_strlen(s)) 
    {

    }

    WALLE_CPP14_CONSTEXPR basic_string_view(std::basic_string<T> &str) : _begin(str.data()), _count(str.size()) {} 
    basic_string_view& operator=(const basic_string_view& view) = default;

    WALLE_CPP14_CONSTEXPR const_iterator begin() const WALLE_NOEXCEPT { return _begin; }
    WALLE_CPP14_CONSTEXPR const_iterator cbegin() const WALLE_NOEXCEPT { return _begin; }
    WALLE_CPP14_CONSTEXPR const_iterator end() const WALLE_NOEXCEPT { return _begin + _count; }
    WALLE_CPP14_CONSTEXPR const_iterator cend() const WALLE_NOEXCEPT { return _begin + _count; }
    WALLE_CPP14_CONSTEXPR const_reverse_iterator rbegin() const WALLE_NOEXCEPT { return const_reverse_iterator(_begin + _count); }
    WALLE_CPP14_CONSTEXPR const_reverse_iterator crbegin() const WALLE_NOEXCEPT { return const_reverse_iterator(_begin + _count); }
    WALLE_CPP14_CONSTEXPR const_reverse_iterator rend() const WALLE_NOEXCEPT { return const_reverse_iterator(_begin); }
    WALLE_CPP14_CONSTEXPR const_reverse_iterator crend() const WALLE_NOEXCEPT { return const_reverse_iterator(_begin); }


    WALLE_CPP14_CONSTEXPR const_pointer data() const 
    { 
        return _begin; 
    }
  
    WALLE_CPP14_CONSTEXPR const_reference front() const
    {
        WALLE_ASSERT_MSG(!empty(), "empty");
        return _begin[0];
    }

    WALLE_CPP14_CONSTEXPR const_reference back() const
    {
        WALLE_ASSERT_MSG(!empty(), "empty");
        return _begin[_count - 1];
    }

    WALLE_CPP14_CONSTEXPR const_reference operator[](size_type pos) const
    {
        if(WALLE_UNLIKELY(pos >= _count))
            WALLE_ASSERT_MSG(false, "out of range");
        return _begin[pos];
    }

    WALLE_CPP14_CONSTEXPR const_reference at(size_type pos) const
    {
        if(WALLE_UNLIKELY(pos >= _count))
            WALLE_ASSERT_MSG(false, "out of range");
        return _begin[pos];
    }

    WALLE_CPP14_CONSTEXPR size_type size() const WALLE_NOEXCEPT 
    { 
        return _count; 
    }

	WALLE_CPP14_CONSTEXPR size_type length() const WALLE_NOEXCEPT 
    { 
        return _count; 
    }

    WALLE_CPP14_CONSTEXPR size_type max_size() const WALLE_NOEXCEPT 
    { 
        return (std::numeric_limits<size_type>::max)(); 
    }
	
    WALLE_CPP14_CONSTEXPR bool empty() const WALLE_NOEXCEPT 
    { 
        return _count == 0; 
    }

    WALLE_CPP14_CONSTEXPR void swap(basic_string_view& v)
    {
        std::swap(_begin, v._begin);
        std::swap(_count, v._count);
    }

    WALLE_CPP14_CONSTEXPR void remove_prefix(size_type n)
    {
        WALLE_ASSERT_MSG(n <= _count, "behavior is undefined if moving past the end of the string");
        _begin += n;
        _count -= n;
    }

    WALLE_CPP14_CONSTEXPR void remove_suffix(size_type n)
    {
        WALLE_ASSERT_MSG(n <= _count, "behavior is undefined if moving past the beginning of the string");
        _count -= n;
    }

    size_type copy(T* pDestination, size_type count, size_type pos = 0) const
    {
        if(WALLE_UNLIKELY(pos > _count))
            WALLE_ASSERT_MSG(false, "string_view::copy -- out of range");

        count = std::min(count, _count - pos);
        auto* pResult = char_string_uninitialized_copy(_begin + pos, _begin + pos + count, pDestination);
        *pResult = 0; // write null-terminator
        return pResult - pDestination;
    }

    WALLE_CPP14_CONSTEXPR basic_string_view substr(size_type pos = 0, size_type count = npos) const
    {
            if(WALLE_UNLIKELY(pos > _count))
                WALLE_ASSERT_MSG(false, "string_view::substr -- out of range");

        count = std::min(count, _count - pos);
        return this_type(_begin + pos, count);
    }

    WALLE_CPP14_CONSTEXPR int compare(basic_string_view sw) const WALLE_NOEXCEPT
    {
        return wsl::internal::compare(_begin, sw.data(), std::min(size(), sw.size()));
    }

    WALLE_CPP14_CONSTEXPR int compare(size_type pos1, size_type count1, basic_string_view sw) const
    {
        return substr(pos1, count1).compare(sw);
    }

    WALLE_CPP14_CONSTEXPR int compare(size_type pos1,
                                size_type count1,
                                basic_string_view sw,
                                size_type pos2,
                                size_type count2) const
    {
        return substr(pos1, count1).compare(sw.substr(pos2, count2));
    }

    WALLE_CPP14_CONSTEXPR int compare(const T* s) const 
    { 
        return compare(basic_string_view(s)); 
    }

	WALLE_CPP14_CONSTEXPR int compare(size_type pos1, size_type count1, const T* s) const
    {
        return substr(pos1, count1).compare(basic_string_view(s));
    }

	WALLE_CPP14_CONSTEXPR int compare(size_type pos1, size_type count1, const T* s, size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    WALLE_CPP14_CONSTEXPR size_type find(basic_string_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        auto* pEnd = _begin + _count;
        if (WALLE_LIKELY(((npos - sw.size()) >= pos) && (pos + sw.size()) <= _count)) {
            const value_type* const pTemp = std::search(_begin + pos, pEnd, sw.data(), sw.data() + sw.size());

            if ((pTemp != pEnd) || (sw.size() == 0))
                return (size_type)(pTemp - _begin);
        }
        return npos;
    }

	WALLE_CPP14_CONSTEXPR size_type find(T c, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find(basic_string_view(&c, 1), pos);
    }

    WALLE_CPP14_CONSTEXPR size_type find(const T* s, size_type pos, size_type count) const
    {
        return find(basic_string_view(s, count), pos);
    }

	WALLE_CPP14_CONSTEXPR size_type find(const T* s, size_type pos = 0) const 
    { 
        return find(basic_string_view(s), pos); 
    }

	WALLE_CPP14_CONSTEXPR size_type rfind(basic_string_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {
        return rfind(sw._begin, pos, sw._count);
    }

    WALLE_CPP14_CONSTEXPR size_type rfind(T c, size_type pos = npos) const WALLE_NOEXCEPT
    {
        if (WALLE_LIKELY(_count))
        {
            const value_type* const pEnd = _begin + std::min(_count - 1, pos) + 1;
            const value_type* const pResult = CharTypeStringRFind(pEnd, _begin, c);

            if (pResult != _begin)
                return (size_type)((pResult - 1) - _begin);
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type rfind(const T* s, size_type pos, size_type n) const
    {
        if (WALLE_LIKELY(n <= _count)) {
            if (WALLE_LIKELY(n)) {
                const const_iterator pEnd = _begin + std::min(_count - n, pos) + n;
                const const_iterator pResult = char_type_string_rsearch(_begin, pEnd, s, s + n);

                if (pResult != pEnd)
                    return (size_type)(pResult - _begin);
            } else {
                return std::min(_count, pos);
            }
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type rfind(const T* s, size_type pos = npos) const
    {
        return rfind(s, pos, (size_type)char_strlen(s));
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_of(basic_string_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find_first_of(sw._begin, pos, _count);
    }

	WALLE_CPP14_CONSTEXPR size_type find_first_of(T c, size_type pos = 0) const WALLE_NOEXCEPT 
    { 
        return find(c, pos); 
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_of(const T* s, size_type pos, size_type n) const
    {
        // If position is >= size, we return npos.
        if (WALLE_LIKELY((pos < _count))) {
            const value_type* const pBegin = _begin + pos;
            const value_type* const pEnd = _begin + _count;
            const const_iterator pResult = char_type_string_find_first_of(pBegin, pEnd, s, s + n);

            if (pResult != pEnd) {
                return (size_type)(pResult - _begin);
            }
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_of(const T* s, size_type pos = 0) const
    {
        return find_first_of(s, pos, (size_type)char_strlen(s));
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_of(basic_string_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {
        return find_last_of(sw._begin, pos, sw._count);
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_of(T c, size_type pos = npos) const WALLE_NOEXCEPT 
    { 
        return rfind(c, pos); 
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_of(const T* s, size_type pos, size_type n) const
    {
        // If n is zero or position is >= size, we return npos.
        if (WALLE_LIKELY(_count)) {
            const value_type* const pEnd = _begin + std::min(_count - 1, pos) + 1;
            const value_type* const pResult = CharTypeStringRFindFirstOf(pEnd, _begin, s, s + n);

            if (pResult != _begin)
                return (size_type)((pResult - 1) - _begin);
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_of(const T* s, size_type pos = npos) const
    {
        return find_last_of(s, pos, (size_type)char_strlen(s));
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_not_of(basic_string_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find_first_not_of(sw._begin, pos, sw._count);
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_not_of(T c, size_type pos = 0) const WALLE_NOEXCEPT
    {
        if (WALLE_LIKELY(pos <= _count)) {
            const auto pEnd = _begin + _count;
            // Todo: Possibly make a specialized version of CharTypeStringFindFirstNotOf(pBegin, pEnd, c).
            const const_iterator pResult = char_type_string_find_first_not_of(_begin + pos, pEnd, &c, &c + 1);

            if (pResult != pEnd) {
                return (size_type)(pResult - _begin);
            }
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos, size_type n) const
    {
        if (WALLE_LIKELY(pos <= _count)) {
            const auto pEnd = _begin + _count;
            const const_iterator pResult = char_type_string_findFirst_not_of(_begin + pos, pEnd, s, s + n);

            if (pResult != pEnd)
                return (size_type)(pResult - _begin);
        }
        return npos;
    }

	WALLE_CPP14_CONSTEXPR size_type find_first_not_of(const T* s, size_type pos = 0) const
    {
        return find_first_not_of(s, pos, (size_type)CharStrlen(s));
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_not_of(basic_string_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {
        return find_last_not_of(sw._begin, pos, sw._count);
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_not_of(T c, size_type pos = npos) const WALLE_NOEXCEPT
    {
        if (WALLE_LIKELY(_count)) {
            // Todo: Possibly make a specialized version of CharTypeStringRFindFirstNotOf(pBegin, pEnd, c).
            const value_type* const pEnd = _begin + std::min(_count - 1, pos) + 1;
            const value_type* const pResult = char_type_string_rfind_first_not_of(pEnd, _begin, &c, &c + 1);

            if (pResult != _begin)
                return (size_type)((pResult - 1) - _begin);
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos, size_type n) const
    {
        if (WALLE_LIKELY(_count))
        {
            const value_type* const pEnd = _begin + std::min(_count - 1, pos) + 1;
            const value_type* const pResult = char_type_string_rfind_first_not_of(pEnd, _begin, s, s + n);

            if (pResult != _begin)
                return (size_type)((pResult - 1) - _begin);
        }
        return npos;
    }

    WALLE_CPP14_CONSTEXPR size_type find_last_not_of(const T* s, size_type pos = npos) const
    {
        return find_last_not_of(s, pos, (size_type)char_strlen(s));
    }
};

template <typename T>
class basic_cstring_view : public basic_string_view<T> {
public:
    typedef basic_string_view<T>                        supper_type;
    typedef basic_cstring_view<T>						this_type;
    typedef T 											value_type;
    typedef T* 											pointer;
    typedef const T* 									const_pointer;
    typedef T& 											reference;
    typedef const T& 									const_reference;
    typedef T* 											iterator;
    typedef const T* 									const_iterator;
    typedef std::reverse_iterator<iterator> 			reverse_iterator;
    typedef std::reverse_iterator<const_iterator> 	    const_reverse_iterator;
    typedef size_t 										size_type;
    typedef ptrdiff_t 									difference_type;
    static const WALLE_CONSTEXPR size_type npos = size_type(-1);

public:
    using supper_type::_begin;
    using supper_type::_count;
public:
    basic_cstring_view(const_pointer v) : basic_string_view<T>(v)
    {
        WALLE_ASSERT_MSG(_begin[_count] == '\0', "not \0 terminal");
    }

    const_pointer* c_str() const
    {
        return supper_type::_begin;
    }
public:
    using supper_type::begin;
    using supper_type::cbegin;
    using supper_type::end;
    using supper_type::cend;
    using supper_type::rbegin;
    using supper_type::crbegin;
    using supper_type::rend;
    using supper_type::crend;
    using supper_type::data;
  
    using supper_type::front;

    using supper_type::back;

    using supper_type::operator[];

    using supper_type::at;

    using supper_type::size;

	using supper_type::length;

    using supper_type::max_size;
	
    using supper_type::empty;

    WALLE_CPP14_CONSTEXPR void swap(basic_cstring_view& v)
    {
        std::swap(_begin, v._begin);
        std::swap(_count, v._count);
    }

    using supper_type::remove_prefix;

    using supper_type::remove_suffix;

    using supper_type::copy;
    using supper_type::compare;

    WALLE_CPP14_CONSTEXPR int compare(basic_cstring_view sw) const WALLE_NOEXCEPT
    {
        supper_type dummy(sw._begin, sw._count);
        return supper_type::compare(dummy);
    }

    using supper_type::substr;
    using supper_type::find;
    WALLE_CPP14_CONSTEXPR size_type find(basic_cstring_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find(sw.c_str(), pos, sw.size());
    }

	using supper_type::rfind;
	WALLE_CPP14_CONSTEXPR size_type rfind(basic_cstring_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {
        return rfind(sw.data(), pos, sw.size());
    }

    using supper_type::find_first_of;
    WALLE_CPP14_CONSTEXPR size_type find_first_of(basic_cstring_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find_first_of(sw._begin, pos, _count);
    }

    using supper_type::find_last_of;
    WALLE_CPP14_CONSTEXPR size_type find_last_of(basic_cstring_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {
        return find_last_of(sw._begin, pos, sw._count);
    }

    using supper_type::find_first_not_of;

    WALLE_CPP14_CONSTEXPR size_type find_first_not_of(basic_cstring_view sw, size_type pos = 0) const WALLE_NOEXCEPT
    {
        return find_first_not_of(sw._begin, pos, sw._count);
    }

    using supper_type::find_last_not_of;
    WALLE_CPP14_CONSTEXPR size_type find_last_not_of(basic_cstring_view sw, size_type pos = npos) const WALLE_NOEXCEPT
    {   
        return find_last_not_of(sw._begin, pos, sw._count);
    }

   
    
};

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator==(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator==(basic_cstring_view<CharT> lhs, basic_cstring_view<CharT> rhs)
{
    return (lhs.size() == rhs.size()) && (lhs.compare(rhs) == 0);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator==(basic_cstring_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    basic_string_view<CharT> dummy(lhs.data(), lhs.size());
    return (dummy.size() == rhs.size()) && (dummy.compare(rhs) == 0);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator==(basic_string_view<CharT> lhs, basic_cstring_view<CharT> rhs)
{
    basic_string_view<CharT> dummy(rhs.data(), rhs.size());
    return (lhs.size() == dummy.size()) && (dummy.compare(rhs) == 0);
}


template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator!=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return !(lhs == rhs);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator!=(basic_cstring_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return !(lhs == rhs);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator!=(basic_string_view<CharT> lhs, basic_cstring_view<CharT> rhs)
{
    return !(lhs == rhs);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator<(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return lhs.compare(rhs) < 0;
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator<=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return !(rhs < lhs);
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator>(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return rhs < lhs;
}

template <class CharT>
inline WALLE_CPP14_CONSTEXPR bool operator>=(basic_string_view<CharT> lhs, basic_string_view<CharT> rhs)
{
    return !(lhs < rhs);
}

typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;


typedef basic_string_view<char8_t> u8string_view;  // Actually not a C++17 type, but added for consistency.
typedef basic_string_view<char16_t> u16string_view;
typedef basic_string_view<char32_t> u32string_view;

typedef basic_cstring_view<char> cstring_view;
typedef basic_string_view<wchar_t> wcstring_view;


typedef basic_cstring_view<char8_t> u8cstring_view;  // Actually not a C++17 type, but added for consistency.
typedef basic_cstring_view<char16_t> u16cstring_view;
typedef basic_cstring_view<char32_t> u32cstring_view;

template <typename T> struct hash;
template<> struct hash<string_view> {
    size_t operator()(const string_view& x) const
    {
        const unsigned char* p = (const unsigned char*)x.data(); // To consider: limit p to at most 256 chars.
        unsigned int c, result = 2166136261U; // We implement an FNV-like string hash. 
        while((c = *p++) != 0) // Using '!=' disables compiler warnings.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};

template<> struct hash<u16string_view> {
    size_t operator()(const u16string_view& x) const
    {
        const char16_t* p = x.data();
        unsigned int c, result = 2166136261U;
        while((c = *p++) != 0)
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};

template<> struct hash<u32string_view> {
    size_t operator()(const u32string_view& x) const
    {
        const char32_t* p = x.data();
        unsigned int c, result = 2166136261U;
        while((c = (unsigned int)*p++) != 0)
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};

#if defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE
    template<> struct hash<wstring_view> {
        size_t operator()(const wstring_view& x) const
        {
            const wchar_t* p = x.data();
            unsigned int c, result = 2166136261U;
            while((c = (unsigned int)*p++) != 0)
                result = (result * 16777619) ^ c;
            return (size_t)result;
        }
    };
#endif //defined(WALLE_WCHAR_UNIQUE) && WALLE_WCHAR_UNIQUE

}
#endif //WALLE_WSL_STRING_VIEW_H_