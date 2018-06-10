#include <walle/format/detail/simple_fp.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <google/gtest/gtest.h>
#include <walle/math/math.h>

using namespace walle::format_detail;

TEST(IEEE, Uint64Conversions)
{
    uint64_t ordered = WALLE_UINT64_2PART_C(0x01234567, 89ABCDEF);
    EXPECT_EQ(3512700564088504e-318, Double(ordered).value());

    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    EXPECT_EQ(5e-324, Double(min_double64).value());

    uint64_t max_double64 = WALLE_UINT64_2PART_C(0x7fefffff, ffffffff);
    EXPECT_EQ(1.7976931348623157e308, Double(max_double64).value());
}

TEST(IEEE, Uint32Conversions)
{
    uint32_t ordered = 0x01234567;
    EXPECT_EQ(2.9988165487136453e-38f, Single(ordered).value());

    uint32_t min_float32 = 0x00000001;
    EXPECT_EQ(1.4e-45f, Single(min_float32).value());

    uint32_t max_float32 = 0x7f7fffff;
    EXPECT_EQ(3.4028234e38f, Single(max_float32).value());
}

TEST(IEEE, Double_Simeple_fp) {
    uint64_t ordered = WALLE_UINT64_2PART_C(0x01234567, 89ABCDEF);
    SimpleFp diy_fp = Double(ordered).to_simplefp();
    EXPECT_EQ(0x12 - 0x3FF - 52, diy_fp.e());
    // The 52 mantissa bits, plus the implicit 1 in bit 52 as a UINT64.
    EXPECT_EQ(WALLE_UINT64_2PART_C(0x00134567, 89ABCDEF),  diy_fp.f());  // NOLINT

    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    diy_fp = Double(min_double64).to_simplefp();
    EXPECT_EQ(-0x3FF - 52 + 1, diy_fp.e());
    // This is a denormal; so no hidden bit.
    EXPECT_EQ(1, diy_fp.f());  // NOLINT

    uint64_t max_double64 = WALLE_UINT64_2PART_C(0x7fefffff, ffffffff);
    diy_fp = Double(max_double64).to_simplefp();
    EXPECT_EQ(0x7FE - 0x3FF - 52, diy_fp.e());
    EXPECT_EQ(WALLE_UINT64_2PART_C(0x001fffff, ffffffff) , diy_fp.f());  // NOLINT
}

TEST(IEEE, Single_Simeple_fp) {
    uint32_t ordered = 0x01234567;
    SimpleFp diy_fp = Single(ordered).to_simplefp();
    EXPECT_EQ(0x2 - 0x7F - 23, diy_fp.e());
    // The 23 mantissa bits, plus the implicit 1 in bit 24 as a uint32_t.
    EXPECT_EQ(0xA34567, diy_fp.f());

    uint32_t min_float32 = 0x00000001;
    diy_fp = Single(min_float32).to_simplefp();
    EXPECT_EQ(-0x7F - 23 + 1, diy_fp.e());
    // This is a denormal; so no hidden bit.
    EXPECT_EQ(1, diy_fp.f());

    uint32_t max_float32 = 0x7f7fffff;
    diy_fp = Single(max_float32).to_simplefp();
    EXPECT_EQ(0xFE - 0x7F - 23, diy_fp.e());
    EXPECT_EQ(0x00ffffff, diy_fp.f());
}

TEST(IEEE, to_normalized_simplefp) {
    uint64_t ordered = WALLE_UINT64_2PART_C(0x01234567, 89ABCDEF);
    SimpleFp diy_fp = Double(ordered).to_normalized_simplefp();
    EXPECT_EQ(0x12 - 0x3FF - 52 - 11, diy_fp.e());
    EXPECT_EQ((WALLE_UINT64_2PART_C(0x00134567, 89ABCDEF) << 11), diy_fp.f());  // NOLINT

    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    diy_fp = Double(min_double64).to_normalized_simplefp();
    EXPECT_EQ(-0x3FF - 52 + 1 - 63, diy_fp.e());
    // This is a denormal; so no hidden bit.
    EXPECT_EQ(WALLE_UINT64_2PART_C(0x80000000, 00000000), diy_fp.f());  // NOLINT

    uint64_t max_double64 = WALLE_UINT64_2PART_C(0x7fefffff, ffffffff);
    diy_fp = Double(max_double64).to_normalized_simplefp();
    EXPECT_EQ(0x7FE - 0x3FF - 52 - 11, diy_fp.e());
    EXPECT_EQ((WALLE_UINT64_2PART_C(0x001fffff, ffffffff) << 11), diy_fp.f());  // NOLINT
}

TEST(IEEE, Double_is_denormal) {
    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    EXPECT_EQ(Double(min_double64).is_denormal(), true);
    uint64_t bits = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    EXPECT_EQ(Double(bits).is_denormal(), true);
    bits = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    EXPECT_EQ(!Double(bits).is_denormal(), true);
}

TEST(IEEE, Single_is_denormal) {
    uint32_t min_float32 = 0x00000001;
    EXPECT_EQ(Single(min_float32).is_denormal(), true);
    uint32_t bits = 0x007FFFFF;
    EXPECT_EQ(Single(bits).is_denormal(), true);
    bits = 0x00800000;
    EXPECT_EQ(!Single(bits).is_denormal(), true);
}

TEST(IEEE, Double_is_special) {
    EXPECT_EQ(Double(Double::infinity()).is_special(), true);
    EXPECT_EQ(Double(-Double::infinity()).is_special(), true);
    EXPECT_EQ(Double(Double::nan()).is_special(), true);
    uint64_t bits = WALLE_UINT64_2PART_C(0xFFF12345, 00000000);
    EXPECT_EQ(Double(bits).is_special(), true);
    // Denormals are not special:
    EXPECT_EQ(!Double(5e-324).is_special(), true);
    EXPECT_EQ(!Double(-5e-324).is_special(), true);
    // And some random numbers:
    EXPECT_EQ(!Double(0.0).is_special(), true);
    EXPECT_EQ(!Double(-0.0).is_special(), true);
    EXPECT_EQ(!Double(1.0).is_special(), true);
    EXPECT_EQ(!Double(-1.0).is_special(), true);
    EXPECT_EQ(!Double(1000000.0).is_special(), true);
    EXPECT_EQ(!Double(-1000000.0).is_special(), true);
    EXPECT_EQ(!Double(1e23).is_special(), true);
    EXPECT_EQ(!Double(-1e23).is_special(), true);
    EXPECT_EQ(!Double(1.7976931348623157e308).is_special(), true);
    EXPECT_EQ(!Double(-1.7976931348623157e308).is_special(), true);
}

TEST(IEEE, Single_is_special) {
    EXPECT_EQ(Single(Single::infinity()).is_special(), true);
    EXPECT_EQ(Single(-Single::infinity()).is_special(), true);
    EXPECT_EQ(Single(Single::nan()).is_special(), true);
    uint32_t bits = 0xFFF12345;
    EXPECT_EQ(Single(bits).is_special(), true);
    // Denormals are not special:
    EXPECT_EQ(!Single(1.4e-45f).is_special(), true);
    EXPECT_EQ(!Single(-1.4e-45f).is_special(), true);
    // And some random numbers:
    EXPECT_EQ(!Single(0.0f).is_special(), true);
    EXPECT_EQ(!Single(-0.0f).is_special(), true);
    EXPECT_EQ(!Single(1.0f).is_special(), true);
    EXPECT_EQ(!Single(-1.0f).is_special(), true);
    EXPECT_EQ(!Single(1000000.0f).is_special(), true);
    EXPECT_EQ(!Single(-1000000.0f).is_special(), true);
    EXPECT_EQ(!Single(1e23f).is_special(), true);
    EXPECT_EQ(!Single(-1e23f).is_special(), true);
    EXPECT_EQ(!Single(1.18e-38f).is_special(), true);
    EXPECT_EQ(!Single(-1.18e-38f).is_special(), true);
}

TEST(IEEE, Double_is_infinite) {
    EXPECT_EQ(Double(Double::infinity()).is_infinite(), true);
    EXPECT_EQ(Double(-Double::infinity()).is_infinite(), true);
    EXPECT_EQ(!Double(Double::nan()).is_infinite(), true);
    EXPECT_EQ(!Double(0.0).is_infinite(), true);
    EXPECT_EQ(!Double(-0.0).is_infinite(), true);
    EXPECT_EQ(!Double(1.0).is_infinite(), true);
    EXPECT_EQ(!Double(-1.0).is_infinite(), true);
    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    EXPECT_EQ(!Double(min_double64).is_infinite(), true);
}


TEST(IEEE, Single_is_infinite) {
    EXPECT_EQ(Single(Single::infinity()).is_infinite(), true);
    EXPECT_EQ(Single(-Single::infinity()).is_infinite(), true);
    EXPECT_EQ(!Single(Single::nan()).is_infinite(), true);
    EXPECT_EQ(!Single(0.0f).is_infinite(), true);
    EXPECT_EQ(!Single(-0.0f).is_infinite(), true);
    EXPECT_EQ(!Single(1.0f).is_infinite(), true);
    EXPECT_EQ(!Single(-1.0f).is_infinite(), true);
    uint32_t min_float32 = 0x00000001;
    EXPECT_EQ(!Single(min_float32).is_infinite(), true);
}

TEST(IEEE, Double_is_nan) {
    EXPECT_EQ(Double(Double::nan()).is_nan(), true);
    uint64_t other_nan = WALLE_UINT64_2PART_C(0xFFFFFFFF, 00000001);
    EXPECT_EQ(Double(other_nan).is_nan(), true);
    EXPECT_EQ(!Double(Double::infinity()).is_nan(), true);
    EXPECT_EQ(!Double(-Double::infinity()).is_nan(), true);
    EXPECT_EQ(!Double(0.0).is_nan(), true);
    EXPECT_EQ(!Double(-0.0).is_nan(), true);
    EXPECT_EQ(!Double(1.0).is_nan(), true);
    EXPECT_EQ(!Double(-1.0).is_nan(), true);
    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    EXPECT_EQ(!Double(min_double64).is_nan(), true);
}


TEST(IEEE, Single_is_nan) {
    EXPECT_EQ(Single(Single::nan()).is_nan(), true);
    uint32_t other_nan = 0xFFFFF001;
    EXPECT_EQ(Single(other_nan).is_nan(), true);
    EXPECT_EQ(!Single(Single::infinity()).is_nan(), true);
    EXPECT_EQ(!Single(-Single::infinity()).is_nan(), true);
    EXPECT_EQ(!Single(0.0f).is_nan(), true);
    EXPECT_EQ(!Single(-0.0f).is_nan(), true);
    EXPECT_EQ(!Single(1.0f).is_nan(), true);
    EXPECT_EQ(!Single(-1.0f).is_nan(), true);
    uint32_t min_float32 = 0x00000001;
    EXPECT_EQ(!Single(min_float32).is_nan(), true);
}

TEST(IEEE, Double_sign) {
    EXPECT_EQ(1, Double(1.0).sign());
    EXPECT_EQ(1, Double(Double::infinity()).sign());
    EXPECT_EQ(-1, Double(-Double::infinity()).sign());
    EXPECT_EQ(1, Double(0.0).sign());
    EXPECT_EQ(-1, Double(-0.0).sign());
    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    EXPECT_EQ(1, Double(min_double64).sign());
}


TEST(IEEE, Single_sign) {
    EXPECT_EQ(1, Single(1.0f).sign());
    EXPECT_EQ(1, Single(Single::infinity()).sign());
    EXPECT_EQ(-1, Single(-Single::infinity()).sign());
    EXPECT_EQ(1, Single(0.0f).sign());
    EXPECT_EQ(-1, Single(-0.0f).sign());
    uint32_t min_float32 = 0x00000001;
    EXPECT_EQ(1, Single(min_float32).sign());
}

TEST(IEEE, Double_normalized_boundaries) {
    SimpleFp boundary_plus;
    SimpleFp boundary_minus;
    SimpleFp diy_fp = Double(1.5).to_normalized_simplefp();
    Double(1.5).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // 1.5 does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((1 << 10) , diy_fp.f() - boundary_minus.f());  // NOLINT

    diy_fp = Double(1.0).to_normalized_simplefp();
    Double(1.0).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // 1.0 does have a significand of the form 2^p (for some p).
    // Therefore its lower boundary is twice as close as the upper boundary.
    EXPECT_EQ(boundary_plus.f() - diy_fp.f() > diy_fp.f() - boundary_minus.f(), true);
    EXPECT_EQ((1 << 9) , diy_fp.f() - boundary_minus.f());  // NOLINT
    EXPECT_EQ((1 << 10) , boundary_plus.f() - diy_fp.f());  // NOLINT

    uint64_t min_double64 = WALLE_UINT64_2PART_C(0x00000000, 00000001);
    diy_fp = Double(min_double64).to_normalized_simplefp();
    Double(min_double64).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // min-value does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    // Denormals have their boundaries much closer.
    EXPECT_EQ((static_cast<uint64_t>(1) << 62) ,
            diy_fp.f() - boundary_minus.f());  // NOLINT

    uint64_t smallest_normal64 = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    diy_fp = Double(smallest_normal64).to_normalized_simplefp();
    Double(smallest_normal64).normalized_boundaries(&boundary_minus,
                                                    &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // Even though the significand is of the form 2^p (for some p), its boundaries
    // are at the same distance. (This is the only exception).
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((1 << 10) , diy_fp.f() - boundary_minus.f());  // NOLINT

    uint64_t largest_denormal64 = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    diy_fp = Double(largest_denormal64).to_normalized_simplefp();
    Double(largest_denormal64).normalized_boundaries(&boundary_minus,
                                                    &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    EXPECT_EQ(diy_fp.f() - boundary_minus.f(), boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((1 << 11), diy_fp.f() - boundary_minus.f());  // NOLINT

    uint64_t max_double64 = WALLE_UINT64_2PART_C(0x7fefffff, ffffffff);
    diy_fp = Double(max_double64).to_normalized_simplefp();
    Double(max_double64).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // max-value does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((1 << 10) , diy_fp.f() - boundary_minus.f());  // NOLINT
}


TEST(IEEE, Single_normalized_boundaries) {
    uint64_t kOne64 = 1;
    SimpleFp boundary_plus;
    SimpleFp boundary_minus;
    SimpleFp diy_fp = Single(1.5f).to_simplefp();
    diy_fp.normalize();
    Single(1.5f).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // 1.5 does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    // Normalization shifts the significand by 8 bits. Add 32 bits for the bigger
    // data-type, and remove 1 because boundaries are at half a ULP.
    EXPECT_EQ((kOne64 << 39) , diy_fp.f() - boundary_minus.f());

    diy_fp = Single(1.0f).to_simplefp();
    diy_fp.normalize();
    Single(1.0f).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // 1.0 does have a significand of the form 2^p (for some p).
    // Therefore its lower boundary is twice as close as the upper boundary.
    EXPECT_EQ(boundary_plus.f() - diy_fp.f() > diy_fp.f() - boundary_minus.f(), true);
    EXPECT_EQ((kOne64 << 38) , diy_fp.f() - boundary_minus.f());  // NOLINT
    EXPECT_EQ((kOne64 << 39) , boundary_plus.f() - diy_fp.f());  // NOLINT

    uint32_t min_float32 = 0x00000001;
    diy_fp = Single(min_float32).to_simplefp();
    diy_fp.normalize();
    Single(min_float32).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // min-value does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    // Denormals have their boundaries much closer.
    EXPECT_EQ((kOne64 << 62) , diy_fp.f() - boundary_minus.f());  // NOLINT

    uint32_t smallest_normal32 = 0x00800000;
    diy_fp = Single(smallest_normal32).to_simplefp();
    diy_fp.normalize();
    Single(smallest_normal32).normalized_boundaries(&boundary_minus,
                                                    &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // Even though the significand is of the form 2^p (for some p), its boundaries
    // are at the same distance. (This is the only exception).
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((kOne64 << 39) , diy_fp.f() - boundary_minus.f());  // NOLINT

    uint32_t largest_denormal32 = 0x007FFFFF;
    diy_fp = Single(largest_denormal32).to_simplefp();
    diy_fp.normalize();
    Single(largest_denormal32).normalized_boundaries(&boundary_minus,
                                                    &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((kOne64 << 40) , diy_fp.f() - boundary_minus.f());  // NOLINT

    uint32_t max_float32 = 0x7f7fffff;
    diy_fp = Single(max_float32).to_simplefp();
    diy_fp.normalize();
    Single(max_float32).normalized_boundaries(&boundary_minus, &boundary_plus);
    EXPECT_EQ(diy_fp.e(), boundary_minus.e());
    EXPECT_EQ(diy_fp.e(), boundary_plus.e());
    // max-value does not have a significand of the form 2^p (for some p).
    // Therefore its boundaries are at the same distance.
    EXPECT_EQ(diy_fp.f() - boundary_minus.f() , boundary_plus.f() - diy_fp.f());
    EXPECT_EQ((kOne64 << 39) , diy_fp.f() - boundary_minus.f());  // NOLINT
}

TEST(IEEE, next_double) {
    EXPECT_EQ(4e-324, Double(0.0).next_double());
    EXPECT_EQ(0.0, Double(-0.0).next_double());
    EXPECT_EQ(-0.0, Double(-4e-324).next_double());
    EXPECT_EQ(Double(Double(-0.0).next_double()).sign() > 0, true);
    EXPECT_EQ(Double(Double(-4e-324).next_double()).sign() < 0, true);
    Double d0(-4e-324);
    Double d1(d0.next_double());
    Double d2(d1.next_double());
    EXPECT_EQ(-0.0, d1.value());
    EXPECT_EQ(d1.sign() < 0, true);
    EXPECT_EQ(0.0, d2.value());
    EXPECT_EQ(d2.sign() > 0, true);
    EXPECT_EQ(4e-324, d2.next_double());
    EXPECT_EQ(-1.7976931348623157e308, Double(-Double::infinity()).next_double());
    EXPECT_EQ(Double::infinity(),
           Double(WALLE_UINT64_2PART_C(0x7fefffff, ffffffff)).next_double());
}


TEST(IEEE, previous_double) {
  EXPECT_EQ(0.0, Double(4e-324).previous_double());
  EXPECT_EQ(-0.0, Double(0.0).previous_double());
  EXPECT_EQ(Double(Double(0.0).previous_double()).sign() < 0, true);
  EXPECT_EQ(-4e-324, Double(-0.0).previous_double());
  Double d0(4e-324);
  Double d1(d0.previous_double());
  Double d2(d1.previous_double());
  EXPECT_EQ(0.0, d1.value());
  EXPECT_EQ(d1.sign() > 0, true);
  EXPECT_EQ(-0.0, d2.value());
  EXPECT_EQ(d2.sign() < 0, true);
  EXPECT_EQ(-4e-324, d2.previous_double());
  EXPECT_EQ(1.7976931348623157e308, Double(Double::infinity()).previous_double());
  EXPECT_EQ(-Double::infinity(),
           Double(WALLE_UINT64_2PART_C(0xffefffff, ffffffff)).previous_double());
}