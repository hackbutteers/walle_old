#ifndef WALLE_FORMAT_DETAIL_CACHED_POWERS_H_
#define WALLE_FORMAT_DETAIL_CACHED_POWERS_H_
#include <walle/format/detail/simple_fp.h>

namespace walle {
namespace format_detail {

class PowersOfTenCache {
public:
    /** 
     * @note Not all powers of ten are cached. The decimal exponent 
     * of two neighboring cached numbers will differ by 
     * kDecimalExponentDistance.
     */
    static const int kDecimalExponentDistance;

    static const int kMinDecimalExponent;
    static const int kMaxDecimalExponent;

    /** 
     * @brief Returns a cached power-of-ten with a binary exponent in the range
     * [min_exponent; max_exponent] (boundaries included).
     * @note  
     * @param  min_exponent: 
     * @param  max_exponent: 
     * @param  power: 
     * @param  decimal_exponent: 
     * @retval None
     */
    static void get_cached_power_for_binary_exponent_range(int minExponent,
                                                   int maxExponent,
                                                   SimpleFp* power,
                                                   int* decimalExponent);
    
    /** 
     * @brief  
     * @note Returns a cached power of ten x ~= 10^k such that
     *       k <= decimal_exponent < k + kCachedPowersDecimalDistance.
     *       The given decimal_exponent must satisfy
     *       kMinDecimalExponent <= requested_exponent, and
     *       requested_exponent < kMaxDecimalExponent + kDecimalExponentDistance.  
     * @param  requested_exponent: 
     * @param  power: 
     * @param  found_exponent: 
     * @retval None
     */                                               
    static void get_cached_power_for_decimal_exponent(int requestedExponent,
                                               SimpleFp* power,
                                               int* foundExponent);
};

} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_CACHED_POWERS_H_