#ifndef WALLE_STRINGS_STRING_BUILDER_H_
#define WALLE_STRINGS_STRING_BUILDER_H_
#include <walle/buffer/buffer_ref.h>
#include <walle/strings/string_ref.h>
#include <string>
#include <cstddef>
#include <type_traits>
#include <cstdlib>
#include <cstring>

namespace walle {

static const int kCharSize = sizeof(char);

template <typename Char>
WALLE_FORCE_INLINE int str_length(const Char *str)
{
    size_t length = std::char_traits<Char>::length(str);
    WALLE_ASSERT(length == static_cast<size_t>(static_cast<int>(length)), "");
    return static_cast<int>(length);
}
/** 
 * @brief  Helper class for building result strings in a character buffer.
 * @note    The purpose of the class is to use safe operations that checks 
 *          the buffer bounds on all operations in debug mode.
 * @retval None
 */

template <typename Char>
class BasicStringBuilder {
 public:
    BasicStringBuilder(Char* buffer, int bufferSize)
        : _buffer(buffer, bufferSize), 
          _position(0),
          _finalize(false) 
    {

    }

    BasicStringBuilder(BasicStringRef<Char> ref)
        : _buffer(ref.data(), ref.size()),
         _position(0),
         _finalize(false)
    { 

    }
    ~BasicStringBuilder() 
    { 
        if (!is_finalized()) {
            finalize(); 
        }
    }

    int size() const 
    { 
        return _buffer.size(); 
    }

    // Get the current position in the builder.
    int position() const 
    {
        WALLE_ASSERT(!is_finalized(), "");
        return _position;
    }

    /** 
     * @brief  Reset the position.
     * @note   
     * @retval None
     */
    void reset() 
    { 
        _position = 0; 
        _finalize = false;
    }

    /** 
     * @brief  Add a single character to the builder. 
     * @note  It is not allowed to add
     *        0-characters; use the Finalize() method to terminate the string instead. 
     * @param  c: 
     * @retval None
     */
    void add_character(Char c) 
    {
        WALLE_ASSERT(c != '\0', "");
        WALLE_ASSERT(!is_finalized() && static_cast<size_t>(_position) < _buffer.size(), "");
        _buffer[_position++] = c;
    }

    // Add an entire string to the builder. Uses strlen() internally to
    // compute the length of the input string.
    /** 
     * @brief Add an entire string to the builder.
     * @note   
     * @param  s: 
     * @retval None
     */
    void add_string(const Char* s) 
    {
        add_substring(s, str_length(s));
    }

    // Add the first 'n' characters of the given string 's' to the
    // builder. The input string must have enough characters.
    void add_substring(const Char* s, int n) 
    {
        WALLE_ASSERT(!is_finalized(),"");
        WALLE_ASSERT(static_cast<size_t>( _position + n) < _buffer.size(), "");
        WALLE_ASSERT(static_cast<size_t>(n) <= static_cast<size_t>(str_length(s)), "");
        memmove(&_buffer[_position], s, n * kCharSize);
        _position += n;
   }

   /** 
    * @brief  dd character padding to the builder.
    * @note  If count is non-positive, nothing is added to the builder.
    * @param  c: 
    * @param  count: 
    * @retval None
    */
    void add_padding(Char c, int count) 
    {
        for (int i = 0; i < count; i++) {
            add_character(c);
        }
    }

    void reverse()
    {
        int mid = _position/2;
        for(int i =0; i < mid; i++){
            std::swap(_buffer[i], _buffer[_position - i]);
        }
    }

    // Finalize the string by 0-terminating it and returning the buffer.
    char* finalize() 
    {
        WALLE_ASSERT(static_cast<size_t>(_position) < _buffer.size(), "");
        if(!is_finalized()) {
            _buffer[_position] = '\0';
            _finalize = true;
            // Make sure nobody managed to add a 0-character to the
            // buffer while building the string.
            WALLE_ASSERT(str_length(_buffer.data()) == static_cast<size_t>(_position),"");
            WALLE_ASSERT(is_finalized(), "");
        }
        return _buffer.data();
    }

private:
    BufferRef<Char>         _buffer;
    int                     _position;
    bool                    _finalize;
private:
    bool is_finalized() const 
    { 
        return _finalize; 
    }

    WALLE_NON_COPYABLE(BasicStringBuilder);
    WALLE_DISABLE_DEFAULT_CTOR(BasicStringBuilder);
};

typedef BasicStringBuilder<char> StringBuilder;


WALLE_FORCE_INLINE std::string& resize_helper(std::string &str, size_t s)
{
    str.resize(s);
    return str;
};
template <size_t Size>
class StdStringBuilderHelper{
public:
    explicit StdStringBuilderHelper(std::string &str)
    : _ref(resize_helper(str, Size)),
       _builder(&_ref[0], Size)
    {

    }

    ~StdStringBuilderHelper()
    {
        _ref.resize(_builder.position());
    }

    StringBuilder& builder()
    {
        return _builder;
    }

private:
    std::string  &_ref;
    StringBuilder _builder;
    WALLE_DISABLE_DEFAULT_CTOR(StdStringBuilderHelper);
};

template <size_t Size>
class StdStringBuilderAppendHelper{
public:
    explicit StdStringBuilderAppendHelper(std::string &str)
    : _old_size(str.size()),
      _ref(resize_helper(str, _old_size + Size)),
      _builder(&_ref[_old_size], Size)
    {
        
    }

    StringBuilder& builder()
    {
        return _builder;
    }
    
    ~StdStringBuilderAppendHelper()
    {
        _ref.resize(_builder.position() + _old_size);
    }
private:
    size_t        _old_size;
    std::string  &_ref;  
    StringBuilder _builder;
    WALLE_DISABLE_DEFAULT_CTOR(StdStringBuilderAppendHelper);
};

} //namespace walle
#endif //WALLE_STRINGS_STRING_BUILDER_H_