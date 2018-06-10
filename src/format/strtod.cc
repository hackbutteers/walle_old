#include <walle/format/detail/strtod.h>
#include <walle/format/detail/bignum.h>
#include <walle/format/detail/cached_powers.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <stdarg.h>
#include <limits.h>

#include <iostream>

#if WALLE_COMPILER_CLANG
    WALLE_DISABLE_CLANG_WARNING(-Wsign-compare)
#endif

#if WALLE_COMPILER_GNUC
    WALLE_DISABLE_GCC_WARNING(-Wsign-compare)
#endif

namespace walle {
namespace format_detail {

// 2^53 = 9007199254740992.
// Any integer with at most 15 decimal digits will hence fit into a double
// (which has a 53bit significand) without loss of precision.
static const int kMaxExactDoubleIntegerDecimalDigits = 15;
// 2^64 = 18446744073709551616 > 10^19
static const int kMaxUint64DecimalDigits = 19;

// Max double: 1.7976931348623157 x 10^308
// Min non-zero double: 4.9406564584124654 x 10^-324
// Any x >= 10^309 is interpreted as +infinity.
// Any x <= 10^-324 is interpreted as 0.
// Note that 2.5e-324 (despite being smaller than the min double) will be read
// as non-zero (equal to the min non-zero double).
static const int kMaxDecimalPower = 309;
static const int kMinDecimalPower = -324;

// 2^64 = 18446744073709551616
static const uint64_t kMaxUint64 = WALLE_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF);


static const double exact_powers_of_ten[] = {
    1.0,  // 10^0
    10.0,
    100.0,
    1000.0,
    10000.0,
    100000.0,
    1000000.0,
    10000000.0,
    100000000.0,
    1000000000.0,
    10000000000.0,  // 10^10
    100000000000.0,
    1000000000000.0,
    10000000000000.0,
    100000000000000.0,
    1000000000000000.0,
    10000000000000000.0,
    100000000000000000.0,
    1000000000000000000.0,
    10000000000000000000.0,
    100000000000000000000.0,  // 10^20
    1000000000000000000000.0,
    // 10^22 = 0x21e19e0c9bab2400000 = 0x878678326eac9 * 2^22
    10000000000000000000000.0
};
static const int kExactPowersOfTenSize = WALLE_ARRAY_SIZE(exact_powers_of_ten);

// Maximum number of significant digits in the decimal representation.
// In fact the value is 772 (see conversions.cc), but to give us some margin
// we round up to 780.
static const int kMaxSignificantDecimalDigits = 780;

static walle::BufferRef<const char> trim_leading_zeros(walle::BufferRef<const char> buffer) 
{
    for (size_t i = 0; i < buffer.size(); i++) {
        if (buffer[i] != '0') {
            return buffer.sub_buffer_ref(i, buffer.size());
        }
    }
    return walle::BufferRef<const char>(buffer.data(), 0);
}


static walle::BufferRef<const char> trim_trailing_zeros(walle::BufferRef<const char> buffer) 
{
    for (int i = buffer.size() - 1; i >= 0; --i) {
        if (buffer[i] != '0') {
        return buffer.sub_buffer_ref(0, i + 1);
        }
    }
    return walle::BufferRef<const char>(buffer.data(), 0);
}


static void cut_to_max_significant_digits(walle::BufferRef<const char> buffer,
                                       int exponent,
                                       char* significant_buffer,
                                       int* significant_exponent) 
{
    for (int i = 0; i < kMaxSignificantDecimalDigits - 1; ++i) {
        significant_buffer[i] = buffer[i];
    }
    // The input buffer has been trimmed. Therefore the last digit must be
    // different from '0'.
    WALLE_ASSERT(buffer[buffer.size() - 1] != '0', "");
    // Set the last digit to be non-zero. This is sufficient to guarantee
    // correct rounding.
    significant_buffer[kMaxSignificantDecimalDigits - 1] = '1';
    *significant_exponent =
        exponent + (buffer.size() - kMaxSignificantDecimalDigits);
}


// Trims the buffer and cuts it to at most kMaxSignificantDecimalDigits.
// If possible the input-buffer is reused, but if the buffer needs to be
// modified (due to cutting), then the input needs to be copied into the
// buffer_copy_space.
static void trim_and_cut(walle::BufferRef<const char> buffer, int exponent,
                       char* buffer_copy_space, int space_size,
                       walle::BufferRef<const char>* trimmed, int* updated_exponent) 
{
    walle::BufferRef<const char> left_trimmed = trim_leading_zeros(buffer);
    walle::BufferRef<const char> right_trimmed = trim_trailing_zeros(left_trimmed);
    exponent += left_trimmed.size() - right_trimmed.size();
    if (right_trimmed.size() > kMaxSignificantDecimalDigits) {
        WALLE_UNUSED(space_size);  // Mark variable as used.
        WALLE_ASSERT(space_size >= kMaxSignificantDecimalDigits, "");
        cut_to_max_significant_digits(right_trimmed, exponent,
                                buffer_copy_space, updated_exponent);
        *trimmed = walle::BufferRef<const char>(buffer_copy_space,
                                    kMaxSignificantDecimalDigits);
    } else {
        *trimmed = right_trimmed;
        *updated_exponent = exponent;
    }
}


// Reads digits from the buffer and converts them to a uint64.
// Reads in as many digits as fit into a uint64.
// When the string starts with "1844674407370955161" no further digit is read.
// Since 2^64 = 18446744073709551616 it would still be possible read another
// digit if it was less or equal than 6, but this would complicate the code.
static uint64_t read_uint64(walle::BufferRef<const char> buffer,
                           size_t* number_of_read_digits) 
{
    uint64_t result = 0;
    size_t i = 0;
    while (i < buffer.size() && result <= (kMaxUint64 / 10 - 1)) {
        int digit = buffer[i++] - '0';
        WALLE_ASSERT(0 <= digit && digit <= 9, "");
        result = 10 * result + digit;
    }
    *number_of_read_digits = i;
    return result;
}


// Reads a DiyFp from the buffer.
// The returned DiyFp is not necessarily normalized.
// If remaining_decimals is zero then the returned DiyFp is accurate.
// Otherwise it has been rounded and has error of at most 1/2 ulp.
static void read_simple_fp(walle::BufferRef<const char> buffer,
                      SimpleFp* result,
                      int* remaining_decimals) 
{
    size_t read_digits;
    uint64_t significand = read_uint64(buffer, &read_digits);
    if (buffer.size() == read_digits) {
        *result = SimpleFp(significand, 0);
        *remaining_decimals = 0;
    } else {
        // Round the significand.
        if (buffer[read_digits] >= '5') {
            significand++;
        }
        // Compute the binary exponent.
        int exponent = 0;
        *result = SimpleFp(significand, exponent);
        *remaining_decimals = buffer.size() - read_digits;
    }
}


static bool double_strtod(walle::BufferRef<const char> trimmed,
                         int exponent,
                         double* result) 
{
#if !defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
    // On x86 the floating-point stack can be 64 or 80 bits wide. If it is
    // 80 bits wide (as is the case on Linux) then double-rounding occurs and the
    // result is not accurate.
    // We know that Windows32 uses 64 bits and is therefore accurate.
    // Note that the ARM simulator is compiled for 32bits. It therefore exhibits
    // the same problem.
    return false;
#endif
    if (trimmed.size() <= kMaxExactDoubleIntegerDecimalDigits) {
        size_t read_digits;
        // The trimmed input fits into a double.
        // If the 10^exponent (resp. 10^-exponent) fits into a double too then we
        // can compute the result-double simply by multiplying (resp. dividing) the
        // two numbers.
        // This is possible because IEEE guarantees that floating-point operations
        // return the best possible approximation.
        if (exponent < 0 && -exponent < kExactPowersOfTenSize) {
        // 10^-exponent fits into a double.
            *result = static_cast<double>(read_uint64(trimmed, &read_digits));
            WALLE_ASSERT(read_digits == trimmed.size(), "");
            *result /= exact_powers_of_ten[-exponent];
            return true;
        }
        if (0 <= exponent && exponent < kExactPowersOfTenSize) {
            // 10^exponent fits into a double.
            *result = static_cast<double>(read_uint64(trimmed, &read_digits));
            WALLE_ASSERT(read_digits == trimmed.size(),"");
            *result *= exact_powers_of_ten[exponent];
            return true;
        }
        int remaining_digits =
            kMaxExactDoubleIntegerDecimalDigits - trimmed.size();
        if ((0 <= exponent) &&
            (exponent - remaining_digits < kExactPowersOfTenSize)) {
        // The trimmed string was short and we can multiply it with
        // 10^remaining_digits. As a result the remaining exponent now fits
        // into a double too.
            *result = static_cast<double>(read_uint64(trimmed, &read_digits));
            WALLE_ASSERT(read_digits == trimmed.size(), "");
            *result *= exact_powers_of_ten[remaining_digits];
            *result *= exact_powers_of_ten[exponent - remaining_digits];
            return true;
        }
    }
    return false;
}


// Returns 10^exponent as an exact DiyFp.
// The given exponent must be in the range [1; kDecimalExponentDistance[.
static SimpleFp adjustment_power_of_ten(int exponent) 
{
    WALLE_ASSERT(0 < exponent, "");
    WALLE_ASSERT(exponent < PowersOfTenCache::kDecimalExponentDistance, "");
    // Simply hardcode the remaining powers for the given decimal exponent
    // distance.
    WALLE_ASSERT(PowersOfTenCache::kDecimalExponentDistance == 8, "");
    switch (exponent) {
        case 1: return SimpleFp(WALLE_UINT64_2PART_C(0xa0000000, 00000000), -60);
        case 2: return SimpleFp(WALLE_UINT64_2PART_C(0xc8000000, 00000000), -57);
        case 3: return SimpleFp(WALLE_UINT64_2PART_C(0xfa000000, 00000000), -54);
        case 4: return SimpleFp(WALLE_UINT64_2PART_C(0x9c400000, 00000000), -50);
        case 5: return SimpleFp(WALLE_UINT64_2PART_C(0xc3500000, 00000000), -47);
        case 6: return SimpleFp(WALLE_UINT64_2PART_C(0xf4240000, 00000000), -44);
        case 7: return SimpleFp(WALLE_UINT64_2PART_C(0x98968000, 00000000), -40);
        default:WALLE_ASSERT(false, "");
    }
}


// If the function returns true then the result is the correct double.
// Otherwise it is either the correct double or the double that is just below
// the correct double.
static bool simple_fp_strtod(walle::BufferRef<const char> buffer,
                        int exponent,
                        double* result) 
{
    SimpleFp input;
    int remaining_decimals;
    read_simple_fp(buffer, &input, &remaining_decimals);
    // Since we may have dropped some digits the input is not accurate.
    // If remaining_decimals is different than 0 than the error is at most
    // .5 ulp (unit in the last place).
    // We don't want to deal with fractions and therefore keep a common
    // denominator.
    const int kDenominatorLog = 3;
    const int kDenominator = 1 << kDenominatorLog;
    // Move the remaining decimals into the exponent.
    exponent += remaining_decimals;
    uint64_t error = (remaining_decimals == 0 ? 0 : kDenominator / 2);

    int old_e = input.e();
    input.normalize();
    error <<= old_e - input.e();

    WALLE_ASSERT(exponent <= PowersOfTenCache::kMaxDecimalExponent, "");
    if (exponent < PowersOfTenCache::kMinDecimalExponent) {
        *result = 0.0;
        return true;
    }
    SimpleFp cached_power;
    int cached_decimal_exponent;
    PowersOfTenCache::get_cached_power_for_decimal_exponent(exponent,
                                                     &cached_power,
                                                     &cached_decimal_exponent);

    if (cached_decimal_exponent != exponent) {
        int adjustment_exponent = exponent - cached_decimal_exponent;
        SimpleFp adjustment_power = adjustment_power_of_ten(adjustment_exponent);
        input.multiply(adjustment_power);
        if (static_cast<size_t>(kMaxUint64DecimalDigits) - buffer.size() >= static_cast<size_t>(adjustment_exponent)) {
        // The product of input with the adjustment power fits into a 64 bit
        // integer.
            WALLE_ASSERT(SimpleFp::kSignificandSize == 64, "");
        } else {
        // The adjustment power is exact. There is hence only an error of 0.5.
            error += kDenominator / 2;
        }
    }

    input.multiply(cached_power);
    // The error introduced by a multiplication of a*b equals
    //   error_a + error_b + error_a*error_b/2^64 + 0.5
    // Substituting a with 'input' and b with 'cached_power' we have
    //   error_b = 0.5  (all cached powers have an error of less than 0.5 ulp),
    //   error_ab = 0 or 1 / kDenominator > error_a*error_b/ 2^64
    int error_b = kDenominator / 2;
    int error_ab = (error == 0 ? 0 : 1);  // We round up to 1.
    int fixed_error = kDenominator / 2;
    error += error_b + error_ab + fixed_error;

    old_e = input.e();
    input.normalize();
    error <<= old_e - input.e();

    // See if the double's significand changes if we add/subtract the error.
    int order_of_magnitude = SimpleFp::kSignificandSize + input.e();
    int effective_significand_size =
          Double::significand_size_for_order_of_magnitude(order_of_magnitude);
    int precision_digits_count =
          SimpleFp::kSignificandSize - effective_significand_size;
    if (precision_digits_count + kDenominatorLog >= SimpleFp::kSignificandSize) {
        // This can only happen for very small denormals. In this case the
        // half-way multiplied by the denominator exceeds the range of an uint64.
        // Simply shift everything to the right.
        int shift_amount = (precision_digits_count + kDenominatorLog) -
            SimpleFp::kSignificandSize + 1;
        input.set_f(input.f() >> shift_amount);
        input.set_e(input.e() + shift_amount);
        // We add 1 for the lost precision of error, and kDenominator for
        // the lost precision of input.f().
        error = (error >> shift_amount) + 1 + kDenominator;
        precision_digits_count -= shift_amount;
    }
    // We use uint64_ts now. This only works if the SimpleFp uses uint64_ts too.
    WALLE_ASSERT(SimpleFp::kSignificandSize == 64, "");
    WALLE_ASSERT(precision_digits_count < 64, "");
    uint64_t one64 = 1;
    uint64_t precision_bits_mask = (one64 << precision_digits_count) - 1;
    uint64_t precision_bits = input.f() & precision_bits_mask;
    uint64_t half_way = one64 << (precision_digits_count - 1);
    precision_bits *= kDenominator;
    half_way *= kDenominator;
    SimpleFp rounded_input(input.f() >> precision_digits_count,
                      input.e() + precision_digits_count);
    if (precision_bits >= half_way + error) {
        rounded_input.set_f(rounded_input.f() + 1);
    }
    // If the last_bits are too close to the half-way case than we are too
    // inaccurate and round down. In this case we return false so that we can
    // fall back to a more precise algorithm.

    *result = Double(rounded_input).value();
    if (half_way - error < precision_bits && precision_bits < half_way + error) {
        // Too imprecise. The caller will have to fall back to a slower version.
        // However the returned number is guaranteed to be either the correct
        // double, or the next-lower double.
        return false;
    } else {
        return true;
    }
}


// Returns
//   - -1 if buffer*10^exponent < diy_fp.
//   -  0 if buffer*10^exponent == diy_fp.
//   - +1 if buffer*10^exponent > diy_fp.
// Preconditions:
//   buffer.length() + exponent <= kMaxDecimalPower + 1
//   buffer.length() + exponent > kMinDecimalPower
//   buffer.length() <= kMaxDecimalSignificantDigits
static int compare_buffer_with_simple_fp(walle::BufferRef<const char> buffer,
                                  int exponent,
                                  SimpleFp diy_fp) 
{
    int s = static_cast<int>(buffer.size()) + exponent;
    WALLE_ASSERT( s <= kMaxDecimalPower + 1, "");
    WALLE_ASSERT( s > kMinDecimalPower, "");
    WALLE_ASSERT(buffer.size() <= kMaxSignificantDecimalDigits, "");
    // Make sure that the Bignum will be able to hold all our numbers.
    // Our Bignum implementation has a separate field for exponents. Shifts will
    // consume at most one bigit (< 64 bits).
    // ln(10) == 3.3219...
    WALLE_ASSERT(((kMaxDecimalPower + 1) * 333 / 100) < Bignum::kMaxSignificantBits, "");
    Bignum buffer_bignum;
    Bignum diy_fp_bignum;
    buffer_bignum.assign_decimal_string(buffer);
    diy_fp_bignum.assign_uint64(diy_fp.f());
    if (exponent >= 0) {
        buffer_bignum.multi_by_power_of_ten(exponent);
    } else {
        diy_fp_bignum.multi_by_power_of_ten(-exponent);
    }
    if (diy_fp.e() > 0) {
        diy_fp_bignum.shift_left(diy_fp.e());
    } else {
        buffer_bignum.shift_left(-diy_fp.e());
    }
    return Bignum::compare(buffer_bignum, diy_fp_bignum);
}


// Returns true if the guess is the correct double.
// Returns false, when guess is either correct or the next-lower double.
static bool compute_guess(walle::BufferRef<const char> trimmed, int exponent,
                         double* guess) 
{
    int s = static_cast<int>(trimmed.size());
    if (s == 0) {
        *guess = 0.0;
        return true;
    }
    if ((exponent + s - 1) >= kMaxDecimalPower) {
        *guess = Double::infinity();
        return true;
    }
    if ((exponent + s) <= kMinDecimalPower) {
        *guess = 0.0;
        return true;
    }

    if (double_strtod(trimmed, exponent, guess) ||
        simple_fp_strtod(trimmed, exponent, guess)) {
        return true;
    }
    if (*guess == Double::infinity()) {
        return true;
    }
    return false;
}

double str_to_double(walle::BufferRef<const char> buffer, int exponent) 
{
    char copy_buffer[kMaxSignificantDecimalDigits];
    walle::BufferRef<const char> trimmed;
    int updated_exponent;
    trim_and_cut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
             &trimmed, &updated_exponent);
    exponent = updated_exponent;

    double guess;
    bool is_correct = compute_guess(trimmed, exponent, &guess);
    if (is_correct) {
        return guess;
    }

    SimpleFp upper_boundary = Double(guess).upper_boundary();
    int comparison = compare_buffer_with_simple_fp(trimmed, exponent, upper_boundary);
    if (comparison < 0) {
        return guess;
    } else if (comparison > 0) {
        return Double(guess).next_double();
    } else if ((Double(guess).significand() & 1) == 0) {
        // Round towards even.
        return guess;
    } else {
        return Double(guess).next_double();
    }
}

float str_to_float(walle::BufferRef<const char> buffer, int exponent) 
{
    char copy_buffer[kMaxSignificantDecimalDigits];
    walle::BufferRef<const char> trimmed;
    int updated_exponent;
    trim_and_cut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
             &trimmed, &updated_exponent);
    exponent = updated_exponent;

    double double_guess;
    bool is_correct = compute_guess(trimmed, exponent, &double_guess);

    float float_guess = static_cast<float>(double_guess);
    if (float_guess == double_guess) {
        // This shortcut triggers for integer values.
        return float_guess;
    }

    // We must catch double-rounding. Say the double has been rounded up, and is
    // now a boundary of a float, and rounds up again. This is why we have to
    // look at previous too.
    // Example (in decimal numbers):
    //    input: 12349
    //    high-precision (4 digits): 1235
    //    low-precision (3 digits):
    //       when read from input: 123
    //       when rounded from high precision: 124.
    // To do this we simply look at the neigbors of the correct result and see
    // if they would round to the same float. If the guess is not correct we have
    // to look at four values (since two different doubles could be the correct
    // double).

    double double_next = Double(double_guess).next_double();
    double double_previous = Double(double_guess).previous_double();

    float f1 = static_cast<float>(double_previous);
    float f2 = float_guess;
    float f3 = static_cast<float>(double_next);
    float f4;
    if (is_correct) {
        f4 = f3;
    } else {
        double double_next2 = Double(double_next).next_double();
        f4 = static_cast<float>(double_next2);
    }
    (void) f2;  // Mark variable as used.
    WALLE_ASSERT(f1 <= f2 && f2 <= f3 && f3 <= f4, "");

    // If the guess doesn't lie near a single-precision boundary we can simply
    // return its float-value.
    if (f1 == f4) {
        return float_guess;
    }

    WALLE_ASSERT((f1 != f2 && f2 == f3 && f3 == f4) ||
         (f1 == f2 && f2 != f3 && f3 == f4) ||
         (f1 == f2 && f2 == f3 && f3 != f4), "");

    // guess and next are the two possible canditates (in the same way that
    // double_guess was the lower candidate for a double-precision guess).
    float guess = f1;
    float next = f4;
    SimpleFp upper_boundary;
    if (guess == 0.0f) {
        float min_float = 1e-45f;
        upper_boundary = Double(static_cast<double>(min_float) / 2).to_simplefp();
    } else {
        upper_boundary = Single(guess).upper_boundary();
    }
    int comparison = compare_buffer_with_simple_fp(trimmed, exponent, upper_boundary);
    if (comparison < 0) {
        return guess;
    } else if (comparison > 0) {
        return next;
    } else if ((Single(guess).significand() & 1) == 0) {
        // Round towards even.
        return guess;
    } else {
        return next;
    }
}
#if WALLE_COMPILER_CLANG
    WALLE_RESTORE_CLANG_WARNING()
#endif
     WALLE_RESTORE_GCC_WARNING()
#if  WALLE_COMPILER_GNUC

#endif
} //namespace format_detail
} //namespace walle
