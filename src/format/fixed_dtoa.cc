#include <walle/format/detail/fixed_dtoa.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/math/math.h>

namespace walle {
namespace format_detail {
/**
 * @brief Represents a 128bit type. This class should be replaced by a native type on
 * platforms that support 128bit integers.
 */

class UInt128 {
public:
    UInt128() : _high_bits(0), _low_bits(0) { }
    UInt128(uint64_t high, uint64_t low) : _high_bits(high), _low_bits(low) { }

    void multiply(uint32_t multiplicand) 
    {
        uint64_t accumulator;

        accumulator = (_low_bits & kMask32) * multiplicand;
        uint32_t part = static_cast<uint32_t>(accumulator & kMask32);
        accumulator >>= 32;
        accumulator = accumulator + (_low_bits >> 32) * multiplicand;
        _low_bits = (accumulator << 32) + part;
        accumulator >>= 32;
        accumulator = accumulator + (_high_bits & kMask32) * multiplicand;
        part = static_cast<uint32_t>(accumulator & kMask32);
        accumulator >>= 32;
        accumulator = accumulator + (_high_bits >> 32) * multiplicand;
        _high_bits = (accumulator << 32) + part;
        WALLE_ASSERT((accumulator >> 32) == 0, "");
    }

    void shift(int shift_amount) 
    {
        WALLE_ASSERT(-64 <= shift_amount && shift_amount <= 64, "");
        if (shift_amount == 0) {
            return;
        } else if (shift_amount == -64) {
            _high_bits = _low_bits;
            _low_bits = 0;
        } else if (shift_amount == 64) {
            _low_bits = _high_bits;
            _high_bits = 0;
        } else if (shift_amount <= 0) {
            _high_bits <<= -shift_amount;
            _high_bits += _low_bits >> (64 + shift_amount);
            _low_bits <<= -shift_amount;
        } else {
            _low_bits >>= shift_amount;
            _low_bits += _high_bits << (64 - shift_amount);
            _high_bits >>= shift_amount;
        }
    }

    // Modifies *this to *this MOD (2^power).
    // Returns *this DIV (2^power).
    int div_mod_power_of_2(int power) 
    {
        if (power >= 64) {
            int result = static_cast<int>(_high_bits >> (power - 64));
            _high_bits -= static_cast<uint64_t>(result) << (power - 64);
            return result;
        } else {
            uint64_t part_low = _low_bits >> power;
            uint64_t part_high = _high_bits << (64 - power);
            int result = static_cast<int>(part_low + part_high);
            _high_bits = 0;
            _low_bits -= part_low << power;
            return result;
        }
    }

    bool is_zero() const 
    {
        return _high_bits == 0 && _low_bits == 0;
    }

    int bit_at(int position) const 
    {
        if (position >= 64) {
            return static_cast<int>(_high_bits >> (position - 64)) & 1;
        } else {
            return static_cast<int>(_low_bits >> position) & 1;
        }
    }

private:
    static const uint64_t kMask32 = 0xFFFFFFFF;
    // Value == (high_bits_ << 64) + low_bits_
    uint64_t _high_bits;
    uint64_t _low_bits;
};

static const int kDoubleSignificandSize = 53;  // Includes the hidden bit.

static void fill_digits32_fixed_length(uint32_t number, int requested_length,
                                    walle::BufferRef<char> &buffer, int* length) 
{
    for (int i = requested_length - 1; i >= 0; --i) {
        buffer[(*length) + i] = '0' + number % 10;
        number /= 10;
    }
    *length += requested_length;
}


static void fill_digits32(uint32_t number, walle::BufferRef<char> buffer, int* length) 
{
    int number_length = 0;
    // We fill the digits in reverse order and exchange them afterwards.
    while (number != 0) {
        int digit = number % 10;
        number /= 10;
        buffer[(*length) + number_length] = static_cast<char>('0' + digit);
        number_length++;
    }
    // Exchange the digits.
    int i = *length;
    int j = *length + number_length - 1;
    while (i < j) {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
        i++;
        j--;
    }
    *length += number_length;
}


static void fill_digits64_fixed_length(uint64_t number,
                                    walle::BufferRef<char> buffer, int* length) 
{
    const uint32_t kTen7 = 10000000;
    // For efficiency cut the number into 3 uint32_t parts, and print those.
    uint32_t part2 = static_cast<uint32_t>(number % kTen7);
    number /= kTen7;
    uint32_t part1 = static_cast<uint32_t>(number % kTen7);
    uint32_t part0 = static_cast<uint32_t>(number / kTen7);

  fill_digits32_fixed_length(part0, 3, buffer, length);
  fill_digits32_fixed_length(part1, 7, buffer, length);
  fill_digits32_fixed_length(part2, 7, buffer, length);
}


static void fill_digits64(uint64_t number, walle::BufferRef<char> buffer, int* length) 
{
    const uint32_t kTen7 = 10000000;
    // For efficiency cut the number into 3 uint32_t parts, and print those.
    uint32_t part2 = static_cast<uint32_t>(number % kTen7);
    number /= kTen7;
    uint32_t part1 = static_cast<uint32_t>(number % kTen7);
    uint32_t part0 = static_cast<uint32_t>(number / kTen7);

    if (part0 != 0) {
        fill_digits32(part0, buffer, length);
        fill_digits32_fixed_length(part1, 7, buffer, length);
        fill_digits32_fixed_length(part2, 7, buffer, length);
    } else if (part1 != 0) {
        fill_digits32(part1, buffer, length);
        fill_digits32_fixed_length(part2, 7, buffer, length);
    } else {
        fill_digits32(part2, buffer, length);
    }
}


static void round_up(walle::BufferRef<char> buffer, int* length, int* decimal_point) 
{
    // An empty buffer represents 0.
    if (*length == 0) {
        buffer[0] = '1';
        *decimal_point = 1;
        *length = 1;
        return;
    }
    // Round the last digit until we either have a digit that was not '9' or until
    // we reached the first digit.
    buffer[(*length) - 1]++;
    for (int i = (*length) - 1; i > 0; --i) {
        if (buffer[i] != '0' + 10) {
            return;
        }
        buffer[i] = '0';
        buffer[i - 1]++;
    }
    // If the first digit is now '0' + 10, we would need to set it to '0' and add
    // a '1' in front. However we reach the first digit only if all following
    // digits had been '9' before rounding up. Now all trailing digits are '0' and
    // we simply switch the first digit to '1' and update the decimal-point
    // (indicating that the point is now one digit to the right).
    if (buffer[0] == '0' + 10) {
        buffer[0] = '1';
        (*decimal_point)++;
    }
}


// The given fractionals number represents a fixed-point number with binary
// point at bit (-exponent).
// Preconditions:
//   -128 <= exponent <= 0.
//   0 <= fractionals * 2^exponent < 1
//   The buffer holds the result.
// The function will round its result. During the rounding-process digits not
// generated by this function might be updated, and the decimal-point variable
// might be updated. If this function generates the digits 99 and the buffer
// already contained "199" (thus yielding a buffer of "19999") then a
// rounding-up will change the contents of the buffer to "20000".
static void fill_fractionals(uint64_t fractionals, int exponent,
                            int fractional_count, walle::BufferRef<char> buffer,
                            int* length, int* decimal_point) 
{
    WALLE_ASSERT(-128 <= exponent && exponent <= 0, "");
    // 'fractionals' is a fixed-point number, with binary point at bit
    // (-exponent). Inside the function the non-converted remainder of fractionals
    // is a fixed-point number, with binary point at bit 'point'.
    if (-exponent <= 64) {
        // One 64 bit number is sufficient.
        WALLE_ASSERT(fractionals >> 56 == 0, "");
        int point = -exponent;
        for (int i = 0; i < fractional_count; ++i) {
            if (fractionals == 0) break;
            // Instead of multiplying by 10 we multiply by 5 and adjust the point
            // location. This way the fractionals variable will not overflow.
            // Invariant at the beginning of the loop: fractionals < 2^point.
            // Initially we have: point <= 64 and fractionals < 2^56
            // After each iteration the point is decremented by one.
            // Note that 5^3 = 125 < 128 = 2^7.
            // Therefore three iterations of this loop will not overflow fractionals
            // (even without the subtraction at the end of the loop body). At this
            // time point will satisfy point <= 61 and therefore fractionals < 2^point
            // and any further multiplication of fractionals by 5 will not overflow.
            fractionals *= 5;
            point--;
            int digit = static_cast<int>(fractionals >> point);
            WALLE_ASSERT(digit <= 9, "");
            buffer[*length] = static_cast<char>('0' + digit);
            (*length)++;
            fractionals -= static_cast<uint64_t>(digit) << point;
        }
        // If the first bit after the point is set we have to round up.
        if (((fractionals >> (point - 1)) & 1) == 1) {
            round_up(buffer, length, decimal_point);
        }
    } else {  // We need 128 bits.
        WALLE_ASSERT(64 < -exponent && -exponent <= 128, "");
        UInt128 fractionals128 = UInt128(fractionals, 0);
        fractionals128.shift(-exponent - 64);
        int point = 128;
        for (int i = 0; i < fractional_count; ++i) {
            if (fractionals128.is_zero()) {
                break;
            }
            // As before: instead of multiplying by 10 we multiply by 5 and adjust the
            // point location.
            // This multiplication will not overflow for the same reasons as before.
            fractionals128.multiply(5);
            point--;
            int digit = fractionals128.div_mod_power_of_2(point);
            WALLE_ASSERT(digit <= 9, "");
            buffer[*length] = static_cast<char>('0' + digit);
            (*length)++;
        }
        if (fractionals128.bit_at(point - 1) == 1) {
            round_up(buffer, length, decimal_point);
        }
    }
}


// Removes leading and trailing zeros.
// If leading zeros are removed then the decimal point position is adjusted.
static void trim_zeros(walle::BufferRef<char> buffer, int* length, int* decimal_point) 
{
    while (*length > 0 && buffer[(*length) - 1] == '0') {
        (*length)--;
    }
      int first_non_zero = 0;
    while (first_non_zero < *length && buffer[first_non_zero] == '0') {
        first_non_zero++;
    }
    if (first_non_zero != 0) {
        for (int i = first_non_zero; i < *length; ++i) {
            buffer[i - first_non_zero] = buffer[i];
        }
        *length -= first_non_zero;
        *decimal_point -= first_non_zero;
    }
}


bool fast_fixed_dtoa(double v,
                   int fractional_count,
                   walle::BufferRef<char> buffer,
                   int* length,
                   int* decimal_point) 
{
    const uint32_t kMaxUInt32 = 0xFFFFFFFF;
    uint64_t significand = Double(v).significand();
    int exponent = Double(v).exponent();
    // v = significand * 2^exponent (with significand a 53bit integer).
    // If the exponent is larger than 20 (i.e. we may have a 73bit number) then we
    // don't know how to compute the representation. 2^73 ~= 9.5*10^21.
    // If necessary this limit could probably be increased, but we don't need
    // more.
    if (exponent > 20) {
        return false;
    }
    if (fractional_count > 20) {
        return false;
    }
    *length = 0;
    // At most kDoubleSignificandSize bits of the significand are non-zero.
    // Given a 64 bit integer we have 11 0s followed by 53 potentially non-zero
    // bits:  0..11*..0xxx..53*..xx
    if (exponent + kDoubleSignificandSize > 64) {
        // The exponent must be > 11.
        //
        // We know that v = significand * 2^exponent.
        // And the exponent > 11.
        // We simplify the task by dividing v by 10^17.
        // The quotient delivers the first digits, and the remainder fits into a 64
        // bit number.
        // Dividing by 10^17 is equivalent to dividing by 5^17*2^17.
        const uint64_t kFive17 = WALLE_UINT64_2PART_C(0xB1, A2BC2EC5);  // 5^17
        uint64_t divisor = kFive17;
        int divisor_power = 17;
        uint64_t dividend = significand;
        uint32_t quotient;
        uint64_t remainder;
        // Let v = f * 2^e with f == significand and e == exponent.
        // Then need q (quotient) and r (remainder) as follows:
        //   v            = q * 10^17       + r
        //   f * 2^e      = q * 10^17       + r
        //   f * 2^e      = q * 5^17 * 2^17 + r
        // If e > 17 then
        //   f * 2^(e-17) = q * 5^17        + r/2^17
        // else
        //   f  = q * 5^17 * 2^(17-e) + r/2^e
        if (exponent > divisor_power) {
            // We only allow exponents of up to 20 and therefore (17 - e) <= 3
            dividend <<= exponent - divisor_power;
            quotient = static_cast<uint32_t>(dividend / divisor);
            remainder = (dividend % divisor) << divisor_power;
        } else {
            divisor <<= divisor_power - exponent;
            quotient = static_cast<uint32_t>(dividend / divisor);
            remainder = (dividend % divisor) << exponent;
        }
        fill_digits32(quotient, buffer, length);
        fill_digits64_fixed_length(remainder, buffer, length);
        *decimal_point = *length;
    } else if (exponent >= 0) {
        // 0 <= exponent <= 11
        significand <<= exponent;
        fill_digits64(significand, buffer, length);
        *decimal_point = *length;
    } else if (exponent > -kDoubleSignificandSize) {
        // We have to cut the number.
        uint64_t integrals = significand >> -exponent;
        uint64_t fractionals = significand - (integrals << -exponent);
        if (integrals > kMaxUInt32) {
            fill_digits64(integrals, buffer, length);
        } else {
            fill_digits32(static_cast<uint32_t>(integrals), buffer, length);
        }
        *decimal_point = *length;
        fill_fractionals(fractionals, exponent, fractional_count,
                    buffer, length, decimal_point);
    } else if (exponent < -128) {
        // This configuration (with at most 20 digits) means that all digits must be
        // 0.
        WALLE_ASSERT(fractional_count <= 20, "");
        buffer[0] = '\0';
        *length = 0;
        *decimal_point = -fractional_count;
    } else {
        *decimal_point = 0;
        fill_fractionals(significand, exponent, fractional_count,
                    buffer, length, decimal_point);
    }
    trim_zeros(buffer, length, decimal_point);
    buffer[*length] = '\0';
    if ((*length) == 0) {
        // The string is empty and the decimal_point thus has no importance. Mimick
        // Gay's dtoa and and set it to -fractional_count.
        *decimal_point = -fractional_count;
    }
    return true;
}

} //namespace format_detail
} //namespace walle