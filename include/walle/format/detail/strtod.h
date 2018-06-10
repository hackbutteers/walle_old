#ifndef WALLE_FORMAT_DETAIL_STRTOD_H_
#define WALLE_FORMAT_DETAIL_STRTOD_H_
#include <walle/buffer/buffer_ref.h>

namespace walle {
namespace format_detail {

/** 
 * @brief convert string to double
 * @note  The buffer must only contain digits in the range [0-9]. It must not
 * contain a dot or a sign. It must not start with '0', and must not be empty. 
 * @param  buffer: 
 * @param  exponent: 
 * @retval 
 */
double str_to_double(walle::BufferRef<const char> buffer, int exponent);

/** 
 * @brief convert string to float 
 * @note  The buffer must only contain digits in the range [0-9]. It must not
 * contain a dot or a sign. It must not start with '0', and must not be empty.
 * @param  buffer: 
 * @param  exponent: 
 * @retval 
 */
float str_to_float(walle::BufferRef<const char> buffer, int exponent);
} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_STRTOD_H_