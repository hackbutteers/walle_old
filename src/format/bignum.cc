#include <walle/format/detail/bignum.h>
#include <walle/math/math.h>
#include <algorithm>
#include <iostream>
namespace walle {
namespace format_detail {

template <typename S>
static size_t bit_size(const S &v)
{
    WALLE_UNUSED(v);
    return 8 * sizeof(v);
}


Bignum::Bignum()
    : _bigits(_bigits_buffer, kBigitCapacity), 
      _used_bigits(0), 
      _exponent(0) 
{
    for(int i = 0; i < kBigitCapacity; i++) {
        _bigits[i] = 0;
    }
}

void Bignum::assign_uint16(uint16_t v)
{
    WALLE_ASSERT(kBigitSize >= bit_size(v), "");
    zero();
    if (v == 0){
        return;
    }
    ensure_capcity(1);
    _bigits[0] = v;
    _used_bigits = 1;
}

void Bignum::ensure_capcity(int size)
{
    WALLE_ASSERT(size <= kBigitCapacity, "");
}

void Bignum::assign_uint64(uint64_t v)
{
    const int kUInt64Size = 64;
    zero();
    if(v == 0) {
        return;
    }

    int neededBigits = kUInt64Size / kBigitSize + 1;
    ensure_capcity(neededBigits);
    for (int i = 0; i < neededBigits; ++i) {
        _bigits[i] = v & kBigitMask;
        v = v >> kBigitSize;
    }
    _used_bigits = neededBigits;
    clamp();
}

static uint64_t read_uint64(walle::BufferRef<const char> buffer,
                           int From,
                           int DigitsToRead) 
{
    uint64_t Result = 0;
    for (int i = From; i < From + DigitsToRead; ++i) {
        int digit = buffer[i] - '0';
        WALLE_ASSERT(0 <= digit && digit <= 9, "not digit");
        Result = Result * 10 + digit;
    }
    return Result;
}


// Precondition: this/other < 16bit.
uint16_t Bignum::divide_modulo_int_bignum(const Bignum& other) 
{
    WALLE_ASSERT(is_clamped(), "");
    WALLE_ASSERT(other.is_clamped(), "");
    WALLE_ASSERT(other._used_bigits > 0, "");

    // Easy case: if we have less digits than the divisor than the result is 0.
    // Note: this handles the case where this == 0, too.
    if (bigit_length() < other.bigit_length()) {
        return 0;
    }

    align(other);

    uint16_t result = 0;

    // Start by removing multiples of 'other' until both numbers have the same
    // number of digits.
    while (bigit_length() > other.bigit_length()) {
        // This naive approach is extremely inefficient if `this` divided by other
        // is big. This function is implemented for doubleToString where
        // the result should be small (less than 10).
        WALLE_ASSERT(other._bigits[other._used_bigits - 1] >= ((1 << kBigitSize) / 16), "");
        WALLE_ASSERT(_bigits[_used_bigits - 1] < 0x10000, "");
        // Remove the multiples of the first digit.
        // Example this = 23 and other equals 9. -> Remove 2 multiples.
        result += static_cast<uint16_t>(_bigits[_used_bigits - 1]);
        subtract_times(other, _bigits[_used_bigits - 1]);
    }

    WALLE_ASSERT(bigit_length() == other.bigit_length(), "");

    // Both bignums are at the same length now.
    // Since other has more than 0 digits we know that the access to
    // _bigits[_used_bigits - 1] is safe.
    Chunk this_bigit = _bigits[_used_bigits - 1];
    Chunk other_bigit = other._bigits[other._used_bigits - 1];

    if (other._used_bigits == 1) {
        // Shortcut for easy (and common) case.
        int quotient = this_bigit / other_bigit;
        _bigits[_used_bigits - 1] = this_bigit - other_bigit * quotient;
        WALLE_ASSERT(quotient < 0x10000, "");
        result += static_cast<uint16_t>(quotient);
        clamp();
        return result;
    }

    int division_estimate = this_bigit / (other_bigit + 1);
    WALLE_ASSERT(division_estimate < 0x10000, "");
    result += static_cast<uint16_t>(division_estimate);
    subtract_times(other, division_estimate);

    if (other_bigit * (division_estimate + 1) > this_bigit) {
        // No need to even try to subtract. Even if other's remaining digits were 0
        // another subtraction would be too much.
        return result;
    }

    while (less_equal(other, *this)) {
        subtract_bignum(other);
        result++;
    }
    return result;
}

void Bignum::assign_decimal_string(walle::BufferRef<const char> value)
{
    // 2^64 = 18446744073709551616 > 10^19
    const int kMaxUint64DecimalDigits = 19;
    zero();
    int length = value.size();
    unsigned int pos = 0;
    // Let's just say that each digit needs 4 bits.
    while (length >= kMaxUint64DecimalDigits) {
        uint64_t digits = read_uint64(value, pos, kMaxUint64DecimalDigits);
        pos += kMaxUint64DecimalDigits;
        length -= kMaxUint64DecimalDigits;
        multi_by_power_of_ten(kMaxUint64DecimalDigits);
        add_uint64(digits);
    }
    uint64_t digits = read_uint64(value, pos, length);
    multi_by_power_of_ten(length);
    add_uint64(digits);
    clamp();
}

static int hex_char_value(char c) 
{
    if ('0' <= c && c <= '9') 
        return c - '0';
    if ('a' <= c && c <= 'f') 
        return 10 + c - 'a';
    WALLE_ASSERT('A' <= c && c <= 'F',"");
    return 10 + c - 'A';
}

void Bignum::assign_hex_string(walle::BufferRef<const char> value) {
    zero();
    int length = value.size();

    int needed_bigits = length * 4 / kBigitSize + 1;
    ensure_capcity(needed_bigits);
    int string_index = length - 1;
    for (int i = 0; i < needed_bigits - 1; ++i) {
        // These bigits are guaranteed to be "full".
        Chunk current_bigit = 0;
        for (int j = 0; j < kBigitSize / 4; j++) {
            current_bigit += hex_char_value(value[string_index--]) << (j * 4);
        }
        _bigits[i] = current_bigit;
    }
    _used_bigits = needed_bigits - 1;

    Chunk most_significant_bigit = 0;  // Could be = 0;
    for (int j = 0; j <= string_index; ++j) {
        most_significant_bigit <<= 4;
        most_significant_bigit += hex_char_value(value[j]);
    }
    if (most_significant_bigit != 0) {
        _bigits[_used_bigits] = most_significant_bigit;
        _used_bigits++;
    }
  clamp();
}

void Bignum::add_uint64(uint64_t operand) 
{
    if (operand == 0) 
        return;
    Bignum other;
    other.assign_uint64(operand);
    add_bignum(other);
}

void Bignum::assign_bignum(const Bignum& other) 
{
    _exponent = other._exponent;
    for (int i = 0; i < other._used_bigits; ++i) {
        _bigits[i] = other._bigits[i];
    }
    // Clear the excess digits (if there were any).
    for (int i = other._used_bigits; i < _used_bigits; ++i) {
        _bigits[i] = 0;
    }
    _used_bigits = other._used_bigits;
}

void Bignum::add_bignum(const Bignum& other) {
    WALLE_ASSERT(is_clamped(), "");
    WALLE_ASSERT(other.is_clamped(), "");

    // If this has a greater exponent than other append zero-bigits to this.
    // After this call exponent_ <= other.exponent_.
    align(other);

    // There are two possibilities:
    //   aaaaaaaaaaa 0000  (where the 0s represent a's exponent)
    //     bbbbb 00000000
    //   ----------------
    //   ccccccccccc 0000
    // or
    //    aaaaaaaaaa 0000
    //  bbbbbbbbb 0000000
    //  -----------------
    //  cccccccccccc 0000
    // In both cases we might need a carry bigit.

    Chunk carry = 0;
    int bigit_pos = other._exponent - _exponent;
    WALLE_ASSERT(bigit_pos >= 0, "");
    for (int i = 0; i < other._used_bigits; ++i) {
        Chunk sum = _bigits[bigit_pos] + other._bigits[i] + carry;
        _bigits[bigit_pos] = sum & kBigitMask;
        carry = sum >> kBigitSize;
        bigit_pos++;
    }

    while (carry != 0) {
        Chunk sum = _bigits[bigit_pos] + carry;
        _bigits[bigit_pos] = sum & kBigitMask;
        carry = sum >> kBigitSize;
        bigit_pos++;
    }
    _used_bigits = std::max(bigit_pos, _used_bigits);
    WALLE_ASSERT(is_clamped(),"not clamped");
}


void Bignum::subtract_bignum(const Bignum& other) 
{
    WALLE_ASSERT(is_clamped(), "this not clamped");
    WALLE_ASSERT(other.is_clamped(), "other not clamed");
    // We require this to be bigger than other.
    WALLE_ASSERT(less_equal(other, *this),"this >= other");

    align(other);

    int offset = other._exponent - _exponent;
    Chunk borrow = 0;
    int i;
    for (i = 0; i < other._used_bigits; ++i) {
        WALLE_ASSERT((borrow == 0) || (borrow == 1), "");
        Chunk difference = _bigits[i + offset] - other._bigits[i] - borrow;
        _bigits[i + offset] = difference & kBigitMask;
        borrow = difference >> (kChunkSize - 1);
    }
    while (borrow != 0) {
        Chunk difference = _bigits[i + offset] - borrow;
        _bigits[i + offset] = difference & kBigitMask;
        borrow = difference >> (kChunkSize - 1);
        ++i;
    }
  clamp();
}

void Bignum::shift_left(int shift_amount) 
{
    if (_used_bigits == 0) 
        return;
    _exponent += shift_amount / kBigitSize;
    int local_shift = shift_amount % kBigitSize;
    bigits_shift_left(local_shift);
}

void Bignum::multi_by_uint32(uint32_t factor) 
{
    if (factor == 1) 
        return;
    if (factor == 0) {
        zero();
        return;
    }
    
    if (_used_bigits == 0) 
        return;

    // The product of a bigit with the factor is of size kBigitSize + 32.
    // Assert that this number + 1 (for the carry) fits into double chunk.
    WALLE_ASSERT(kDoubleChunkSize >= kBigitSize + 32 + 1, "");
    DoubleChunk carry = 0;
    for (int i = 0; i < _used_bigits; ++i) {
        DoubleChunk product = static_cast<DoubleChunk>(factor) * _bigits[i] + carry;
        _bigits[i] = static_cast<Chunk>(product & kBigitMask);
        carry = (product >> kBigitSize);
    }

    while (carry != 0) {
        ensure_capcity(_used_bigits + 1);
        _bigits[_used_bigits] = carry & kBigitMask;
        _used_bigits++;
        carry >>= kBigitSize;
    }
}

void Bignum::multi_by_uint64(uint64_t factor) {
    if (factor == 1) 
        return;
    if (factor == 0) {
        zero();
        return;
    }

    WALLE_ASSERT(kBigitSize < 32, "");
    uint64_t carry = 0;
    uint64_t low = factor & 0xFFFFFFFF;
    uint64_t high = factor >> 32;
    for (int i = 0; i < _used_bigits; ++i) {
        uint64_t product_low = low * _bigits[i];
        uint64_t product_high = high * _bigits[i];
        uint64_t tmp = (carry & kBigitMask) + product_low;
        _bigits[i] = tmp & kBigitMask;
        carry = (carry >> kBigitSize) + (tmp >> kBigitSize) +
            (product_high << (32 - kBigitSize));
    }
    while (carry != 0) {
        ensure_capcity(_used_bigits + 1);
        _bigits[_used_bigits] = carry & kBigitMask;
        _used_bigits++;
        carry >>= kBigitSize;
    }
}

void Bignum::multi_by_power_of_ten(int exponent) 
{
    const uint64_t kFive27 = WALLE_UINT64_2PART_C(0x6765c793, fa10079d);
    const uint16_t kFive1 = 5;
    const uint16_t kFive2 = kFive1 * 5;
    const uint16_t kFive3 = kFive2 * 5;
    const uint16_t kFive4 = kFive3 * 5;
    const uint16_t kFive5 = kFive4 * 5;
    const uint16_t kFive6 = kFive5 * 5;
    const uint32_t kFive7 = kFive6 * 5;
    const uint32_t kFive8 = kFive7 * 5;
    const uint32_t kFive9 = kFive8 * 5;
    const uint32_t kFive10 = kFive9 * 5;
    const uint32_t kFive11 = kFive10 * 5;
    const uint32_t kFive12 = kFive11 * 5;
    const uint32_t kFive13 = kFive12 * 5;
    const uint32_t kFive1_to_12[] =
        { kFive1, kFive2, kFive3, kFive4, kFive5, kFive6,
            kFive7, kFive8, kFive9, kFive10, kFive11, kFive12 };

    WALLE_ASSERT(exponent >= 0, "");
    if (exponent == 0) 
        return;
    if (_used_bigits == 0) 
        return;

    // We shift by exponent at the end just before returning.
    int remaining_exponent = exponent;
    while (remaining_exponent >= 27) {
        multi_by_uint64(kFive27);
        remaining_exponent -= 27;
    }
    while (remaining_exponent >= 13) {
        multi_by_uint32(kFive13);
        remaining_exponent -= 13;
    }
    if (remaining_exponent > 0) {
        multi_by_uint32(kFive1_to_12[remaining_exponent - 1]);
    }
    shift_left(exponent);
}

void Bignum::square() 
{
    WALLE_ASSERT(is_clamped(), "");
    int product_length = 2 * _used_bigits;
    ensure_capcity(product_length);

    // Comba multiplication: compute each column separately.
    // Example: r = a2a1a0 * b2b1b0.
    //    r =  1    * a0b0 +
    //        10    * (a1b0 + a0b1) +
    //        100   * (a2b0 + a1b1 + a0b2) +
    //        1000  * (a2b1 + a1b2) +
    //        10000 * a2b2
    //
    // In the worst case we have to accumulate nb-digits products of digit*digit.
    //
    // Assert that the additional number of bits in a DoubleChunk are enough to
    // sum up used_digits of Bigit*Bigit.
    if ((1 << (2 * (kChunkSize - kBigitSize))) <= _used_bigits) {
        WALLE_ASSERT(false,"abort()");
    }
    DoubleChunk accumulator = 0;
    // First shift the digits so we don't overwrite them.
    int copy_offset = _used_bigits;
    for (int i = 0; i < _used_bigits; ++i) {
        _bigits[copy_offset + i] = _bigits[i];
    }
    // We have two loops to avoid some 'if's in the loop.
    for (int i = 0; i < _used_bigits; ++i) {
        // Process temporary digit i with power i.
        // The sum of the two indices must be equal to i.
        int bigit_index1 = i;
        int bigit_index2 = 0;
        // Sum all of the sub-products.
        while (bigit_index1 >= 0) {
            Chunk chunk1 = _bigits[copy_offset + bigit_index1];
            Chunk chunk2 = _bigits[copy_offset + bigit_index2];
            accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
        bigit_index1--;
        bigit_index2++;
        }
        _bigits[i] = static_cast<Chunk>(accumulator) & kBigitMask;
        accumulator >>= kBigitSize;
    }
    
    for (int i = _used_bigits; i < product_length; ++i) {
        int bigit_index1 = _used_bigits - 1;
        int bigit_index2 = i - bigit_index1;
        // Invariant: sum of both indices is again equal to i.
        // Inner loop runs 0 times on last iteration, emptying accumulator.
        while (bigit_index2 < _used_bigits) {
            Chunk chunk1 = _bigits[copy_offset + bigit_index1];
            Chunk chunk2 = _bigits[copy_offset + bigit_index2];
            accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
            bigit_index1--;
            bigit_index2++;
        }
        // The overwritten _bigits[i] will never be read in further loop iterations,
        // because bigit_index1 and bigit_index2 are always greater
        // than i - _used_bigits.
        _bigits[i] = static_cast<Chunk>(accumulator) & kBigitMask;
        accumulator >>= kBigitSize;
    }
    // Since the result was guaranteed to lie inside the number the
    // accumulator must be 0 now.
    WALLE_ASSERT(accumulator == 0, "");

    // Don't forget to update the used_digits and the exponent.
    _used_bigits = product_length;
    _exponent *= 2;
    clamp();
}

void Bignum::assign_power_uint16(uint16_t base, int power_exponent) {
    WALLE_ASSERT(base != 0, "");
    WALLE_ASSERT(power_exponent >= 0, "");
    if (power_exponent == 0) {
        assign_uint16(1);
        return;
    }
    zero();
    int shifts = 0;
    // We expect base to be in range 2-32, and most often to be 10.
    // It does not make much sense to implement different algorithms for counting
    // the bits.
    while ((base & 1) == 0) {
        base >>= 1;
        shifts++;
    }
    int bit_size = 0;
    int tmp_base = base;
    while (tmp_base != 0) {
        tmp_base >>= 1;
        bit_size++;
    }
    int final_size = bit_size * power_exponent;
    // 1 extra bigit for the shifting, and one for rounded final_size.
    ensure_capcity(final_size / kBigitSize + 2);

    // Left to Right exponentiation.
    int mask = 1;
    while (power_exponent >= mask) {
        mask <<= 1;
    }

    // The mask is now pointing to the bit above the most significant 1-bit of
    // power_exponent.
    // Get rid of first 1-bit;
    mask >>= 2;
    uint64_t this_value = base;

    bool delayed_multipliciation = false;
    const uint64_t max_32bits = 0xFFFFFFFF;
    while (mask != 0 && this_value <= max_32bits) {
        this_value = this_value * this_value;
        // Verify that there is enough space in this_value to perform the
        // multiplication.  The first bit_size bits must be 0.
        if ((power_exponent & mask) != 0) {
            uint64_t base_bits_mask =
                ~((static_cast<uint64_t>(1) << (64 - bit_size)) - 1);
            bool high_bits_zero = (this_value & base_bits_mask) == 0;
            if (high_bits_zero) {
                this_value *= base;
            } else {
                delayed_multipliciation = true;
            }
        }
        mask >>= 1;
    }
    assign_uint64(this_value);
    if (delayed_multipliciation) {
        multi_by_uint32(base);
    }

    // Now do the same thing as a bignum.
    while (mask != 0) {
        square();
        if ((power_exponent & mask) != 0) {
            multi_by_uint32(base);
        }
        mask >>= 1;
    }

    // And finally add the saved shifts.
    shift_left(shifts * power_exponent);
}

// Precondition: this/other < 16bit.
uint16_t Bignum::divide_module_by_bignum(const Bignum& other) 
{
    WALLE_ASSERT(is_clamped(), "");
    WALLE_ASSERT(other.is_clamped(), "");
    WALLE_ASSERT(other._used_bigits > 0, "");

    // Easy case: if we have less digits than the divisor than the result is 0.
    // Note: this handles the case where this == 0, too.
    if (bigit_length() < other.bigit_length()) {
        return 0;
    }

    align(other);

    uint16_t result = 0;

    // Start by removing multiples of 'other' until both numbers have the same
    // number of digits.
    while (bigit_length() > other.bigit_length()) {
        // This naive approach is extremely inefficient if `this` divided by other
        // is big. This function is implemented for doubleToString where
        // the result should be small (less than 10).
        WALLE_ASSERT(other._bigits[other._used_bigits - 1] >= ((1 << kBigitSize) / 16), "");
        WALLE_ASSERT(_bigits[_used_bigits - 1] < 0x10000,"");
        // Remove the multiples of the first digit.
        // Example this = 23 and other equals 9. -> Remove 2 multiples.
        result += static_cast<uint16_t>(_bigits[_used_bigits - 1]);
        subtract_times(other, _bigits[_used_bigits - 1]);
    }

    WALLE_ASSERT(bigit_length() == other.bigit_length(), "");

    // Both bignums are at the same length now.
    // Since other has more than 0 digits we know that the access to
    // _bigits[_used_bigits - 1] is safe.
    Chunk this_bigit = _bigits[_used_bigits - 1];
    Chunk other_bigit = other._bigits[other._used_bigits - 1];

    if (other._used_bigits == 1) {
        // Shortcut for easy (and common) case.
        int quotient = this_bigit / other_bigit;
        _bigits[_used_bigits - 1] = this_bigit - other_bigit * quotient;
        WALLE_ASSERT(quotient < 0x10000, "");
        result += static_cast<uint16_t>(quotient);
        clamp();
        return result;
    }

    int division_estimate = this_bigit / (other_bigit + 1);
    WALLE_ASSERT(division_estimate < 0x10000, "");
    result += static_cast<uint16_t>(division_estimate);
    subtract_times(other, division_estimate);

    if (other_bigit * (division_estimate + 1) > this_bigit) {
        // No need to even try to subtract. Even if other's remaining digits were 0
        // another subtraction would be too much.
        return result;
    }

    while (less_equal(other, *this)) {
        subtract_bignum(other);
        result++;
    }
    return result;
}

template<typename S>
static int size_in_hex_chars(S number) 
{
    WALLE_ASSERT(number > 0, "");
    int result = 0;
    while (number != 0) {
        number >>= 4;
        result++;
    }
    return result;
}


static char hex_char_of_value(int value) 
{
    WALLE_ASSERT(0 <= value && value <= 16, "");
    if (value < 10) 
        return static_cast<char>(value + '0');
    return static_cast<char>(value - 10 + 'A');
}

bool Bignum::to_hex_string(char* buffer, int buffer_size) const 
{
    WALLE_ASSERT(is_clamped(), "1");
    // Each bigit must be printable as separate hex-character.
    WALLE_ASSERT(kBigitSize % 4 == 0, "");
    const int kHexCharsPerBigit = kBigitSize / 4;
    if (_used_bigits == 0) {
        if (buffer_size < 2) 
            return false;
        buffer[0] = '0';
        buffer[1] = '\0';
        return true;
    }
    // We add 1 for the terminating '\0' character.
    int needed_chars = (bigit_length() - 1) * kHexCharsPerBigit +
        size_in_hex_chars(_bigits[_used_bigits - 1]) + 1;
    if (needed_chars > buffer_size) 
        return false;
    
    int string_index = needed_chars - 1;
    buffer[string_index--] = '\0';
    for (int i = 0; i < _exponent; ++i) {
        for (int j = 0; j < kHexCharsPerBigit; ++j) {
            buffer[string_index--] = '0';
        }
    }
    for (int i = 0; i < _used_bigits - 1; ++i) {
        Chunk current_bigit = _bigits[i];
        for (int j = 0; j < kHexCharsPerBigit; ++j) {
            buffer[string_index--] = hex_char_of_value(current_bigit & 0xF);
            current_bigit >>= 4;
        }
    }
    // And finally the last bigit.
    Chunk most_significant_bigit = _bigits[_used_bigits - 1];
    while (most_significant_bigit != 0) {
        buffer[string_index--] = hex_char_of_value(most_significant_bigit & 0xF);
        most_significant_bigit >>= 4;
    }
    return true;
}

Bignum::Chunk Bignum::bigit_at(int index) const 
{
    if (index >= bigit_length()) {
        return 0;
    }
    if (index < _exponent) {
        return 0;
    }
    return _bigits[index - _exponent];
}


int Bignum::compare(const Bignum& a, const Bignum& b) 
{
    WALLE_ASSERT(a.is_clamped(), "");
    WALLE_ASSERT(b.is_clamped(), "");
    int bigit_length_a = a.bigit_length();
    int bigit_length_b = b.bigit_length();
    if (bigit_length_a < bigit_length_b) 
        return -1;
    if (bigit_length_a > bigit_length_b) 
        return +1;
    for (int i = bigit_length_a - 1; i >= std::min(a._exponent, b._exponent); --i) {
        Chunk bigit_a = a.bigit_at(i);
        Chunk bigit_b = b.bigit_at(i);
        if (bigit_a < bigit_b) { 
            return -1;
        }
        if (bigit_a > bigit_b) {
            return +1;
        // Otherwise they are equal up to this digit. Try the next digit.
        }
    }
  return 0;
}

int Bignum::plus_compare(const Bignum& a, const Bignum& b, const Bignum& c) {
    WALLE_ASSERT(a.is_clamped(), "");
    WALLE_ASSERT(b.is_clamped(), "");
    WALLE_ASSERT(c.is_clamped(), "");
    if (a.bigit_length() < b.bigit_length()) {
        return plus_compare(b, a, c);
    }
    if (a.bigit_length() + 1 < c.bigit_length()) 
        return -1;
    if (a.bigit_length() > c.bigit_length()) 
        return +1;
    // The exponent encodes 0-bigits. So if there are more 0-digits in 'a' than
    // 'b' has digits, then the bigit-length of 'a'+'b' must be equal to the one
    // of 'a'.
    if (a._exponent >= b.bigit_length() && a.bigit_length() < c.bigit_length()) {
        return -1;
    }

    Chunk borrow = 0;
    // Starting at min_exponent all digits are == 0. So no need to compare them.
    int min_exponent = std::min(std::min(a._exponent, b._exponent), c._exponent);
    for (int i = c.bigit_length() - 1; i >= min_exponent; --i) {
        Chunk chunk_a = a.bigit_at(i);
        Chunk chunk_b = b.bigit_at(i);
        Chunk chunk_c = c.bigit_at(i);
        Chunk sum = chunk_a + chunk_b;
        if (sum > chunk_c + borrow) {
            return +1;
        } else {
            borrow = chunk_c + borrow - sum;
            if (borrow > 1) 
                return -1;
            borrow <<= kBigitSize;
        }
    }
    if (borrow == 0)
        return 0;
    return -1;
}

void Bignum::zero()
{
    for(int i = 0; i < kBigitCapacity; i++) {
        _bigits[i] = 0;
    }
    _used_bigits = 0;
    _exponent = 0;
}

bool Bignum::is_clamped() const 
{
    return _used_bigits == 0 || _bigits[_used_bigits - 1] != 0;
}

void Bignum::clamp()
{
    while (_used_bigits > 0 && _bigits[_used_bigits - 1] == 0) {
        _used_bigits--;
    }
    if (_used_bigits == 0) {
        _exponent = 0;
    }
}

void Bignum::align(const Bignum &other)
{
    /** 
     * @note If "X" represents a "hidden" digit (by the exponent) then we are in the
     * following case (a == this, b == other):
     * a:  aaaaaaXXXX   or a:   aaaaaXXX
     * b:     bbbbbbX      b: bbbbbbbbXX
     * We replace some of the hidden digits (X) of a with 0 digits.
     * a:  aaaaaa000X   or a:   aaaaa0XX 
     */
    if (_exponent > other._exponent) {
        int zeroDigits = _exponent - other._exponent;
        for (int i = _used_bigits - 1; i >= 0; --i) {
            _bigits[i + zeroDigits] = _bigits[i];
        }
        for (int i = 0; i < zeroDigits; ++i) {
            _bigits[i] = 0;
        }
        _used_bigits += zeroDigits;
        _exponent -= zeroDigits;
        WALLE_ASSERT(_used_bigits >= 0, "");
        WALLE_ASSERT(_exponent >= 0, "");
    }
}
/*
void Bignum::shift_left(int shiftAmount)
{
    WALLE_ASSERT(shiftAmount < kBigitSize, "");
    WALLE_ASSERT(shiftAmount >= 0,"");
    Chunk carry = 0;
    for (int i = 0; i < _used_bigits; ++i) {
        Chunk new_carry = _bigits[i] >> (kBigitSize - shiftAmount);
        _bigits[i] = ((_bigits[i] << shiftAmount) + carry) & kBigitMask;
        carry = new_carry;
    }
    if (carry != 0) {
        _bigits[_used_bigits]= carry;
        _used_bigits++;
    }
}
*/
void Bignum::subtract_times(const Bignum& other, int factor)
{
    WALLE_ASSERT(_exponent <= other._exponent, "");
    if (factor < 3) {
        for (int i = 0; i < factor; ++i) {
            subtract_bignum(other);
        }
        return;
    }
    Chunk borrow = 0;
    int exponentDiff = other._exponent - _exponent;
    for (int i = 0; i < other._used_bigits; ++i) {
        DoubleChunk product = static_cast<DoubleChunk>(factor) * other._bigits[i];
        DoubleChunk remove = borrow + product;
        Chunk difference = _bigits[i + exponentDiff] - (remove & kBigitMask);
        _bigits[i + exponentDiff] = difference & kBigitMask;
        borrow = static_cast<Chunk>((difference >> (kChunkSize - 1)) +
                                (remove >> kBigitSize));
    }
    for (int i = other._used_bigits + exponentDiff; i < _used_bigits; ++i) {
        if (borrow == 0) return;
        Chunk difference = _bigits[i] - borrow;
        _bigits[i] = difference & kBigitMask;
        borrow = difference >> (kChunkSize - 1);
    }
    clamp();
}

void Bignum::bigits_shift_left(int shift_amount) 
{
    WALLE_ASSERT(shift_amount < kBigitSize, "");
    WALLE_ASSERT(shift_amount >= 0, "");
    Chunk carry = 0;
    for (int i = 0; i < _used_bigits; ++i) {
        Chunk new_carry = _bigits[i] >> (kBigitSize - shift_amount);
        _bigits[i] = ((_bigits[i] << shift_amount) + carry) & kBigitMask;
        carry = new_carry;
    }
    if (carry != 0) {
        _bigits[_used_bigits] = carry;
        _used_bigits++;
    }
}
} //namespace format_detail
} //namespace walle