#include <walle/format/detail/simple_fp.h>

namespace walle {
namespace format_detail {

void SimpleFp::multiply(const SimpleFp& other) {
    // Simply "emulates" a 128 bit multiplication.
    // However: the resulting number only contains 64 bits. The least
    // significant 64 bits are only used for rounding the most significant 64
    // bits.
    const uint64_t kM32 = 0xFFFFFFFFU;
    uint64_t a = _f >> 32;
    uint64_t b = _f & kM32;
    uint64_t c = other._f >> 32;
    uint64_t d = other._f & kM32;
    uint64_t ac = a * c;
    uint64_t bc = b * c;
    uint64_t ad = a * d;
    uint64_t bd = b * d;
    uint64_t tmp = (bd >> 32) + (ad & kM32) + (bc & kM32);
    // By adding 1U << 31 to tmp we round the final result.
    // Halfway cases will be round up.
    tmp += 1U << 31;
    uint64_t result_f = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
    _e += other._e + 64;
    _f = result_f;
}
} //namespace format_detail
} //namespace walle