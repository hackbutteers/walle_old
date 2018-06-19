#ifndef WALLE_WSL_ASCII_H_
#define WALLE_WSL_ASCII_H_
#include <walle/config/base.h>
#include <cstdint>

namespace wsl {

/**
* This class contains enumerations and static
* utility functions for dealing with ASCII characters
* and their properties.
*
* The classification functions will also work if
* non-ASCII character codes are passed to them,
* but classification will only check for
* ASCII characters.
*
* This allows the classification methods to be used
* on the single bytes of a UTF-8 string, without
* causing assertions or inconsistent results (depending
* upon the current locale) on bytes outside the ASCII range,
* as may be produced by ascii::isSpace(), etc.
*/

class  ascii {
public:
	//* ASCII character properties.
	enum CharacterProperties {
        eAcpControl = 0x0001,
        eAcpSpace    = 0x0002,
        eAcpPunct    = 0x0004,
        eAcpDigit    = 0x0008,
        eAcpHexDigit = 0x0010,
        eAcpAlpha    = 0x0020,
        eAcpLower    = 0x0040,
        eAcpUpper    = 0x0080,
        eAcpGraph    = 0x0100,
        eAcpPrint    = 0x0200
	};

	/**
	 * @return the ASCII character properties for the
	 * character with the given ASCII value.
	 *
	 * If the character is outside the ASCII range
	 * (0 .. 127), 0 is returned.
	 */
	static int properties(int ch);


	/**
	 * @returns true if the given character is
	 * within the ASCII range and has at least one of 
	 * the given properties.
	 */

	static bool has_some_properties(int ch, int properties);

	/**
	 * @returns true if the given character is
	 * within the ASCII range and has all of 
	 * the given properties.
	 */
	static bool has_properties(int ch, int properties);

	/**
	 * @returns true iff the given character code is within
	 * the ASCII range (0 .. 127).
	 */
	static bool is_ascii(int ch);
	
	/**
	 * @returns true iff the given character is a whitespace.
	 */
	static bool is_space(int ch);
		
	/**
	 * @returns true iff the given character is a digit.
	 */	
	static bool is_digit(int ch);
		
	/**
	 * @returns true iff the given character is a hexadecimal digit.
	 */	
	static bool is_hex_digit(int ch);
		
	/**
	 * @returns true iff the given character is a punctuation character.
	 */			
	static bool is_punct(int ch);

	/**
	 * Returns true iff the given character is an alphabetic character.	
	 */				
	static bool is_alpha(int ch);
	
	/**
	 * Returns true iff the given character is an alphabetic character.	
	 */	
	static bool is_alpha_numeric(int ch);
			
	/**
	 * @return true iff the given character is a lowercase alphabetic
	 * character.	
	 */		
	static bool is_lower(int ch);
	
	/**
	 * @returns true iff the given character is an uppercase alphabetic
	 * character.	
	 */			
	static bool is_upper(int ch);
	
	/**
	 * If the given character is an uppercase character,
	 * return its lowercase counterpart, otherwise return
	 * the character.
	 */
	static int to_lower(int ch);

	/**
	 * If the given character is a lowercase character,
	 * return its uppercase counterpart, otherwise return
	 * the character.
	 */
	static int to_upper(int ch);

		
private:
	static const int CHARACTER_PROPERTIES[128];
};


//
// WALLE_FORCE_INLINEs
//
WALLE_FORCE_INLINE int ascii::properties(int ch)
{
	if (is_ascii(ch)) 
		return CHARACTER_PROPERTIES[ch];
	else
		return 0;
}


WALLE_FORCE_INLINE bool ascii::is_ascii(int ch)
{
	return (static_cast<uint32_t>(ch) & 0xFFFFFF80) == 0;
}


WALLE_FORCE_INLINE bool ascii::has_properties(int ch, int props)
{
	return (properties(ch) & props) == props;
}


WALLE_FORCE_INLINE bool ascii::has_some_properties(int ch, int props)
{
	return (properties(ch) & props) != 0;
}


WALLE_FORCE_INLINE bool ascii::is_space(int ch)
{
	return has_properties(ch, eAcpSpace);
}


WALLE_FORCE_INLINE bool ascii::is_digit(int ch)
{
	return has_properties(ch, eAcpDigit);
}


WALLE_FORCE_INLINE bool ascii::is_hex_digit(int ch)
{
	return has_properties(ch, eAcpHexDigit);
}


WALLE_FORCE_INLINE bool ascii::is_punct(int ch)
{
	return has_properties(ch, eAcpPunct);
}


WALLE_FORCE_INLINE bool ascii::is_alpha(int ch)
{
	return has_properties(ch, eAcpAlpha);
}


WALLE_FORCE_INLINE bool ascii::is_alpha_numeric(int ch)
{
	return has_some_properties(ch, eAcpAlpha | eAcpDigit);
}


WALLE_FORCE_INLINE bool ascii::is_lower(int ch)
{
	return has_properties(ch, eAcpLower);
}


WALLE_FORCE_INLINE bool ascii::is_upper(int ch)
{
	return has_properties(ch, eAcpUpper);
}


WALLE_FORCE_INLINE int ascii::to_lower(int ch)
{
	if (is_upper(ch)) {
        return ch + 32;
    } else {
        return ch;
    }
}


WALLE_FORCE_INLINE int ascii::to_upper(int ch)
{
	if (is_lower(ch)) {
        return ch - 32;
    } else {
        return ch;
    }
}

} //namespace wsl
#endif //WALLE_WSL_ASCII_H_

