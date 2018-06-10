#ifndef WALLE_FORMAT_DETAIL_FAST_DTOA_H_
#define WALLE_FORMAT_DETAIL_FAST_DTOA_H_
#include <walle/buffer/buffer_ref.h>

namespace walle {
namespace format_detail {

enum class FastDtoaMode {

  /** 
   * @note Computes the shortest representation of the given input. The returned
   * result will be the most accurate number of this length. Longer
   * representations might be more accurate.  
   */
  eFastDtoaShortest,

  /** 
   * @note Same as FAST_DTOA_SHORTEST but for single-precision floats.  
   */
  eFastDtoaShortestSingle,
  /** 
   * @note Computes a representation where the precision (number of digits) is
   * given as input. The precision is independent of the decimal point. 
   */
  eFastDtoaPrecision
  
};

/** 
 * @note FastDtoa will produce at most kFastDtoaMaximalLength digits. This does not
 *  include the terminating '\0' character.  
 */
static const int kFastDtoaMaximalLength = 17;

/** 
 * @note  Same for single-precision numbers. 
 */
static const int kFastDtoaMaximalSingleLength = 9;

/** 
 * @brief Provides a decimal representation of v. 
 * @note  The result should be interpreted as buffer * 10^(point - length).
 *        Precondition:
 *        * v must be a strictly positive finite double. 
 *        There will be *length digits inside the buffer followed by a null terminator.
 *        If the function returns true and mode equals
 *        - eFastDtoaShortest, then
 *        the parameter requested_digits is ignored.
 *        The result satisfies
 *        v == (double) (buffer * 10^(point - length)).
 *        The digits in the buffer are the shortest representation possible. E.g.
 *        if 0.099999999999 and 0.1 represent the same double then "1" is returned
 *        with point = 0.
 *        The last digit will be closest to the actual v. That is, even if several
 *        digits might correctly yield 'v' when read again, the buffer will contain
 *        the one closest to v.
 *        - eFastDtoaPrecision, then
 *        the buffer contains requested_digits digits.
 *        the difference v - (buffer * 10^(point-length)) is closest to zero for
 *        all possible representations of requested_digits digits.
 *        If there are two values that are equally close, then FastDtoa returns
 *        false.
 *        For both modes the buffer must be large enough to hold the result. 
 *         
 * @param  length: 
 * @retval Returns true if it succeeds, otherwise the result can not be trusted.
 */
bool fast_dtoa(double d,
              FastDtoaMode mode,
              int requestedDigits,
              walle::BufferRef<char> buffer,
              int* length,
              int* decimal_point);
}
}
#endif //WALLE_FORMAT_DETAIL_FAST_DTOA_H_