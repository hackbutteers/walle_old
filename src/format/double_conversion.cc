#include <walle/format/detail/double_conversion.h>
#include <walle/format/detail/fast_dtoa.h>
#include <walle/format/detail/bignum_dtoa.h>
#include <walle/format/detail/fixed_dtoa.h>
#include <walle/format/detail/ieee.h>
#include <walle/format/detail/strtod.h>
#include <climits>
#include <cmath>

namespace walle {
namespace format_detail {

const DoubleToStringConverter& DoubleToStringConverter::ecma_script_converter() 
{
    int flags = eUniqueZero | eEmitPositiveExponentSign;
    static DoubleToStringConverter converter(flags,
                                            "Infinity",
                                            "NaN",
                                            'e',
                                            -6, 21,
                                            6, 0);
    return converter;
}


bool DoubleToStringConverter::handle_special_values(
    double value,
    StringBuilder* resultBuilder) const 
{
    Double doubleInspect(value);
    if (doubleInspect.is_infinite()) {
        if (_infinity_symbol == NULL) return false;
        if (value < 0) {
            resultBuilder->add_character('-');
        }
        resultBuilder->add_string(_infinity_symbol);
        return true;
    }
    if (doubleInspect.is_nan()) {
        if (_nan_symbol == NULL) {
            return false;
        }
        resultBuilder->add_string(_nan_symbol);
        return true;
    }
    return false;
}


void DoubleToStringConverter::create_exponential_representation(
    const char* decimal_digits,
    int length,
    int exponent,
    StringBuilder* resultBuilder) const 
{
    WALLE_ASSERT(length != 0, "");
    resultBuilder->add_character(decimal_digits[0]);
    if (length != 1) {
        resultBuilder->add_character('.');
        resultBuilder->add_substring(&decimal_digits[1], length-1);
    }
    resultBuilder->add_character(_exponent_character);
    if (exponent < 0) {
        resultBuilder->add_character('-');
        exponent = -exponent;
    } else {
        if ((_flags & eEmitPositiveExponentSign) != 0) {
            resultBuilder->add_character('+');
        }
    }
    if (exponent == 0) {
        resultBuilder->add_character('0');
        return;
    }
    WALLE_ASSERT(exponent < 1e4, "");
    const int kMaxExponentLength = 5;
    char buffer[kMaxExponentLength + 1];
    buffer[kMaxExponentLength] = '\0';
    int first_char_pos = kMaxExponentLength;
    while (exponent > 0) {
        buffer[--first_char_pos] = '0' + (exponent % 10);
        exponent /= 10;
    }
    resultBuilder->add_substring(&buffer[first_char_pos],
                               kMaxExponentLength - first_char_pos);
}


void DoubleToStringConverter::create_decimal_representation(
    const char* decimal_digits,
    int length,
    int decimal_point,
    int digits_after_point,
    StringBuilder* resultBuilder) const 
{
      // Create a representation that is padded with zeros if needed.
    if (decimal_point <= 0) {
          // "0.00000decimal_rep" or "0.000decimal_rep00".
        resultBuilder->add_character('0');
        if (digits_after_point > 0) {
            resultBuilder->add_character('.');
            resultBuilder->add_padding('0', -decimal_point);
            WALLE_ASSERT(length <= digits_after_point - (-decimal_point), "");
            resultBuilder->add_substring(decimal_digits, length);
            int remaining_digits = digits_after_point - (-decimal_point) - length;
            resultBuilder->add_padding('0', remaining_digits);
        }
    } else if (decimal_point >= length) {
        // "decimal_rep0000.00000" or "decimal_rep.0000".
        resultBuilder->add_substring(decimal_digits, length);
        resultBuilder->add_padding('0', decimal_point - length);
        if (digits_after_point > 0) {
            resultBuilder->add_character('.');
            resultBuilder->add_padding('0', digits_after_point);
        }
    } else {
        // "decima.l_rep000".
        WALLE_ASSERT(digits_after_point > 0, "");
        resultBuilder->add_substring(decimal_digits, decimal_point);
        resultBuilder->add_character('.');
        WALLE_ASSERT(length - decimal_point <= digits_after_point, "");
        resultBuilder->add_substring(&decimal_digits[decimal_point],
                                 length - decimal_point);
        int remaining_digits = digits_after_point - (length - decimal_point);
        resultBuilder->add_padding('0', remaining_digits);
    }
    if (digits_after_point == 0) {
        if ((_flags & eEmitTrailingDecimalpoint) != 0) {
            resultBuilder->add_character('.');
        }
        if ((_flags & eEmitTrailingZeroAfterPoint) != 0) {
            resultBuilder->add_character('0');
        }
    }
}


bool DoubleToStringConverter::to_shortest_ieee_number(
    double value,
    StringBuilder* resultBuilder,
    DoubleToStringConverter::DtoaMode mode) const 
{
    WALLE_ASSERT(mode == eShortest || mode == eShortestSingle, "");
    if (Double(value).is_special()) {
        return handle_special_values(value, resultBuilder);
    }

    int decimal_point;
    bool sign;
    const int kDecimalRepCapacity = kBase10MaximalLength + 1;
    char decimal_rep[kDecimalRepCapacity];
    int decimal_rep_length;

    double_to_ascii(value, mode, 0, decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);

    bool unique_zero = (_flags & eUniqueZero) != 0;
    if (sign && (value != 0.0 || !unique_zero)) {
        resultBuilder->add_character('-');
    }

    int exponent = decimal_point - 1;
    if ((_decimal_in_shortest_low <= exponent) &&
      (exponent < _decimal_in_shortest_high)) {
        create_decimal_representation(decimal_rep, decimal_rep_length,
                                decimal_point,
                                std::max(0, decimal_rep_length - decimal_point),
                                resultBuilder);
    } else {
        create_exponential_representation(decimal_rep, decimal_rep_length, exponent,
                                    resultBuilder);
    }
    return true;
}


bool DoubleToStringConverter::to_fixed(double value,
                                      int requested_digits,
                                      StringBuilder* resultBuilder) const 
{
    WALLE_ASSERT(kMaxFixedDigitsBeforePoint == 60, "");
    const double kFirstNonFixed = 1e60;

    if (Double(value).is_special()) {
        return handle_special_values(value, resultBuilder);
    }

    if (requested_digits > kMaxFixedDigitsAfterPoint) {
        return false;
    }
    if (value >= kFirstNonFixed || value <= -kFirstNonFixed) {
        return false;
    }
    // Find a sufficiently precise decimal representation of n.
    int decimal_point;
    bool sign;
    // Add space for the '\0' byte.
    const int kDecimalRepCapacity =
        kMaxFixedDigitsBeforePoint + kMaxFixedDigitsAfterPoint + 1;
    char decimal_rep[kDecimalRepCapacity];
    int decimal_rep_length;
    double_to_ascii(value, eFixed, requested_digits,
                decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);

    bool unique_zero = ((_flags & eUniqueZero) != 0);
    if (sign && (value != 0.0 || !unique_zero)) {
        resultBuilder->add_character('-');
    }

    create_decimal_representation(decimal_rep, decimal_rep_length, decimal_point,
                              requested_digits, resultBuilder);
    return true;
}


bool DoubleToStringConverter::to_exponential(
    double value,
    int requested_digits,
    StringBuilder* resultBuilder) const 
{
    if (Double(value).is_special()) {
        return handle_special_values(value, resultBuilder);
    }

    if (requested_digits < -1) {
        return false;
    }
    if (requested_digits > kMaxExponentialDigits) {
        return false;
    }
    int decimal_point;
    bool sign;
    // Add space for digit before the decimal point and the '\0' character.
    const int kDecimalRepCapacity = kMaxExponentialDigits + 2;
    WALLE_ASSERT(kDecimalRepCapacity > kBase10MaximalLength, "");
    char decimal_rep[kDecimalRepCapacity];
    int decimal_rep_length;

    if (requested_digits == -1) {
        double_to_ascii(value, eShortest, 0,
                  decimal_rep, kDecimalRepCapacity,
                  &sign, &decimal_rep_length, &decimal_point);
    } else {
        double_to_ascii(value, ePrecision, requested_digits + 1,
                  decimal_rep, kDecimalRepCapacity,
                  &sign, &decimal_rep_length, &decimal_point);
        WALLE_ASSERT(decimal_rep_length <= requested_digits + 1, "");

        for (int i = decimal_rep_length; i < requested_digits + 1; ++i) {
            decimal_rep[i] = '0';
        }
        decimal_rep_length = requested_digits + 1;
    }

    bool unique_zero = ((_flags & eUniqueZero) != 0);
    if (sign && (value != 0.0 || !unique_zero)) {
        resultBuilder->add_character('-');
    }

    int exponent = decimal_point - 1;
    create_exponential_representation(decimal_rep,
                                  decimal_rep_length,
                                  exponent,
                                  resultBuilder);
    return true;
}


bool DoubleToStringConverter::to_precision(double value,
                                          int precision,
                                          StringBuilder* resultBuilder) const 
{
    if (Double(value).is_special()) {
        return handle_special_values(value, resultBuilder);
    }

    if (precision < kMinPrecisionDigits || precision > kMaxPrecisionDigits) {
        return false;
    }

    // Find a sufficiently precise decimal representation of n.
    int decimal_point;
    bool sign;
    // Add one for the terminating null character.
    const int kDecimalRepCapacity = kMaxPrecisionDigits + 1;
    char decimal_rep[kDecimalRepCapacity];
    int decimal_rep_length;

    double_to_ascii(value, ePrecision, precision,
                decimal_rep, kDecimalRepCapacity,
                &sign, &decimal_rep_length, &decimal_point);
    WALLE_ASSERT(decimal_rep_length <= precision, "");

    bool unique_zero = ((_flags & eUniqueZero) != 0);
    if (sign && (value != 0.0 || !unique_zero)) {
        resultBuilder->add_character('-');
    }

    // The exponent if we print the number as x.xxeyyy. That is with the
    // decimal point after the first digit.
    int exponent = decimal_point - 1;

    int extra_zero = ((_flags & eEmitTrailingZeroAfterPoint) != 0) ? 1 : 0;
    if ((-decimal_point + 1 > _max_leading_padding_zeroes_in_precision_mode) ||
        (decimal_point - precision + extra_zero >
        _max_trailing_padding_zeroes_in_precision_mode)) {
        // Fill buffer to contain 'precision' digits.
        // Usually the buffer is already at the correct length, but 'DoubleToAscii'
        // is allowed to return less characters.
        for (int i = decimal_rep_length; i < precision; ++i) {
            decimal_rep[i] = '0';
        }

        create_exponential_representation(decimal_rep,
                                    precision,
                                    exponent,
                                    resultBuilder);
    } else {
        create_decimal_representation(decimal_rep, decimal_rep_length, decimal_point,
                                std::max(0, precision - decimal_point),
                                resultBuilder);
    }
    return true;
}


static BignumDtoaMode dtoa_to_bignum_dtoa_mode(
    DoubleToStringConverter::DtoaMode dtoa_mode) 
{
    switch (dtoa_mode) {
        case DoubleToStringConverter::eShortest:  return BignumDtoaMode::eBignumDtoaShortest;
        case DoubleToStringConverter::eShortestSingle:
            return BignumDtoaMode::eBignumDtoaShortestSingle;
        case DoubleToStringConverter::eFixed:     return BignumDtoaMode::eBignumDtoaFixed;
        case DoubleToStringConverter::ePrecision: return BignumDtoaMode::eBignumDtoaPrecision;
        default:
            WALLE_ASSERT(false, "");
  }
}


void DoubleToStringConverter::double_to_ascii(double v,
                                            DtoaMode mode,
                                            int requested_digits,
                                            char* buffer,
                                            int buffer_length,
                                            bool* sign,
                                            int* length,
                                            int* point) 
{
    walle::BufferRef<char> vector(buffer, buffer_length);
    WALLE_ASSERT(!Double(v).is_special(), "");
    WALLE_ASSERT(mode == eShortest || mode == eShortestSingle || requested_digits >= 0, "");

    if (Double(v).sign() < 0) {
        *sign = true;
        v = -v;
    } else {
        *sign = false;
    }

    if (mode == ePrecision && requested_digits == 0) {
        vector[0] = '\0';
        *length = 0;
        return;
    }

    if (v == 0) {
        vector[0] = '0';
        vector[1] = '\0';
        *length = 1;
        *point = 1;
        return;
    }

    bool fast_worked;
    switch (mode) {
        case eShortest:
            fast_worked = fast_dtoa(v, FastDtoaMode::eFastDtoaShortest, 0, vector, length, point);
            break;
        case eShortestSingle:
            fast_worked = fast_dtoa(v, FastDtoaMode::eFastDtoaShortestSingle, 0,
                             vector, length, point);
            break;
        case eFixed:
            fast_worked = fast_fixed_dtoa(v, requested_digits, vector, length, point);
            break;
        case ePrecision:
            fast_worked = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, requested_digits,
                             vector, length, point);
            break;
        default:
            fast_worked = false;
            WALLE_ASSERT(false, "");
    }
    if (fast_worked) {
        return;
    }

    // If the fast dtoa didn't succeed use the slower bignum version.
    BignumDtoaMode bignum_mode = dtoa_to_bignum_dtoa_mode(mode);
    bignum_dtoa(v, bignum_mode, requested_digits, vector, length, point);
    vector[*length] = '\0';
}


// Consumes the given substring from the iterator.
// Returns false, if the substring does not match.
template <class Iterator>
static bool consume_sub_string(Iterator* current,
                             Iterator end,
                             const char* substring) 
{
    WALLE_ASSERT(**current == *substring, "");
    for (substring++; *substring != '\0'; substring++) {
        ++*current;
        if (*current == end || **current != *substring) {
            return false;
        }
    }
    ++*current;
    return true;
}


// Maximum number of significant digits in decimal representation.
// The longest possible double in decimal representation is
// (2^53 - 1) * 2 ^ -1074 that is (2 ^ 53 - 1) * 5 ^ 1074 / 10 ^ 1074
// (768 digits). If we parse a number whose first digits are equal to a
// mean of 2 adjacent doubles (that could have up to 769 digits) the result
// must be rounded to the bigger one unless the tail consists of zeros, so
// we don't need to preserve all the digits.
const int kMaxSignificantDigits = 772;


static const char kWhitespaceTable7[] = { 32, 13, 10, 9, 11, 12 };
static const int kWhitespaceTable7Length = WALLE_ARRAY_SIZE(kWhitespaceTable7);


static const uint16_t kWhitespaceTable16[] = {
  160, 8232, 8233, 5760, 6158, 8192, 8193, 8194, 8195,
  8196, 8197, 8198, 8199, 8200, 8201, 8202, 8239, 8287, 12288, 65279
};
static const int kWhitespaceTable16Length = WALLE_ARRAY_SIZE(kWhitespaceTable16);


static bool is_whitespace(int x) {
  if (x < 128) {
    for (int i = 0; i < kWhitespaceTable7Length; i++) {
      if (kWhitespaceTable7[i] == x) return true;
    }
  } else {
    for (int i = 0; i < kWhitespaceTable16Length; i++) {
      if (kWhitespaceTable16[i] == x) return true;
    }
  }
  return false;
}


// Returns true if a nonspace found and false if the end has reached.
template <class Iterator>
static inline bool advance_to_nonspace(Iterator* current, Iterator end) {
  while (*current != end) {
    if (!is_whitespace(**current)) return true;
    ++*current;
  }
  return false;
}


static bool is_digit(int x, int radix) {
  return (x >= '0' && x <= '9' && x < '0' + radix)
      || (radix > 10 && x >= 'a' && x < 'a' + radix - 10)
      || (radix > 10 && x >= 'A' && x < 'A' + radix - 10);
}


static double signed_zero(bool sign) {
  return sign ? -0.0 : 0.0;
}


// Returns true if 'c' is a decimal digit that is valid for the given radix.
//
// The function is small and could be inlined, but VS2012 emitted a warning
// because it constant-propagated the radix and concluded that the last
// condition was always true. By moving it into a separate function the
// compiler wouldn't warn anymore.
#if _MSC_VER
#pragma optimize("",off)
static bool is_decimal_digit_for_radix(int c, int radix) {
  return '0' <= c && c <= '9' && (c - '0') < radix;
}
#pragma optimize("",on)
#else
static bool inline is_decimal_digit_for_radix(int c, int radix) {
	return '0' <= c && c <= '9' && (c - '0') < radix;
}
#endif
// Returns true if 'c' is a character digit that is valid for the given radix.
// The 'a_character' should be 'a' or 'A'.
//
// The function is small and could be inlined, but VS2012 emitted a warning
// because it constant-propagated the radix and concluded that the first
// condition was always false. By moving it into a separate function the
// compiler wouldn't warn anymore.
static bool is_character_digit_for_radix(int c, int radix, char a_character) {
  return radix > 10 && c >= a_character && c < a_character + radix - 10;
}


// Parsing integers with radix 2, 4, 8, 16, 32. Assumes current != end.
template <int radix_log_2, class Iterator>
static double radix_string_to_ieee(Iterator* current,
                                Iterator end,
                                bool sign,
                                bool allow_trailing_junk,
                                double junk_string_value,
                                bool read_as_double,
                                bool* result_is_junk) 
{
    WALLE_ASSERT(*current != end, "");

    const int kDoubleSize = Double::kSignificandSize;
    const int kSingleSize = Single::kSignificandSize;
    const int kSignificandSize = read_as_double? kDoubleSize: kSingleSize;

    *result_is_junk = true;

    // Skip leading 0s.
    while (**current == '0') {
        ++(*current);
        if (*current == end) {
            *result_is_junk = false;
            return signed_zero(sign);
        }
    }

    int64_t number = 0;
    int exponent = 0;
    const int radix = (1 << radix_log_2);

    do {
        int digit;
        if (is_decimal_digit_for_radix(**current, radix)) {
            digit = static_cast<char>(**current) - '0';
        } else if (is_character_digit_for_radix(**current, radix, 'a')) {
            digit = static_cast<char>(**current) - 'a' + 10;
        } else if (is_character_digit_for_radix(**current, radix, 'A')) {
            digit = static_cast<char>(**current) - 'A' + 10;
        } else {
            if (allow_trailing_junk || !advance_to_nonspace(current, end)) {
                break;
            } else {
                return junk_string_value;
            }
        }

        number = number * radix + digit;
        int overflow = static_cast<int>(number >> kSignificandSize);
        if (overflow != 0) {
            // Overflow occurred. Need to determine which direction to round the
            // result.
            int overflow_bits_count = 1;
            while (overflow > 1) {
                overflow_bits_count++;
                overflow >>= 1;
            }

            int dropped_bits_mask = ((1 << overflow_bits_count) - 1);
            int dropped_bits = static_cast<int>(number) & dropped_bits_mask;
            number >>= overflow_bits_count;
            exponent = overflow_bits_count;

            bool zero_tail = true;
            for (;;) {
                ++(*current);
                if (*current == end || !is_digit(**current, radix)) {
                    break;
                }
                zero_tail = zero_tail && **current == '0';
                exponent += radix_log_2;
            }

            if (!allow_trailing_junk && advance_to_nonspace(current, end)) {
                return junk_string_value;
            }

            int middle_value = (1 << (overflow_bits_count - 1));
            if (dropped_bits > middle_value) {
                number++;  // Rounding up.
            } else if (dropped_bits == middle_value) {
                // Rounding to even to consistency with decimals: half-way case rounds
                // up if significant part is odd and down otherwise.
                if ((number & 1) != 0 || !zero_tail) {
                    number++;  // Rounding up.
                }
            }

            // Rounding up may cause overflow.
            if ((number & ((int64_t)1 << kSignificandSize)) != 0) {
                exponent++;
                number >>= 1;
            }
            break;
        }
        ++(*current);
    } while (*current != end);

    WALLE_ASSERT(number < ((int64_t)1 << kSignificandSize), "");
    WALLE_ASSERT(static_cast<int64_t>(static_cast<double>(number)) == number, "");

    *result_is_junk = false;

    if (exponent == 0) {
        if (sign) {
            if (number == 0) {
                return -0.0;
            }
            number = -number;
        }
        return static_cast<double>(number);
    }

    WALLE_ASSERT(number != 0, "");
    return Double(SimpleFp(number, exponent)).value();
}


template <class Iterator>
double StringToDoubleConverter::string_to_ieee(
    Iterator input,
    int length,
    bool read_as_double,
    int* processed_characters_count) const 
{
    Iterator current = input;
    Iterator end = input + length;

    *processed_characters_count = 0;

    const bool allow_trailing_junk = (_flags & eAllowTrailingJunk) != 0;
    const bool allow_leading_spaces = (_flags & eAllowLeadingSpace) != 0;
    const bool allow_trailing_spaces = (_flags & eAllowTrailingSpace) != 0;
    const bool allow_spaces_after_sign = (_flags & eAllowSpaceAfterSign) != 0;

    // To make sure that iterator dereferencing is valid the following
    // convention is used:
    // 1. Each '++current' statement is followed by check for equality to 'end'.
    // 2. If advance_to_nonspace returned false then current == end.
    // 3. If 'current' becomes equal to 'end' the function returns or goes to
    // 'parsing_done'.
    // 4. 'current' is not dereferenced after the 'parsing_done' label.
    // 5. Code before 'parsing_done' may rely on 'current != end'.
    if (current == end) {
        return _empty_string_value;
    }

    if (allow_leading_spaces || allow_trailing_spaces) {
        if (!advance_to_nonspace(&current, end)) {
            *processed_characters_count = static_cast<int>(current - input);
            return _empty_string_value;
        }
        if (!allow_leading_spaces && (input != current)) {
            // No leading spaces allowed, but advance_to_nonspace moved forward.
            return _junk_string_value;
        }
    }

    // The longest form of simplified number is: "-<significant digits>.1eXXX\0".
    const int kBufferSize = kMaxSignificantDigits + 10;
    char buffer[kBufferSize];  // NOLINT: size is known at compile time.
    int buffer_pos = 0;

    // Exponent will be adjusted if insignificant digits of the integer part
    // or insignificant leading zeros of the fractional part are dropped.
    int exponent = 0;
    int significant_digits = 0;
    int insignificant_digits = 0;
    bool nonzero_digit_dropped = false;

    bool sign = false;

    if (*current == '+' || *current == '-') {
        sign = (*current == '-');
        ++current;
        Iterator next_non_space = current;
        // Skip following spaces (if allowed).
        if (!advance_to_nonspace(&next_non_space, end)) {
            return _junk_string_value;
        }
        if (!allow_spaces_after_sign && (current != next_non_space)) {
            return _junk_string_value;
        }
        current = next_non_space;
    }

    if (_infinity_symbol != NULL) {
        if (*current == _infinity_symbol[0]) {
            if (!consume_sub_string(&current, end, _infinity_symbol)) {
                return _junk_string_value;
            }

            if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
                return _junk_string_value;
            }
            if (!allow_trailing_junk && advance_to_nonspace(&current, end)) {
                return _junk_string_value;
            }

            WALLE_ASSERT(buffer_pos == 0, "");
            *processed_characters_count = static_cast<int>(current - input);
            return sign ? -Double::infinity() : Double::infinity();
        }
    }

    if (_nan_symbol != NULL) {
        if (*current == _nan_symbol[0]) {
            if (!consume_sub_string(&current, end, _nan_symbol)) {
                return _junk_string_value;
            }

            if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
                return _junk_string_value;
            }
            if (!allow_trailing_junk && advance_to_nonspace(&current, end)) {
                return _junk_string_value;
            }

            WALLE_ASSERT(buffer_pos == 0, "");
            *processed_characters_count = static_cast<int>(current - input);
            return sign ? -Double::nan() : Double::nan();
        }
    }

    bool leading_zero = false;
    if (*current == '0') {
        ++current;
        if (current == end) {
            *processed_characters_count = static_cast<int>(current - input);
            return signed_zero(sign);
        }

        leading_zero = true;

        // It could be hexadecimal value.
        if ((_flags & eAllowHex) && (*current == 'x' || *current == 'X')) {
            ++current;
            if (current == end || !is_digit(*current, 16)) {
                return _junk_string_value;  // "0x".
            }

            bool result_is_junk;
            double result = radix_string_to_ieee<4>(&current,
                                            end,
                                            sign,
                                            allow_trailing_junk,
                                            _junk_string_value,
                                            read_as_double,
                                            &result_is_junk);
            if (!result_is_junk) {
                if (allow_trailing_spaces) advance_to_nonspace(&current, end);
                    *processed_characters_count = static_cast<int>(current - input);
            }
            return result;
        }

        // Ignore leading zeros in the integer part.
        while (*current == '0') {
            ++current;
            if (current == end) {
                *processed_characters_count = static_cast<int>(current - input);
                return signed_zero(sign);
            }
        }
    }

    bool octal = leading_zero && (_flags & eAllowOctals) != 0;

    // Copy significant digits of the integer part (if any) to the buffer.
    while (*current >= '0' && *current <= '9') {
        if (significant_digits < kMaxSignificantDigits) {
            WALLE_ASSERT(buffer_pos < kBufferSize, "");
            buffer[buffer_pos++] = static_cast<char>(*current);
            significant_digits++;
            // Will later check if it's an octal in the buffer.
        } else {
            insignificant_digits++;  // Move the digit into the exponential part.
            nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
        }
        octal = octal && *current < '8';
        ++current;
        if (current == end) goto parsing_done;
    }

    if (significant_digits == 0) {
        octal = false;
    }

    if (*current == '.') {
        if (octal && !allow_trailing_junk) {
            return _junk_string_value;
        }
        if (octal) {
            goto parsing_done;
        }

        ++current;
        if (current == end) {
            if (significant_digits == 0 && !leading_zero) {
                return _junk_string_value;
            } else {
                goto parsing_done;
            }
        }

        if (significant_digits == 0) {
            // octal = false;
            // Integer part consists of 0 or is absent. Significant digits start after
            // leading zeros (if any).
            while (*current == '0') {
                ++current;
                if (current == end) {
                    *processed_characters_count = static_cast<int>(current - input);
                    return signed_zero(sign);
                }
                exponent--;  // Move this 0 into the exponent.
            }
        }

        // There is a fractional part.
        // We don't emit a '.', but adjust the exponent instead.
        while (*current >= '0' && *current <= '9') {
            if (significant_digits < kMaxSignificantDigits) {
                WALLE_ASSERT(buffer_pos < kBufferSize, "");
                buffer[buffer_pos++] = static_cast<char>(*current);
                significant_digits++;
                exponent--;
            } else {
                // Ignore insignificant digits in the fractional part.
                nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
            }
            ++current;
            if (current == end) {
                goto parsing_done;
            }
        }
    }

    if (!leading_zero && exponent == 0 && significant_digits == 0) {
        // If leading_zeros is true then the string contains zeros.
        // If exponent < 0 then string was [+-]\.0*...
        // If significant_digits != 0 the string is not equal to 0.
        // Otherwise there are no digits in the string.
        return _junk_string_value;
    }

    // Parse exponential part.
    if (*current == 'e' || *current == 'E') {
        if (octal && !allow_trailing_junk) {
            return _junk_string_value;
        }
        if (octal) {
            goto parsing_done;
        }
        ++current;
        if (current == end) {
            if (allow_trailing_junk) {
                goto parsing_done;
            } else {
                return _junk_string_value;
            }
        }
        char exponen_sign = '+';
        if (*current == '+' || *current == '-') {
            exponen_sign = static_cast<char>(*current);
            ++current;
            if (current == end) {
                if (allow_trailing_junk) {
                    goto parsing_done;
                } else {
                    return _junk_string_value;
                }
            }
        }

        if (current == end || *current < '0' || *current > '9') {
            if (allow_trailing_junk) {
                goto parsing_done;
            } else {
                return _junk_string_value;
            }
        }

        const int max_exponent = INT_MAX / 2;
        WALLE_ASSERT(-max_exponent / 2 <= exponent && exponent <= max_exponent / 2, "");
        int num = 0;
        do {
            // Check overflow.
            int digit = *current - '0';
            if (num >= max_exponent / 10
                    && !(num == max_exponent / 10 && digit <= max_exponent % 10)) {
                num = max_exponent;
            } else {
                num = num * 10 + digit;
            }
            ++current;
        } while (current != end && *current >= '0' && *current <= '9');

        exponent += (exponen_sign == '-' ? -num : num);
    }

    if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
        return _junk_string_value;
    }
    if (!allow_trailing_junk && advance_to_nonspace(&current, end)) {
        return _junk_string_value;
    }
    if (allow_trailing_spaces) {
        advance_to_nonspace(&current, end);
    }

parsing_done:
    exponent += insignificant_digits;

    if (octal) {
        double result;
        bool result_is_junk;
        char* start = buffer;
        result = radix_string_to_ieee<3>(&start,
                                buffer + buffer_pos,
                                sign,
                                allow_trailing_junk,
                                _junk_string_value,
                                read_as_double,
                                &result_is_junk);
        WALLE_ASSERT(!result_is_junk, "");
        *processed_characters_count = static_cast<int>(current - input);
        return result;
    }

    if (nonzero_digit_dropped) {
        buffer[buffer_pos++] = '1';
        exponent--;
    }

    WALLE_ASSERT(buffer_pos < kBufferSize, "");
    buffer[buffer_pos] = '\0';

    double converted;
    if (read_as_double) {
        converted = str_to_double(walle::BufferRef<const char>(buffer, buffer_pos), exponent);
    } else {
        converted = str_to_float(walle::BufferRef<const char>(buffer, buffer_pos), exponent);
    }
    *processed_characters_count = static_cast<int>(current - input);
    return sign? -converted: converted;
}


double StringToDoubleConverter::string_to_double(
    const char* buffer,
    int length,
    int* processed_characters_count) const 
{
    return string_to_ieee(buffer, length, true, processed_characters_count);
}


double StringToDoubleConverter::string_to_double(
    const uint16_t* buffer,
    int length,
    int* processed_characters_count) const 
{
    return string_to_ieee(buffer, length, true, processed_characters_count);
}


float StringToDoubleConverter::string_to_float(
    const char* buffer,
    int length,
    int* processed_characters_count) const 
{
    return static_cast<float>(string_to_ieee(buffer, length, false,
                                     processed_characters_count));
}


float StringToDoubleConverter::string_to_float(
    const uint16_t* buffer,
    int length,
    int* processed_characters_count) const 
{
    return static_cast<float>(string_to_ieee(buffer, length, false,
                                         processed_characters_count));
}

} //namespace format_detail
} //namespace walle
