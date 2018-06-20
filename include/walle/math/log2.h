#ifndef WALLE_MATH_LOG2_H_
#define WALLE_MATH_LOG2_H_
#include <walle/config/base.h>
namespace walle {
namespace math {
    
template <typename IntegerType>
static WALLE_FORCE_INLINE unsigned integer_log2_floor_template(IntegerType i) 
{
    unsigned p = 0;
    while (i >= 65536) i >>= 16, p += 16;
    while (i >= 256) i >>= 8, p += 8;
    while (i >>= 1) ++p;
    return p;
}

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(int i);

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(unsigned int i);

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(long i);

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(unsigned long i);

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(long long i);

/** 
 * @brief calculate the log2 floor of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_floor(unsigned long long i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(int i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(unsigned int i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(long i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(unsigned long i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(long long i);

/** 
 * @brief calculate the log2 ceiling of an integer type (by repeated bit shifts) 
 * @note   
 * @param  i: 
 * @retval 
 */
unsigned integer_log2_ceil(unsigned long long i);
}
}
#endif //WALLE_MATH_LOG2_H_