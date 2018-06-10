#ifndef WALLE_FORMAT_DETAIL_SIMPLE_FP_H_
#define WALLE_FORMAT_DETAIL_SIMPLE_FP_H_
#include <walle/config/base.h>
#include <walle/math/math.h>

namespace walle {
namespace format_detail {

/** 
 * @brief  do it your self float-pointing.
 * @note   This "Do It Yourself Floating Point" class implements a floating-point number
 *         with a uint64 significand and an int exponent. Normalized SimpleFp numbers will
 *         have the most significant bit of the significand set.
 * multiplication and subtraction do not normalize their results.
 * SimpleFp are not designed to contain special doubles (NaN and Infinity).
 * @retval None
 */
class SimpleFp {
public:
    SimpleFp() : _f(0), _e(0) 
    {

    }
    SimpleFp(uint64_t significand, int exponent) : _f(significand), _e(exponent) 
    {

    }

    /** 
     * @brief  this = this - other.
     * @note   The exponents of both numbers must be the same and the
     *         significand of this must be bigger than the significand of 
     *         other. The result will not be normalized.
     * @param  &other: 
     * @retval None
     */
    void subtract(const SimpleFp &other) 
    {
        WALLE_ASSERT(_e == other._e, "");
        WALLE_ASSERT(_f >= other._f, "");
        _f -= other._f;
    }

    /** 
     * @brief  Returns a - b.
     * @note   The exponents of both numbers must be the same and 
     *         this must be bigger than other. The result will not be 
     *         normalized.
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static SimpleFp minus(const SimpleFp &a, const SimpleFp &b) 
    {
        SimpleFp result = a;
        result.subtract(b);
        return result;
    }

    /** 
     * @brief  return this * other
     * @note   
     * @param  &other: 
     * @retval None
     */
    void multiply(const SimpleFp &other);

    /** 
     * @brief  return this *other
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static SimpleFp times(const SimpleFp &a, const SimpleFp &b) 
    {
        SimpleFp result = a;
        result.multiply(b);
        return result;
    }

    void normalize() 
    {
        WALLE_ASSERT(_f != 0, "");
        uint64_t significand = _f;
        int exponent = _e;

        // This method is mainly called for normalizing boundaries. In general
        // boundaries need to be shifted by 10 bits. We thus optimize for this case.
        const uint64_t k10MSBits = WALLE_UINT64_2PART_C(0xFFC00000, 00000000);
        while ((significand & k10MSBits) == 0) {
            significand <<= 10;
            exponent -= 10;
        }
        while ((significand & kUint64MSB) == 0) {
            significand <<= 1;
            exponent--;
        }
        _f = significand;
        _e = exponent;
    }

    static SimpleFp normalize(const SimpleFp &a) 
    {
        SimpleFp result = a;
        result.normalize();
        return result;
    }

    uint64_t f() const 
    { 
        return _f; 
    }
    int e() const 
    { 
        return _e; 
    }

    void set_f(uint64_t new_value) 
    { 
        _f = new_value; 
    }

    void set_e(int new_value) 
    { 
        _e = new_value; 
    }
public:
    static const int kSignificandSize = 64;
private:
    static const uint64_t kUint64MSB = WALLE_UINT64_2PART_C(0x80000000, 00000000);
    
    uint64_t _f;
    int      _e;

};
} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_SIMPLE_FP_H_