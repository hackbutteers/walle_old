#ifndef WALLE_STRINGS_STRING_REF_H_
#define WALLE_STRINGS_STRING_REF_H_
#include <walle/config/base.h>
#include <string>
#include <cstring>
#include <cstddef>

namespace walle {

template <typename Char>
class BasicStringRef {
public:
    /** Constructs a string reference object from a C string and a size. */
    BasicStringRef(const Char *str, std::size_t s) : _data(str), _size(s)
    {}


    BasicStringRef(const Char *s)
        : _data(s), _size(std::char_traits<Char>::length(s))
    {

    }

    BasicStringRef(const BasicStringRef<Char> &s)
        : _data(s.data()), _size(s.size())
    {}

    BasicStringRef(const std::basic_string<Char> &s)
        : _data(const_cast<Char*>(&s[0])),  
          _size(s.size())
    {}
    BasicStringRef<Char>& operator=(const std::basic_string<Char> &s)
    {
        _data = s.data();
        _size = s.size();
    }
    BasicStringRef<Char> sub_string_ref(size_t from, size_t to)
    {
        WALLE_ASSERT(from < to, "");
        WALLE_ASSERT(to <= _size, "");
        return BasicStringRef<Char>(_data + from, to - from);

    }
    std::basic_string<Char> to_string() const
    {
        return std::basic_string<Char>(_data, _size);
    }

    /** Returns a pointer to the string data. */
    const Char *data() const
    {
        return _data;
    }

    /** Returns the string size. */
    std::size_t size() const
    {
        return _size;
    }

    const Char operator[](size_t n) const 
    {
        WALLE_ASSERT(n < size(), "");
        return _data[n];
    }

    
    void clear() 
    { 
        _data = ""; _size = 0; 
    }

    void remove_prefix(size_t n) 
    {
        WALLE_ASSERT(n <= size(), "");
        _data += n;
        _size -= n;
    }

    void remove_postfix(size_t n)
    {
        WALLE_ASSERT(n <= size(), "");
        _size -= n;   
    }

    void resize(size_t n)
    {
        WALLE_ASSERT(n <= size(), "");
        _size = n;   
    }

    bool starts_with(const BasicStringRef<Char>& x) const 
    {
        return ((_size >= x._size) &&
            (memcmp(_data, x._data, x._size) == 0));
    }
    bool empty()
    {
        return _size == 0;
    }
    // Lexicographically compare this string reference to other.
    int compare(const BasicStringRef& other) const
    {
        std::size_t s = _size < other._size ? _size : other._size;
        int result = std::char_traits<Char>::compare(_data, other._data, s);
        if (result == 0)
            result = _size == other._size ? 0 : (_size < other._size ? -1 : 1);
        return result;
    }


    friend bool operator==(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) == 0;
    }
    friend bool operator!=(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) != 0;
    }
    friend bool operator<(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) < 0;
    }
    friend bool operator<=(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) <= 0;
    }
    friend bool operator>(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) > 0;
    }
    friend bool operator>=(BasicStringRef lhs, BasicStringRef rhs)
    {
        return lhs.compare(rhs) >= 0;
    }
private:
    void check(char* ptr)
	{
        if (ptr > _data + _size) 
            WALLE_ASSERT(false, "over flow");
	}
private:
    const Char   *_data;
    std::size_t   _size;
};

typedef BasicStringRef<const char> ConstStringRef;
typedef BasicStringRef<const wchar_t> ConstWStringRef;

typedef BasicStringRef<char> StringRef;
typedef BasicStringRef<wchar_t> WStringRef;

template <typename Char>
class BasicCStringRef {
private:
    const Char *_data;

public:
    /** Constructs a string reference object from a C string. */
    BasicCStringRef(const Char *s): _data(s)
    {}

    /**
    \rst
    Constructs a string reference from an ``std::string`` object.
    \endrst
    */
    BasicCStringRef(const std::basic_string<Char> &s): _data(s.c_str())
    {}

    /** Returns the pointer to a C string. */
    const Char *c_str() const
    {
        return _data;
    }
};

typedef BasicCStringRef<char> CStringRef;
typedef BasicCStringRef<wchar_t> WCStringRef;

} //namespace walle
#endif //WALLE_STRINGS_STRING_REF_H_