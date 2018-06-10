#ifndef WALLE_FORMAT_DETAIL_IEEE_H_
#define WALLE_FORMAT_DETAIL_IEEE_H_
#include <walle/config/base.h>
#include <walle/format/detail/simple_fp.h>
#include <walle/config/bits.h>
namespace walle {
namespace format_detail  {

//We assume that doubles and uint64_t have the same endianness.
static uint64_t double_to_uint64(double d) 
{ 
    return bit_cast<uint64_t>(d); 
}

static double uint64_to_double(uint64_t d64) 
{ 
    return bit_cast<double>(d64); 
}

static uint32_t float_to_uint32(float f) 
{ 
    return bit_cast<uint32_t>(f); 
}

static float uint32_to_float(uint32_t d32) 
{ 
    return bit_cast<float>(d32); 
}

class Double {
public:
    Double() : _d64(0) 
    {

    }
    explicit Double(double d) : _d64(double_to_uint64(d)) 
    {

    }
    explicit Double(uint64_t d64) : _d64(d64) 
    {

    }
    explicit Double(SimpleFp simpleFp)
    : _d64(simplefp_to_uint64(simpleFp)) 
    {

    }

    SimpleFp to_simplefp() const 
    {
        WALLE_ASSERT(sign() > 0, "");
        WALLE_ASSERT(!is_special(), "");
        return SimpleFp(significand(), exponent());
    }
    /** 
     * @brief  
     * @note  The value encoded by this Double must be strictly greater than 0.
     * @retval 
     */
    SimpleFp to_normalized_simplefp() const 
    {
        WALLE_ASSERT(value() > 0.0, "");
        uint64_t f = significand();
        int e = exponent();

        // The current double could be a denormal.
        while ((f & kHiddenBit) == 0) {
            f <<= 1;
            e--;
        }
        // Do the final shifts in one go.
        f <<= SimpleFp::kSignificandSize - kSignificandSize;
        e -= SimpleFp::kSignificandSize - kSignificandSize;
        return SimpleFp(f, e);
    }

    /** 
     * @brief  Returns the double's bit as uint64.
     * @note   
     * @retval 
     */
    uint64_t to_uint64() const 
    {
        return _d64;
    }

    /** 
     * @brief Returns the next greater double. 
     *        Returns +infinity on input +infinity. 
     * @note   
     * @retval 
     */
    double next_double() const 
    {
        if (_d64 == kInfinity) 
            return Double(kInfinity).value();
        if (sign() < 0 && significand() == 0) {
            // -0.0
            return 0.0;
        }
        if (sign() < 0) {
            return Double(_d64 - 1).value();
        } else {
            return Double(_d64 + 1).value();
        }
    }

    double previous_double() const 
    {
        if (_d64 == (kInfinity | kSignMask)) 
            return -infinity();
        if (sign() < 0) {
            return Double(_d64 + 1).value();
        } else {
            if (significand() == 0) 
                return -0.0;
            return Double(_d64 - 1).value();
        }
    }

    int exponent() const 
    {
        if (is_denormal()) 
            return kDenormalExponent;

        uint64_t d64 = to_uint64();
        int biasedE =
        static_cast<int>((d64 & kExponentMask) >> kPhysicalSignificandSize);
        return biasedE - kExponentBias;
    }

    uint64_t significand() const 
    {
        uint64_t d64 = to_uint64();
        uint64_t s = d64 & kSignificandMask;
        if (!is_denormal()) {
            return s + kHiddenBit;
        } else {
            return s;
        }
    }

    /** 
     * @brief  Returns true if the double is a denormal.
     * @note   
     * @retval 
     */
    bool is_denormal() const 
    {
        uint64_t d64 = to_uint64();
        return (d64 & kExponentMask) == 0;
    }

    /** 
     * @brief  
     * @note   We consider denormals not to be special. 
     *        Hence only Infinity and nan are special.
     * @retval 
     */
    bool is_special() const 
    {
        uint64_t d64 = to_uint64();
        return (d64 & kExponentMask) == kExponentMask;
    }

    bool is_nan() const 
    {
        uint64_t d64 = to_uint64();
        return ((d64 & kExponentMask) == kExponentMask) &&
            ((d64 & kSignificandMask) != 0);
    }

    bool is_infinite() const 
    {
        uint64_t d64 = to_uint64();
        return ((d64 & kExponentMask) == kExponentMask) &&
            ((d64 & kSignificandMask) == 0);
    }

    int sign() const 
    {
        uint64_t d64 = to_uint64();
        return (d64 & kSignMask) == 0? 1: -1;
    }

    /** 
     * @brief  
     * @note  Precondition: the value encoded by this Double must be greater or 
     *        equal than +0.0.
     * @retval 
     */
    SimpleFp upper_boundary() const 
    {
        WALLE_ASSERT(sign() > 0, "");
        return SimpleFp(significand() * 2 + 1, exponent() - 1);
    }

    /** 
     * @brief  Computes the two boundaries of this.
     * @note   The bigger boundary (m_plus) is normalized. The lower boundary 
     *         has the same exponent as m_plus.
     *         Precondition: the value encoded by this Double must be greater than 0.
     * @param  out_m_minus: 
     * @param  out_m_plus: 
     * @retval None
     */
    void normalized_boundaries(SimpleFp* out_m_minus, SimpleFp* out_m_plus) const 
    {
        WALLE_ASSERT(value() > 0.0, "");
        SimpleFp v = this->to_simplefp();
        SimpleFp m_plus = SimpleFp::normalize(SimpleFp((v.f() << 1) + 1, v.e() - 1));
        SimpleFp m_minus;
        if (lower_boundary_is_closer()) {
            m_minus = SimpleFp((v.f() << 2) - 1, v.e() - 2);
        } else {
            m_minus = SimpleFp((v.f() << 1) - 1, v.e() - 1);
        }
        m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
        m_minus.set_e(m_plus.e());
        *out_m_plus = m_plus;
        *out_m_minus = m_minus;
    }

    /** 
     * @brief  
     * @note The boundary is closer if the significand is of the form f == 2^p-1 then
     *       the lower boundary is closer.
     *       Think of v = 1000e10 and v- = 9999e9.
     *       Then the boundary (== (v - v-)/2) is not just at a distance of 1e9 but
     *       at a distance of 1e8. 
     *       The only exception is for the smallest normal: the largest denormal is 
     *       at the same distance as its successor. 
     *       Note: denormals have the same exponent as the smallest normals. 
     * @retval 
     */
    bool lower_boundary_is_closer() const 
    {
        bool PhysicalSignificandIsZero = ((to_uint64() & kSignificandMask) == 0);
        return PhysicalSignificandIsZero && (exponent() != kDenormalExponent);
    }

    double value() const 
    { 
        return uint64_to_double(_d64); 
    }

    /** 
     * @brief Returns the significand size for a given order of magnitude. 
     * @note If v = f*2^e with 2^p-1 <= f <= 2^p then p+e is v's order of magnitude.
     *       This function returns the number of significant binary digits v will 
     *       have once it's encoded into a double. In almost all cases this is 
     *       equal to kSignificandSize. The only exceptions are denormals. They 
     *       start with leading zeroes and their effective significand-size is hence smaller.
     * @param  order: 
     * @retval 
     */ 
    static int significand_size_for_order_of_magnitude(int order) 
    {
        if (order >= (kDenormalExponent + kSignificandSize)) {
            return kSignificandSize;
        }
        if (order <= kDenormalExponent) 
            return 0;
        return order - kDenormalExponent;
    }

    static double infinity() 
    {
        return Double(kInfinity).value();
    }

    static double nan() 
    {
        return Double(kNaN).value();
    }

private:
    static uint64_t simplefp_to_uint64(SimpleFp simpleFp); 
    WALLE_NON_COPYABLE(Double);
public:
    static const uint64_t kSignMask                = WALLE_UINT64_2PART_C(0x80000000, 00000000);
    static const uint64_t kExponentMask            = WALLE_UINT64_2PART_C(0x7FF00000, 00000000);
    static const uint64_t kSignificandMask         = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    static const uint64_t kHiddenBit               = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    static const int      kPhysicalSignificandSize = 52;  // Excludes the hidden bit.
    static const int      kSignificandSize         = 53;
private:
    static const int      kExponentBias     = 0x3FF + kPhysicalSignificandSize;
    static const int      kDenormalExponent = -kExponentBias + 1;
    static const int      kMaxExponent      = 0x7FF - kExponentBias;
    static const uint64_t kInfinity         = WALLE_UINT64_2PART_C(0x7FF00000, 00000000);
    static const uint64_t kNaN              = WALLE_UINT64_2PART_C(0x7FF80000, 00000000);

    const uint64_t   _d64;
};

inline uint64_t Double::simplefp_to_uint64(SimpleFp simpleFp) 
{
        uint64_t significand = simpleFp.f();
        int exponent = simpleFp.e();
        while (significand > kHiddenBit + kSignificandMask) {
            significand >>= 1;
            exponent++;
        }
        if (exponent >= kMaxExponent) {
            return kInfinity;
        }
        if (exponent < kDenormalExponent) {
            return 0;
        }
        while (exponent > kDenormalExponent && (significand & kHiddenBit) == 0) {
            significand <<= 1;
            exponent--;
        }
        uint64_t biasedExponent;
        if (exponent == kDenormalExponent && (significand & kHiddenBit) == 0) {
            biasedExponent = 0;
        } else {
            biasedExponent = static_cast<uint64_t>(exponent + kExponentBias);
        }
        return (significand & kSignificandMask) |
            (biasedExponent << kPhysicalSignificandSize);
}

class Single{
public:

    Single() : _d32(0) 
    {

    }

    explicit Single(float f) : _d32(float_to_uint32(f)) \
    {

    }
  
    explicit Single(uint32_t d32) : _d32(d32) 
    {
        
    }

    /** 
     * @brief  
     * @note The value encoded by this Single must be greater or equal to +0.0.
     *       It must not be special (infinity, or NaN). 
     * @retval 
     */
    SimpleFp to_simplefp() const 
    {
        WALLE_ASSERT(sign() > 0, "");
        WALLE_ASSERT(!is_special(), "");
        return SimpleFp(significand(), exponent());
    }

    /** 
     * @brief  Returns the single's bit as uint32.
     * @note   
     * @retval 
     */
    uint32_t to_uint32() const 
    {
        return _d32;
    }

    int exponent() const 
    {
        if (is_denormal()) 
            return kDenormalExponent;

        uint32_t d32 = to_uint32();
        int biasedE =
            static_cast<int>((d32 & kExponentMask) >> kPhysicalSignificandSize);
        return biasedE - kExponentBias;
    }

    uint32_t significand() const 
    {
        uint32_t d32 = to_uint32();
        uint32_t s = d32 & kSignificandMask;
        if (!is_denormal()) {
            return s + kHiddenBit;
        } else {
            return s;
        }
    }

    /** 
     * @brief  Returns true if the single is a denormal.
     * @note   
     * @retval 
     */
    bool is_denormal() const 
    {
        uint32_t d32 = to_uint32();
        return (d32 & kExponentMask) == 0;
    }
    /** 
     * @brief  
     * @note We consider denormals not to be special.
     *        Hence only Infinity and NaN are special. 
     * @retval 
     */
    bool is_special() const 
    {
        uint32_t d32 = to_uint32();
        return (d32 & kExponentMask) == kExponentMask;
    }

    bool is_nan() const 
    {
        uint32_t d32 = to_uint32();
        return ((d32 & kExponentMask) == kExponentMask) &&
            ((d32 & kSignificandMask) != 0);
    }

    bool is_infinite() const 
    {
        uint32_t d32 = to_uint32();
        return ((d32 & kExponentMask) == kExponentMask) &&
            ((d32 & kSignificandMask) == 0);
    }

    int sign() const 
    {
        uint32_t d32 = to_uint32();
        return (d32 & kSignMask) == 0? 1: -1;
    }
    /** 
     * @brief Computes the two boundaries of this.
     * @note   The bigger boundary (m_plus) is normalized. The lower boundary 
     *         has the same exponent as m_plus.
     *         Precondition: the value encoded by this 
     *         Single must be greater than 0.
     * @param  out_m_minus: 
     * @param  out_m_plus: 
     * @retval None
     */
    void normalized_boundaries(SimpleFp* out_m_minus, SimpleFp* out_m_plus) const 
    {
        WALLE_ASSERT(value() > 0.0, "");
        SimpleFp v = this->to_simplefp();
        SimpleFp m_plus = SimpleFp::normalize(SimpleFp((v.f() << 1) + 1, v.e() - 1));
        SimpleFp m_minus;
        if (lower_boundary_is_closer()) {
            m_minus = SimpleFp((v.f() << 2) - 1, v.e() - 2);
        } else {
            m_minus = SimpleFp((v.f() << 1) - 1, v.e() - 1);
        }
        m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
        m_minus.set_e(m_plus.e());
        *out_m_plus = m_plus;
        *out_m_minus = m_minus;
    }

    /** 
     * @brief  
     * @note  Precondition: the value encoded by this Single must be greater or 
     *        equal than +0.0. 
     * @retval 
     */
    SimpleFp upper_boundary() const 
    {
        WALLE_ASSERT(sign() > 0, "");
        return SimpleFp(significand() * 2 + 1, exponent() - 1);
    }

    /** 
     * @brief The boundary is closer if the significand is of the form f == 2^p-1 then
     *        the lower boundary is closer.
     * @note denormals have the same exponent as the smallest normals.  
     * @retval Think of v = 1000e10 and v- = 9999e9. Then the boundary (== (v - v-)/2) is not just at a distance of 1e9 but 
     * at a distance of 1e8.
     * The only exception is for the smallest normal: the largest denormal is
     * at the same distance as its successor.
     */
    bool lower_boundary_is_closer() const 
    {
        bool physicalSignificandIsZero = ((to_uint32() & kSignificandMask) == 0);
        return physicalSignificandIsZero && (exponent() != kDenormalExponent);
    }
    float value() const 
    { 
        return uint32_to_float(_d32); 
    }

    static float infinity() 
    {
        return Single(kInfinity).value();
    }

    static float nan() 
    {
        return Single(kNaN).value();
    }
public:
    static const uint32_t kSignMask                = 0x80000000;
    static const uint32_t kExponentMask            = 0x7F800000;
    static const uint32_t kSignificandMask         = 0x007FFFFF;
    static const uint32_t kHiddenBit               = 0x00800000;
    static const int      kPhysicalSignificandSize = 23;  //!< Excludes the hidden bit.
    static const int      kSignificandSize         = 24;

private:
    WALLE_NON_COPYABLE(Single);
private:
    static const int      kExponentBias     = 0x7F + kPhysicalSignificandSize;
    static const int      kDenormalExponent = -kExponentBias + 1;
    static const int      kMaxExponent      = 0xFF - kExponentBias;
    static const uint32_t kInfinity         = 0x7F800000;
    static const uint32_t kNaN              = 0x7FC00000;

    const uint32_t _d32;
};

} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_IEEE_H_