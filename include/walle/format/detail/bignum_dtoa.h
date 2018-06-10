#ifndef WALLE_FORMAT_DETAIL_BIGNUM_DTOA_H_
#define WALLE_FORMAT_DETAIL_BIGNUM_DTOA_H_
#include <walle/buffer/buffer_ref.h>

namespace walle {
namespace format_detail {

enum class BignumDtoaMode {
    eBignumDtoaShortest,
    eBignumDtoaShortestSingle,
    eBignumDtoaFixed,
    eBignumDtoaPrecision
};

void bignum_dtoa(double v, BignumDtoaMode mode, int requestedDigits,
                walle::BufferRef<char> buffer, int* length, int* point);
} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_BIGNUM_DTOA_H_