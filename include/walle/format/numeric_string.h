#ifndef WALLE_FORMAT_NUMERIC_STRING_H_
#define WALLE_FORMAT_NUMERIC_STRING_H_
#include <walle/strings/ascii.h>
#include <walle/strings/string_ref.h>
#include <walle/strings/string_builder.h>
#include <limits>
#include <cmath>
#include <cassert>
#include <ctype.h>
#include <cstdint>
#include <cstddef>
#include <string>


namespace walle {

static const int   kMaxIntStringLen = 65;
static const int   kMaxFLTStringLen = 780;
static const char *kFltInf = "inf";
static const char *kFltNan = "nan";
static const char  kFltExp = 'e';

inline std::string get_inf_str()
{
	return kFltInf;
}

inline std::string get_nan_str()
{
	return kFltNan;
}
/*!
 * Returns decimal separator from global locale or
 * default '.' for platforms where locale is unavailable.
 * @author lothar<liyinbin> 2017-02-19
 * @return decimal separator from global locale or
 * default '.' for platforms where locale is unavailable.
 */
inline char decimal_separator()
{
	return '.';
}

/*!
 * [thousandSeparator description]
 * @author lothar<liyinbin> 2017-02-19
 * @return thousand separator from global locale or 
 *  default ',' for platforms where locale is unavailable.
 */
inline char thousand_separator()
{
	return ',';
}

/*!
 * Converts zero-terminated character array to integer number;
 * Thousand separators are recognized for base10 and current locale;
 * it is silently skipped but not verified for correct positioning.
 * Function returns true if successful. If parsing was unsuccessful,
 * the return value is false with the result value undetermined.
 */
template <typename I>
bool str_to_int(StringRef ref, I& result, short base, char thSep = ',')
{
    const char *pStr = ref.data();
    const char *pStrEnd = ref.data() + ref.size();
	if (!pStr) {
        return false;
    }
	while (pStr != pStrEnd && Ascii::is_space(*pStr)) {
		++pStr;
	}

	if (pStr == pStrEnd || *pStr == '\0') {
		return false;
	}
	short sign = 1;
	if ((base == 10) && (*pStr == '-')) {
		if (std::numeric_limits<I>::min() >= 0) {
			return false;
		}
		sign = -1;
		++pStr;
	} else if (*pStr == '+') {
		++pStr;
	}

	// parser states:
	const char STATE_SIGNIFICANT_DIGITS = 1;
	char state = 0;
	
	result = 0;
	I limitCheck = std::numeric_limits<I>::max() / base;
	for (; pStr != pStrEnd && *pStr != '\0'; ++pStr) {
		switch (*pStr) {
			case '0': 
				if (state < STATE_SIGNIFICANT_DIGITS) break;

			case '1': 
			case '2': 
			case '3': 
			case '4':
			case '5': 
			case '6': 
			case '7':
				if (state < STATE_SIGNIFICANT_DIGITS) 
					state = STATE_SIGNIFICANT_DIGITS;
				if (result > limitCheck) 
					return false;
				result = result * base + (*pStr - '0');

				break;

			case '8':
			case '9':
				if ((base == 10) || (base == 0x10)){
					if (state < STATE_SIGNIFICANT_DIGITS) 
						state = STATE_SIGNIFICANT_DIGITS;
					if (result > limitCheck) 
						return false;
					result = result * base + (*pStr - '0');
				}else {
					return false;
				}

				break;

			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
				if (base != 0x10) 
					return false;
				if (state < STATE_SIGNIFICANT_DIGITS) 
					state = STATE_SIGNIFICANT_DIGITS;
				if (result > limitCheck) 
					return false;
				result = result * base + (10 + *pStr - 'a');

				break;

			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				if (base != 0x10) 
					return false;
				if (state < STATE_SIGNIFICANT_DIGITS) 
					state = STATE_SIGNIFICANT_DIGITS;
				if (result > limitCheck) 
					return false;
				result = result * base + (10 + *pStr - 'A');

				break;

			case '.':
				if ((base == 10) && (thSep == '.')) 
					break;
				else 
					return false;

			case ',':
				if ((base == 10) && (thSep == ',')) 
					break;
				else 
					return false;

			case ' ':
				if ((base == 10) && (thSep == ' ')) 
					break;
				// fallthrough

			default:
				return false;
		}
	}

	if ((sign < 0) && (base == 10)) 
		result *= sign;

	return true;
}


template <typename I>
bool str_to_int(const std::string& str, I& result, short base, char thSep = ',')
{
	return str_to_int(StringRef(str), result, base, thSep);
}


/** 
 * @brief Converts integer to string. 
 * @note    Numeric bases from binary to hexadecimal are supported.
 * If width is non-zero, it pads the return value with fill character to the specified width.
 *  When padding is zero character ('0'), it is prepended to the number itself; all other
 *  paddings are prepended to the formatted result with minus sign or base prefix included
 *  If prefix is true and base is octal or hexadecimal, respective prefix ('0' for octal, 
 *  "0x" for hexadecimal) is prepended. For all other bases, prefix argument is ignored.
 *  Formatted string has at least [width] total length.
 * @param  value: 
 * @param  base: 
 * @param  result: 
 * @param  size: 
 * @param  prefix: 
 * @param  width: 
 * @param  fill: 
 * @param  thSep: 
 * @retval 
 */
template <typename T>
bool int_to_str(T value, 
    unsigned short base,
    StringBuilder &result,
	bool prefix = false,
	int width = -1,
	char fill = ' ',
	char thSep = 0)
{
	if (base < 2 || base > 0x10) {
		return false;
	}

	int thCount = 0;
	T tmpVal;
	do {
		tmpVal = value;
		value /= base;
		const char c = "FEDCBA9876543210123456789ABCDEF"[15 + (tmpVal - value * base)];
        result.add_character(c);
		if (thSep && (base == 10) && (++thCount == 3)) {
            result.add_character(thSep);
			thCount = 0;
		}
	} while (value);

	if ('0' == fill) {
		if (tmpVal < 0) {
            --width;
        }
		if (prefix && base == 010) {
            --width;
        }
		if (prefix && base == 0x10) {
            width -= 2;
        }
		while (result.position() < width) {
            result.add_character(fill);
        }
	}

	if (prefix && base == 010) {
        result.add_character('0');
	} else if (prefix && base == 0x10) {
        result.add_character('x');
        result.add_character('0');
	}

	if (tmpVal < 0) {
        result.add_character('-');
    }

	if ('0' != fill) {
		while (result.position() < width) {
            result.add_character(fill);
        }
	}

	int size = result.position();
	assert ((-1 == width) || (size >= size_t(width)));
    WALLE_ASSERT((width == -1) || (size >= width), "");
    result.reverse();
	return true;
}

/** 
 * @brief Converts unsigned integer to string. 
 * @note  Numeric bases from binary to hexadecimal are supported.
 * If width is non-zero, it pads the return value with fill character to the specified width.
 * When padding is zero character ('0'), it is prepended to the number itself; all other
 * paddings are prepended to the formatted result with minus sign or base prefix included
 * If prefix is true and base is octal or hexadecimal, respective prefix ('0' for octal,
 * "0x" for hexadecimal) is prepended. For all other bases, prefix argument is ignored.
 * Formatted string has at least [width] total length.
 * @param  value: 
 * @param  base: 
 * @param  &result: 
 * @param  prefix: 
 * @param  width: 
 * @param  fill: 
 * @param  thSep: 
 * @retval 
 */
template <typename T>
bool uint_to_str(T value,
	unsigned short base,
    StringBuilder &result,
	bool prefix = false,
	int width = -1,
	char fill = ' ',
	char thSep = 0)
{
	if (base < 2 || base > 0x10) {
		return false;
	}
	
	int thCount = 0;
	T tmpVal;
	do {
		tmpVal = value;
		value /= base;
		const char  c = "FEDCBA9876543210123456789ABCDEF"[15 + (tmpVal - value * base)];
        result.add_character(c);
		if (thSep && (base == 10) && (++thCount == 3)) {
            result.add_character(thSep);
			thCount = 0;
		}
	} while (value);
	
	if ('0' == fill) {
		if (prefix && base == 010) {
            --width;
        }
		if (prefix && base == 0x10) {
            width -= 2;
        }
		while (result.position() < width) {
            result.add_character(fill);
        }
	}
	
	if (prefix && base == 010) {
        result.add_character('0');
	} else if (prefix && base == 0x10) {
        result.add_character('x');
        result.add_character('0');
	}
	
	if ('0' != fill) {
		while (result.position() < width) {
            result.add_character(fill);
		}
	}
	result.reverse();
	return true;
}


/**
 * Converts integer to string; This is a wrapper function, for details see see the
 * bool intToStr(T, unsigned short, char*, int, int, char, char) implementation.	
 */
template <typename T>
bool int_to_str (T number, unsigned short base, std::string& result, bool prefix = false, int width = -1, char fill = ' ', char thSep = 0)
{
    StdStringBuilderHelper<kMaxIntStringLen> helper(result);
	bool ret = int_to_str(number, base, helper.builder(), prefix, width, fill, thSep);
	return ret;
}
	
/**
 * Converts unsigned integer to string; This is a wrapper function, for details see see the
 * bool uIntToStr(T, unsigned short, char*, int, int, char, char) implementation.
 */
template <typename T>
bool uint_to_str (T number, unsigned short base, std::string& result, bool prefix = false, int width = -1, char fill = ' ', char thSep = 0)
{
    StdStringBuilderHelper<kMaxIntStringLen> helper(result);
	bool ret = uint_to_str(number, base, helper.builder(), prefix, width, fill, thSep);
	return ret;
}


//
// Wrappers for double-conversion library (http://code.google.com/p/double-conversion/).
//
// Library is the implementation of the algorithm described in Florian Loitsch's paper:
// http://florian.loitsch.com/publications/dtoa-pldi2010.pdf
//

/**
 * Converts a float value to string. Converted string must be shorter than bufferSize.
 * Conversion is done by computing the shortest string of digits that correctly represents
 * the input number. Depending on lowDec and highDec values, the function returns
 * decimal or exponential representation.
 */
void float_to_str(StringBuilder &result,
	float value,
	int lowDec = -std::numeric_limits<float>::digits10,
	int highDec = std::numeric_limits<float>::digits10);
/**
 * Converts a float value to string. Converted string must be shorter than bufferSize.
 * Computes a decimal representation with a fixed number of digits after the
 * decimal point.
 */
void float_to_fixed_str(StringBuilder &result,
	float value,
	int precision);

/**
 * Converts a float value, assigns it to the supplied string and returns the reference.
 * This function calls floatToStr(char*, int, float, int, int) and formats the result according to
 * precision (total number of digits after the decimal point, -1 means ignore precision argument) 
 * and width (total length of formatted string).
 */
std::string& float_to_str(std::string& str,
	float value,
	int precision = -1,
	int width = 0,
	char thSep = 0,
	char decSep = 0);


/**
 * Converts a float value, assigns it to the supplied string and returns the reference.
 * This function calls floatToFixedStr(char*, int, float, int) and formats the result according to
 * precision (total number of digits after the decimal point) and width (total length of formatted string).
 */
std::string& float_to_fixed_str(std::string& str,
	float value,
	int precision,
	int width = 0,
	char thSep = 0,
	char decSep = 0);

/**
 * Converts a double value to string. Converted string must be shorter than bufferSize.
 * Conversion is done by computing the shortest string of digits that correctly represents
 * the input number. Depending on lowDec and highDec values, the function returns
 * decimal or exponential representation.
 */

void double_to_str(StringBuilder &result,
	double value,
	int lowDec = -std::numeric_limits<double>::digits10,
	int highDec = std::numeric_limits<double>::digits10);


/*!
 * Converts a double value to string. Converted string must be shorter than bufferSize.
 * Computes a decimal representation with a fixed number of digits after the
 * decimal point.
 */
void double_to_fixed_str(StringBuilder &result,
	double value,
	int precision);

/*!
 * Converts a double value, assigns it to the supplied string and returns the reference.
 * This function calls doubleToStr(char*, int, double, int, int) and formats the result according to
 * precision (total number of digits after the decimal point, -1 means ignore precision argument) 
 * and width (total length of formatted string).
 */
std::string& double_to_str(std::string& str,
	double value,
	int precision = -1,
	int width = 0,
	char thSep = 0,
	char decSep = 0);


/*!
 * Converts a double value, assigns it to the supplied string and returns the reference.
 * This function calls doubleToFixedStr(char*, int, double, int) and formats the result according to
 * precision (total number of digits after the decimal point) and width (total length of formatted string).
 */
std::string& double_to_fixed_str(std::string& str,
	double value,
	int precision = -1,
	int width = 0,
	char thSep = 0,
	char decSep = 0);


/**
 * Converts the string of characters into single-precision floating point number.
 * Function uses double_convesrion::DoubleToStringConverter to do the conversion.
 */
float str_to_float(StringRef ref);


float str_to_float(StringRef ref, int &proccessed);

/**
 * Converts the string of characters into single-precision floating point number.
 * The conversion result is assigned to the result parameter.
 * If decimal separator and/or thousand separator are different from defaults, they should be
 * supplied to ensure proper conversion.
 * 
 * Returns true if successful, false otherwise.
 */
	
bool str_to_float(const std::string&, float& result, char decSep = '.', char thSep = ',');


/**
 * Converts the string of characters into double-precision floating point number.
 */
double str_to_double(StringRef ref);
double str_to_double(StringRef ref, int &processed);
	
/**
 * Converts the string of characters into double-precision floating point number.
 * The conversion result is assigned to the result parameter.
 * If decimal separator and/or thousand separator are different from defaults, they should be
 * supplied to ensure proper conversion.
 * 
 *  Returns true if successful, false otherwise.
 */

bool str_to_double(const std::string& str, double& result, char decSep = '.', char thSep = ',');




} //namespace walle
#endif //WALLE_FORMAT_NUMERIC_STRING_H_
