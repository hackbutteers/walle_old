#include <stdlib.h>
#include "gay_fixed.h"
#include "gay_precision.h"
#include "gay_shortest.h"
#include "gay_shortest_single.h"

#include <walle/format/detail/double_conversion.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/strings/string_builder.h>
#include <google/gtest/gtest.h>
#include <cstring>
#include <walle/math/math.h>

// DoubleToString is already tested in test-dtoa.cc.

WALLE_DISABLE_WARING(-Wunused-function);
#define CHECK(condition) CheckHelper(__FILE__, __LINE__, #condition, condition)

static inline void CheckHelper(const char* file,
                               int line,
                               const char* source,
                               bool condition) 
{
    if (!condition) {
        printf("%s:%d:\n CHECK(%s) failed\n", file, line, source);
        EXPECT_EQ(1,0);
    }
    EXPECT_EQ(1,1);
}

#define CHECK_EQ(a, b) CheckEqualsHelper(__FILE__, __LINE__, #a, a, #b, b)

static inline void CheckEqualsHelper(const char* file, int line,
                                     const char* expected_source,
                                     const char* expected,
                                     const char* value_source,
                                     const char* value) 
{
    if ((expected == NULL && value != NULL) ||
        (expected != NULL && value == NULL)) {
            EXPECT_EQ(1,0);
    }

    if ((expected != NULL && value != NULL && strcmp(expected, value) != 0)) {
        printf("%s:%d:\n CHECK_EQ(%s, %s) failed\n"
            "#  Expected: %s\n"
            "#  Found:    %s\n",
            file, line, expected_source, value_source, expected, value);
        EXPECT_EQ(1,0);
    }
    EXPECT_EQ(1,1);
}


static inline void CheckEqualsHelper(const char* file, int line,
                                     const char* expected_source,
                                     int expected,
                                     const char* value_source,
                                     int value) {
    if (expected != value) {
        printf("%s:%d:\n CHECK_EQ(%s, %s) failed\n"
            "#  Expected: %d\n"
            "#  Found:    %d\n",
            file, line, expected_source, value_source, expected, value);
        EXPECT_EQ(1,0);
    }
    EXPECT_EQ(1,1);
}

static inline void CheckEqualsHelper(const char* file, int line,
                                     const char* expected_source,
                                     double expected,
                                     const char* value_source,
                                     double value) {
    // If expected and value are NaNs then expected != value.
    if (expected != value && (expected == expected || value == value)) {
            printf("%s:%d:\n CHECK_EQ(%s, %s) failed\n"
                "#  Expected: %.30e\n"
                "#  Found:    %.30e\n",
                file, line, expected_source, value_source, expected, value);
        EXPECT_EQ(1,0);
    }
    EXPECT_EQ(1,1);
}

using namespace walle::format_detail;
using walle::StringBuilder;

TEST(Conversion, DoubleToShortest) 
{
  const int kBufferSize = 128;
  char buffer[kBufferSize];
  StringBuilder builder(buffer, kBufferSize);
  int flags = DoubleToStringConverter::eUniqueZero |
      DoubleToStringConverter::eEmitPositiveExponentSign;
  DoubleToStringConverter dc(flags, NULL, NULL, 'e', -6, 21, 0, 0);

  CHECK_EQ(dc.to_shortest(0.0, &builder), true);
  CHECK_EQ("0", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(12345.0, &builder), true);
  CHECK_EQ("12345", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(12345e23, &builder), true);
  CHECK_EQ("1.2345e+27", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(1e21, &builder), true);
  CHECK_EQ("1e+21", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(1e20, &builder), true);
  CHECK_EQ("100000000000000000000", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(111111111111111111111.0, &builder), true);
  CHECK_EQ("111111111111111110000", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(1111111111111111111111.0, &builder), true);
  CHECK_EQ("1.1111111111111111e+21", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(11111111111111111111111.0, &builder), true);
  CHECK_EQ("1.1111111111111111e+22", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(-0.00001, &builder), true);
  CHECK_EQ("-0.00001", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(-0.000001, &builder), true);
  CHECK_EQ("-0.000001", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(-0.0000001, &builder), true);
  CHECK_EQ("-1e-7", builder.finalize());

  builder.reset();
  CHECK_EQ(dc.to_shortest(-0.0, &builder), true);
  CHECK_EQ("0", builder.finalize());

  flags = DoubleToStringConverter::eNoFlags;
  DoubleToStringConverter dc2(flags, NULL, NULL, 'e', -1, 1, 0, 0);
  builder.reset();
  CHECK_EQ(dc2.to_shortest(0.1, &builder), true);
  CHECK_EQ("0.1", builder.finalize());

  builder.reset();
  CHECK_EQ(dc2.to_shortest(0.01, &builder), true);
  CHECK_EQ("1e-2", builder.finalize());

  builder.reset();
  CHECK_EQ(dc2.to_shortest(1.0, &builder), true);
  CHECK_EQ("1", builder.finalize());

  builder.reset();
    CHECK_EQ(dc2.to_shortest(10.0, &builder), true);
    CHECK_EQ("1e1", builder.finalize());

    builder.reset();
    CHECK_EQ(dc2.to_shortest(-0.0, &builder), true);
    CHECK_EQ("-0", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc3(flags, NULL, NULL, 'E', -5, 5, 0, 0);

    builder.reset();
    CHECK_EQ(dc3.to_shortest(0.1, &builder), true);
    CHECK_EQ("0.1", builder.finalize());

    builder.reset();
    CHECK_EQ(dc3.to_shortest(1.0, &builder), true);
    CHECK_EQ("1.0", builder.finalize());

    builder.reset();
    CHECK_EQ(dc3.to_shortest(10000.0, &builder), true);
    CHECK_EQ("10000.0", builder.finalize());

    builder.reset();
    CHECK_EQ(dc3.to_shortest(100000.0, &builder),true);
    CHECK_EQ("1E5", builder.finalize());

    // Test the examples in the comments of to_shortest.
    flags = DoubleToStringConverter::eEmitPositiveExponentSign;
    DoubleToStringConverter dc4(flags, NULL, NULL, 'e', -6, 21, 0, 0);

    builder.reset();
    CHECK_EQ(dc4.to_shortest(0.000001, &builder),true);
    CHECK_EQ("0.000001", builder.finalize());

    builder.reset();
    CHECK_EQ(dc4.to_shortest(0.0000001, &builder), true);
    CHECK_EQ("1e-7", builder.finalize());

    builder.reset();
    CHECK_EQ(dc4.to_shortest(111111111111111111111.0, &builder), true);
    CHECK_EQ("111111111111111110000", builder.finalize());

    builder.reset();
    CHECK_EQ(dc4.to_shortest(100000000000000000000.0, &builder), true);
    CHECK_EQ("100000000000000000000", builder.finalize());

    builder.reset();
    CHECK_EQ(dc4.to_shortest(1111111111111111111111.0, &builder), true);
    CHECK_EQ("1.1111111111111111e+21", builder.finalize());

    // Test special value handling.
    DoubleToStringConverter dc5(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK_EQ(!dc5.to_shortest(Double::infinity(), &builder), true);

    builder.reset();
    CHECK_EQ(!dc5.to_shortest(-Double::infinity(), &builder), true);

    builder.reset();
    CHECK_EQ(!dc5.to_shortest(Double::nan(), &builder), true);

    builder.reset();
    CHECK_EQ(!dc5.to_shortest(-Double::nan(), &builder), true);

    DoubleToStringConverter dc6(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK_EQ(dc6.to_shortest(Double::infinity(), &builder), true);
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK_EQ(dc6.to_shortest(-Double::infinity(), &builder),true);
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK_EQ(dc6.to_shortest(Double::nan(), &builder),true);
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK_EQ(dc6.to_shortest(-Double::nan(), &builder),true);
    CHECK_EQ("NaN", builder.finalize());
}


TEST(Conversion, Doubleto_shortest_single) {
    const int kBufferSize = 128;
    char buffer[kBufferSize];
    StringBuilder builder(buffer, kBufferSize);
    int flags = DoubleToStringConverter::eUniqueZero |
        DoubleToStringConverter::eEmitPositiveExponentSign;
    DoubleToStringConverter dc(flags, NULL, NULL, 'e', -6, 21, 0, 0);

    CHECK(dc.to_shortest_single(0.0f, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(12345.0f, &builder));
    CHECK_EQ("12345", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(12345e23f, &builder));
    CHECK_EQ("1.2345e+27", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(1e21f, &builder));
    CHECK_EQ("1e+21", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(1e20f, &builder));
    CHECK_EQ("100000000000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(111111111111111111111.0f, &builder));
    CHECK_EQ("111111110000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(1111111111111111111111.0f, &builder));
    CHECK_EQ("1.11111114e+21", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(11111111111111111111111.0f, &builder));
    CHECK_EQ("1.1111111e+22", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(-0.00001f, &builder));
    CHECK_EQ("-0.00001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(-0.000001f, &builder));
    CHECK_EQ("-0.000001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(-0.0000001f, &builder));
    CHECK_EQ("-1e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest_single(-0.0f, &builder));
    CHECK_EQ("0", builder.finalize());

    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc2(flags, NULL, NULL, 'e', -1, 1, 0, 0);
    builder.reset();
    CHECK(dc2.to_shortest_single(0.1f, &builder));
    CHECK_EQ("0.1", builder.finalize());

    builder.reset();
    CHECK(dc2.to_shortest_single(0.01f, &builder));
    CHECK_EQ("1e-2", builder.finalize());

    builder.reset();
    CHECK(dc2.to_shortest_single(1.0f, &builder));
    CHECK_EQ("1", builder.finalize());

    builder.reset();
    CHECK(dc2.to_shortest_single(10.0f, &builder));
    CHECK_EQ("1e1", builder.finalize());

    builder.reset();
    CHECK(dc2.to_shortest_single(-0.0f, &builder));
    CHECK_EQ("-0", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc3(flags, NULL, NULL, 'E', -5, 5, 0, 0);

    builder.reset();
    CHECK(dc3.to_shortest_single(0.1f, &builder));
    CHECK_EQ("0.1", builder.finalize());

    builder.reset();
    CHECK(dc3.to_shortest_single(1.0f, &builder));
    CHECK_EQ("1.0", builder.finalize());

    builder.reset();
    CHECK(dc3.to_shortest_single(10000.0f, &builder));
    CHECK_EQ("10000.0", builder.finalize());

    builder.reset();
    CHECK(dc3.to_shortest_single(100000.0f, &builder));
    CHECK_EQ("1E5", builder.finalize());

    // Test the examples in the comments of to_shortest_single.
    flags = DoubleToStringConverter::eEmitPositiveExponentSign;
    DoubleToStringConverter dc4(flags, NULL, NULL, 'e', -6, 21, 0, 0);

    builder.reset();
    CHECK(dc4.to_shortest_single(0.000001f, &builder));
    CHECK_EQ("0.000001", builder.finalize());

    builder.reset();
    CHECK(dc4.to_shortest_single(0.0000001f, &builder));
    CHECK_EQ("1e-7", builder.finalize());

    builder.reset();
    CHECK(dc4.to_shortest_single(111111111111111111111.0f, &builder));
    CHECK_EQ("111111110000000000000", builder.finalize());

    builder.reset();
    CHECK(dc4.to_shortest_single(100000000000000000000.0f, &builder));
    CHECK_EQ("100000000000000000000", builder.finalize());

    builder.reset();
    CHECK(dc4.to_shortest_single(1111111111111111111111.0f, &builder));
    CHECK_EQ("1.11111114e+21", builder.finalize());

    // Test special value handling.
    DoubleToStringConverter dc5(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(!dc5.to_shortest_single(Single::infinity(), &builder));

    builder.reset();
    CHECK(!dc5.to_shortest_single(-Single::infinity(), &builder));

    builder.reset();
    CHECK(!dc5.to_shortest_single(Single::nan(), &builder));

    builder.reset();
    CHECK(!dc5.to_shortest_single(-Single::nan(), &builder));

    DoubleToStringConverter dc6(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc6.to_shortest_single(Single::infinity(), &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc6.to_shortest_single(-Single::infinity(), &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc6.to_shortest_single(Single::nan(), &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc6.to_shortest_single(-Single::nan(), &builder));
    CHECK_EQ("NaN", builder.finalize());
}


TEST(Conversion, Doubleto_fixed) 
{
    const int kBufferSize = 128;
    char buffer[kBufferSize];
    StringBuilder builder(buffer, kBufferSize);
    int flags = DoubleToStringConverter::eEmitPositiveExponentSign |
        DoubleToStringConverter::eUniqueZero;
    DoubleToStringConverter dc(flags, "Infinity", "NaN", 'e',
                                0, 0, 0, 0);  // Padding zeroes.

    CHECK(dc.to_fixed(0.0, 0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.0, 0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.0, 1, &builder));
    CHECK_EQ("0.0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.0, 1, &builder));
    CHECK_EQ("0.0", builder.finalize());

    WALLE_ASSERT(DoubleToStringConverter::kMaxFixedDigitsBeforePoint == 60, "");
    WALLE_ASSERT(DoubleToStringConverter::kMaxFixedDigitsAfterPoint == 60, "");
    builder.reset();
    CHECK(dc.to_fixed(
        0.0, DoubleToStringConverter::kMaxFixedDigitsAfterPoint, &builder));
    CHECK_EQ("0.000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(
        9e59, DoubleToStringConverter::kMaxFixedDigitsAfterPoint, &builder));
    CHECK_EQ("899999999999999918767229449717619953810131273674690656206848."
            "000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(
        -9e59, DoubleToStringConverter::kMaxFixedDigitsAfterPoint, &builder));
    CHECK_EQ("-899999999999999918767229449717619953810131273674690656206848."
            "000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    builder.reset();
    CHECK(!dc.to_fixed(
        1e60, DoubleToStringConverter::kMaxFixedDigitsAfterPoint, &builder));
    CHECK_EQ(0, builder.position());

    builder.reset();
    CHECK(!dc.to_fixed(
        9e59, DoubleToStringConverter::kMaxFixedDigitsAfterPoint + 1, &builder));
    CHECK_EQ(0, builder.position());

    builder.reset();
    CHECK(dc.to_fixed(3.0, 0, &builder));
    CHECK_EQ("3", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(3.23, 1, &builder));
    CHECK_EQ("3.2", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(3.23, 3, &builder));
    CHECK_EQ("3.230", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.0323, 2, &builder));
    CHECK_EQ("0.03", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.0373, 2, &builder));
    CHECK_EQ("0.04", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.0000373, 2, &builder));
    CHECK_EQ("0.00", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1.5, 0, &builder));
    CHECK_EQ("2", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(2.5, 0, &builder));
    CHECK_EQ("3", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(3.5, 0, &builder));
    CHECK_EQ("4", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.15, 1, &builder));
    CHECK_EQ("0.1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.25, 1, &builder));
    CHECK_EQ("0.3", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.35, 1, &builder));
    CHECK_EQ("0.3", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.45, 1, &builder));
    CHECK_EQ("0.5", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.55, 1, &builder));
    CHECK_EQ("0.6", builder.finalize());

    // Test positive/negative zeroes.
    int flags2 = DoubleToStringConverter::eEmitPositiveExponentSign;
    DoubleToStringConverter dc2(flags2, "Infinity", "NaN", 'e',
                                0, 0, 0, 0);  // Padding zeroes.
    builder.reset();
    CHECK(dc2.to_fixed(0.0, 1, &builder));
    CHECK_EQ("0.0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_fixed(-0.0, 1, &builder));
    CHECK_EQ("-0.0", builder.finalize());

    // Verify the trailing dot is emitted.
    int flags3 = DoubleToStringConverter::eEmitPositiveExponentSign |
        DoubleToStringConverter::eEmitTrailingDecimalpoint;
    DoubleToStringConverter dc3(flags3, "Infinity", "NaN", 'e',
                                0, 0, 0, 0);  // Padding zeroes.
    builder.reset();
    CHECK(dc3.to_fixed(0.0, 0, &builder));
    CHECK_EQ("0.", builder.finalize());

    builder.reset();
    CHECK(dc3.to_fixed(-0.0, 0, &builder));
    CHECK_EQ("-0.", builder.finalize());

    builder.reset();
    CHECK(dc3.to_fixed(1.0, 0, &builder));
    CHECK_EQ("1.", builder.finalize());

    builder.reset();
    CHECK(dc3.to_fixed(-1.0, 0, &builder));
    CHECK_EQ("-1.", builder.finalize());

    // Verify no trailing zero is emitted, even if the configuration is set.
    // The given parameter takes precedence.
    int flags4 = DoubleToStringConverter::eEmitPositiveExponentSign |
        DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc4(flags4, "Infinity", "NaN", 'e',
                                0, 0, 0, 0);  // Padding zeroes.
    builder.reset();
    CHECK(dc4.to_fixed(0.0, 0, &builder));
    CHECK_EQ("0.0", builder.finalize());

    builder.reset();
    CHECK(dc4.to_fixed(-0.0, 0, &builder));
    CHECK_EQ("-0.0", builder.finalize());

    builder.reset();
    CHECK(dc4.to_fixed(1.0, 0, &builder));
    CHECK_EQ("1.0", builder.finalize());

    builder.reset();
    CHECK(dc4.to_fixed(-1.0, 0, &builder));
    CHECK_EQ("-1.0", builder.finalize());

    // Test the examples in the comments of to_fixed.
    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc5(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc5.to_fixed(3.12, 1, &builder));
    CHECK_EQ("3.1", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(3.1415, 3, &builder));
    CHECK_EQ("3.142", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(1234.56789, 4, &builder));
    CHECK_EQ("1234.5679", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(1.23, 5, &builder));
    CHECK_EQ("1.23000", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(0.1, 4, &builder));
    CHECK_EQ("0.1000", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(1e30, 2, &builder));
    CHECK_EQ("1000000000000000019884624838656.00", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(0.1, 30, &builder));
    CHECK_EQ("0.100000000000000005551115123126", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(0.1, 17, &builder));
    CHECK_EQ("0.10000000000000001", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(123.45, 0, &builder));
    CHECK_EQ("123", builder.finalize());

    builder.reset();
    CHECK(dc5.to_fixed(0.678, 0, &builder));
    CHECK_EQ("1", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint;
    DoubleToStringConverter dc6(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc6.to_fixed(123.45, 0, &builder));
    CHECK_EQ("123.", builder.finalize());

    builder.reset();
    CHECK(dc6.to_fixed(0.678, 0, &builder));
    CHECK_EQ("1.", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc7(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc7.to_fixed(123.45, 0, &builder));
    CHECK_EQ("123.0", builder.finalize());

    builder.reset();
    CHECK(dc7.to_fixed(0.678, 0, &builder));
    CHECK_EQ("1.0", builder.finalize());

    // Test special value handling.
    DoubleToStringConverter dc8(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(!dc8.to_fixed(Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_fixed(-Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_fixed(Double::nan(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_fixed(-Double::nan(), 1, &builder));

    DoubleToStringConverter dc9(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc9.to_fixed(Double::infinity(), 1, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc9.to_fixed(-Double::infinity(), 1, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc9.to_fixed(Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc9.to_fixed(-Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());
}


TEST(Conversion, DoubleToExponential) {
    const int kBufferSize = 256;
    char buffer[kBufferSize];
    int flags = DoubleToStringConverter::eEmitPositiveExponentSign |
        DoubleToStringConverter::eUniqueZero;
    StringBuilder builder(buffer, kBufferSize);
    DoubleToStringConverter dc(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc.to_exponential(0.0, 5, &builder));
    CHECK_EQ("0.00000e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0, 0, &builder));
    CHECK_EQ("0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0, 1, &builder));
    CHECK_EQ("0.0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.123456, 5, &builder));
    CHECK_EQ("1.23456e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.2, 1, &builder));
    CHECK_EQ("1.2e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.0, 1, &builder));
    CHECK_EQ("0.0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0, 2, &builder));
    CHECK_EQ("0.00e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.0, 2, &builder));
    CHECK_EQ("0.00e+0", builder.finalize());

    WALLE_ASSERT(DoubleToStringConverter::kMaxExponentialDigits == 120, "");
    builder.reset();
    CHECK(dc.to_exponential(
        0.0, DoubleToStringConverter::kMaxExponentialDigits, &builder));
    CHECK_EQ("0.00000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000e+0",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(
        9e59, DoubleToStringConverter::kMaxExponentialDigits, &builder));
    CHECK_EQ("8.99999999999999918767229449717619953810131273674690656206848"
            "0000000000000000000000000000000000000000000000000000000000000e+59",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(
        -9e59, DoubleToStringConverter::kMaxExponentialDigits, &builder));
    CHECK_EQ("-8.99999999999999918767229449717619953810131273674690656206848"
            "0000000000000000000000000000000000000000000000000000000000000e+59",
            builder.finalize());

    const double max_double = 1.7976931348623157e308;
    builder.reset();
    CHECK(dc.to_exponential(
        max_double, DoubleToStringConverter::kMaxExponentialDigits, &builder));
    CHECK_EQ("1.79769313486231570814527423731704356798070567525844996598917"
            "4768031572607800285387605895586327668781715404589535143824642e+308",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.000001, 2, &builder));
    CHECK_EQ("1.00e-6", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0000001, 2, &builder));
    CHECK_EQ("1.00e-7", builder.finalize());

    // Test the examples in the comments of ToExponential.
    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc2(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc2.to_exponential(3.12, 1, &builder));
    CHECK_EQ("3.1e0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(5.0, 3, &builder));
    CHECK_EQ("5.000e0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(0.001, 2, &builder));
    CHECK_EQ("1.00e-3", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(3.1415, -1, &builder));
    CHECK_EQ("3.1415e0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(3.1415, 4, &builder));
    CHECK_EQ("3.1415e0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(3.1415, 3, &builder));
    CHECK_EQ("3.142e0", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(123456789000000, 3, &builder));
    CHECK_EQ("1.235e14", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(1000000000000000019884624838656.0, -1, &builder));
    CHECK_EQ("1e30", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(1000000000000000019884624838656.0, 32, &builder));
    CHECK_EQ("1.00000000000000001988462483865600e30", builder.finalize());

    builder.reset();
    CHECK(dc2.to_exponential(1234, 0, &builder));
    CHECK_EQ("1e3", builder.finalize());

    // Test special value handling.
    DoubleToStringConverter dc3(flags, NULL, NULL, 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(!dc3.to_exponential(Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc3.to_exponential(-Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc3.to_exponential(Double::nan(), 1, &builder));

    builder.reset();
    CHECK(!dc3.to_exponential(-Double::nan(), 1, &builder));

    DoubleToStringConverter dc4(flags, "Infinity", "NaN", 'e', 0, 0, 0, 0);

    builder.reset();
    CHECK(dc4.to_exponential(Double::infinity(), 1, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc4.to_exponential(-Double::infinity(), 1, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc4.to_exponential(Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc4.to_exponential(-Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());
}


TEST(Conversion, DoubleToPrecision) 
{
    const int kBufferSize = 256;
    char buffer[kBufferSize];
    int flags = DoubleToStringConverter::eEmitPositiveExponentSign |
        DoubleToStringConverter::eUniqueZero;
    StringBuilder builder(buffer, kBufferSize);
    DoubleToStringConverter dc(flags, "Infinity", "NaN", 'e',
                                0, 0,   // Padding zeroes for shortest mode.
                                6, 0);  // Padding zeroes for precision mode.

    WALLE_ASSERT(DoubleToStringConverter::kMinPrecisionDigits == 1, "");
    CHECK(dc.to_precision(0.0, 1, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-0.0, 1, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(0.0, 2, &builder));
    CHECK_EQ("0.0", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-0.0, 2, &builder));
    CHECK_EQ("0.0", builder.finalize());

    WALLE_ASSERT(DoubleToStringConverter::kMaxPrecisionDigits == 120, "");
    builder.reset();
    CHECK(dc.to_precision(
        0.0, DoubleToStringConverter::kMaxPrecisionDigits, &builder));
    CHECK_EQ("0.00000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(
        9e59, DoubleToStringConverter::kMaxPrecisionDigits, &builder));
    CHECK_EQ("899999999999999918767229449717619953810131273674690656206848."
            "000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(
        -9e59, DoubleToStringConverter::kMaxPrecisionDigits, &builder));
    CHECK_EQ("-899999999999999918767229449717619953810131273674690656206848."
            "000000000000000000000000000000000000000000000000000000000000",
            builder.finalize());

    const double max_double = 1.7976931348623157e308;
    builder.reset();
    CHECK(dc.to_precision(
        max_double, DoubleToStringConverter::kMaxPrecisionDigits, &builder));
    CHECK_EQ("1.79769313486231570814527423731704356798070567525844996598917"
            "476803157260780028538760589558632766878171540458953514382464e+308",
            builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(0.000001, 2, &builder));
    CHECK_EQ("0.0000010", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(0.0000001, 2, &builder));
    CHECK_EQ("1.0e-7", builder.finalize());

    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc2(flags, NULL, NULL, 'e', 0, 0, 0, 1);
    builder.reset();
    CHECK(dc2.to_precision(230.0, 2, &builder));
    CHECK_EQ("230", builder.finalize());

    builder.reset();
    CHECK(dc2.to_precision(23.0, 2, &builder));
    CHECK_EQ("23", builder.finalize());

    builder.reset();
    CHECK(dc2.to_precision(2.30, 2, &builder));
    CHECK_EQ("2.3", builder.finalize());

    builder.reset();
    CHECK(dc2.to_precision(2300.0, 2, &builder));
    CHECK_EQ("2.3e3", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint;
    DoubleToStringConverter dc3(flags, NULL, NULL, 'e', 0, 0, 0, 1);
    builder.reset();
    CHECK(dc3.to_precision(230.0, 2, &builder));
    CHECK_EQ("230.", builder.finalize());

    builder.reset();
    CHECK(dc3.to_precision(23.0, 2, &builder));
    CHECK_EQ("23.", builder.finalize());

    builder.reset();
    CHECK(dc3.to_precision(2.30, 2, &builder));
    CHECK_EQ("2.3", builder.finalize());

    builder.reset();
    CHECK(dc3.to_precision(2300.0, 2, &builder));
    CHECK_EQ("2.3e3", builder.finalize());

    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc4(flags, NULL, NULL, 'e', 0, 0, 0, 1);
    builder.reset();
    CHECK(dc4.to_precision(230.0, 2, &builder));
    CHECK_EQ("2.3e2", builder.finalize());

    builder.reset();
    CHECK(dc4.to_precision(23.0, 2, &builder));
    CHECK_EQ("23.0", builder.finalize());

    builder.reset();
    CHECK(dc4.to_precision(2.30, 2, &builder));
    CHECK_EQ("2.3", builder.finalize());

    builder.reset();
    CHECK(dc4.to_precision(2300.0, 2, &builder));
    CHECK_EQ("2.3e3", builder.finalize());

    // Test the examples in the comments of to_precision.
    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc5(flags, "Infinity", "NaN", 'e', 0, 0, 6, 1);
    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint;
    DoubleToStringConverter dc6(flags, "Infinity", "NaN", 'e', 0, 0, 6, 1);
    flags = DoubleToStringConverter::eEmitTrailingDecimalpoint |
        DoubleToStringConverter::eEmitTrailingZeroAfterPoint;
    DoubleToStringConverter dc7(flags, "Infinity", "NaN", 'e', 0, 0, 6, 1);

    builder.reset();
    CHECK(dc5.to_precision(0.0000012345, 2, &builder));
    CHECK_EQ("0.0000012", builder.finalize());

    builder.reset();
    CHECK(dc5.to_precision(0.00000012345, 2, &builder));
    CHECK_EQ("1.2e-7", builder.finalize());

    builder.reset();
    CHECK(dc5.to_precision(230.0, 2, &builder));
    CHECK_EQ("230", builder.finalize());

    builder.reset();
    CHECK(dc6.to_precision(230.0, 2, &builder));
    CHECK_EQ("230.", builder.finalize());

    builder.reset();
    CHECK(dc7.to_precision(230.0, 2, &builder));
    CHECK_EQ("2.3e2", builder.finalize());

    flags = DoubleToStringConverter::eNoFlags;
    DoubleToStringConverter dc8(flags, NULL, NULL, 'e', 0, 0, 6, 3);

    builder.reset();
    CHECK(dc8.to_precision(123450.0, 6, &builder));
    CHECK_EQ("123450", builder.finalize());

    builder.reset();
    CHECK(dc8.to_precision(123450.0, 5, &builder));
    CHECK_EQ("123450", builder.finalize());

    builder.reset();
    CHECK(dc8.to_precision(123450.0, 4, &builder));
    CHECK_EQ("123500", builder.finalize());

    builder.reset();
    CHECK(dc8.to_precision(123450.0, 3, &builder));
    CHECK_EQ("123000", builder.finalize());

    builder.reset();
    CHECK(dc8.to_precision(123450.0, 2, &builder));
    CHECK_EQ("1.2e5", builder.finalize());

    // Test special value handling.
    builder.reset();
    CHECK(!dc8.to_precision(Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_precision(-Double::infinity(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_precision(Double::nan(), 1, &builder));

    builder.reset();
    CHECK(!dc8.to_precision(-Double::nan(), 1, &builder));

    builder.reset();
    CHECK(dc7.to_precision(Double::infinity(), 1, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc7.to_precision(-Double::infinity(), 1, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc7.to_precision(Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc7.to_precision(-Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());
}


TEST(Conversion, DoubleToStringJavaScript) {
    const int kBufferSize = 128;
    char buffer[kBufferSize];
    StringBuilder builder(buffer, kBufferSize);
    const DoubleToStringConverter& dc =
        DoubleToStringConverter::ecma_script_converter();

    builder.reset();
    CHECK(dc.to_shortest(Double::nan(), &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(Double::infinity(), &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-Double::infinity(), &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(9.0, &builder));
    CHECK_EQ("9", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(90.0, &builder));
    CHECK_EQ("90", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(90.12, &builder));
    CHECK_EQ("90.12", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.1, &builder));
    CHECK_EQ("0.1", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.01, &builder));
    CHECK_EQ("0.01", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.0123, &builder));
    CHECK_EQ("0.0123", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(111111111111111111111.0, &builder));
    CHECK_EQ("111111111111111110000", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(100000000000000000000.0, &builder));
    CHECK_EQ("100000000000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(1111111111111111111111.0, &builder));
    CHECK_EQ("1.1111111111111111e+21", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(11111111111111111111111.0, &builder));
    CHECK_EQ("1.1111111111111111e+22", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.00001, &builder));
    CHECK_EQ("0.00001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.000001, &builder));
    CHECK_EQ("0.000001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.0000001, &builder));
    CHECK_EQ("1e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.00000012, &builder));
    CHECK_EQ("1.2e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.000000123, &builder));
    CHECK_EQ("1.23e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.00000001, &builder));
    CHECK_EQ("1e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.000000012, &builder));
    CHECK_EQ("1.2e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.000000012, &builder));
    CHECK_EQ("1.2e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(0.0000000123, &builder));
    CHECK_EQ("1.23e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-9.0, &builder));
    CHECK_EQ("-9", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-90.0, &builder));
    CHECK_EQ("-90", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-90.12, &builder));
    CHECK_EQ("-90.12", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.1, &builder));
    CHECK_EQ("-0.1", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.01, &builder));
    CHECK_EQ("-0.01", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.0123, &builder));
    CHECK_EQ("-0.0123", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-111111111111111111111.0, &builder));
    CHECK_EQ("-111111111111111110000", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-1111111111111111111111.0, &builder));
    CHECK_EQ("-1.1111111111111111e+21", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-11111111111111111111111.0, &builder));
    CHECK_EQ("-1.1111111111111111e+22", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.00001, &builder));
    CHECK_EQ("-0.00001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.000001, &builder));
    CHECK_EQ("-0.000001", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.0000001, &builder));
    CHECK_EQ("-1e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.00000012, &builder));
    CHECK_EQ("-1.2e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.000000123, &builder));
    CHECK_EQ("-1.23e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.00000001, &builder));
    CHECK_EQ("-1e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.000000012, &builder));
    CHECK_EQ("-1.2e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.000000012, &builder));
    CHECK_EQ("-1.2e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_shortest(-0.0000000123, &builder));
    CHECK_EQ("-1.23e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(Double::nan(), 2, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(Double::infinity(), 2, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-Double::infinity(), 2, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.1, 1, &builder));
    CHECK_EQ("-0.1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.1, 2, &builder));
    CHECK_EQ("-0.10", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.1, 3, &builder));
    CHECK_EQ("-0.100", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.01, 2, &builder));
    CHECK_EQ("-0.01", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.01, 3, &builder));
    CHECK_EQ("-0.010", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.01, 4, &builder));
    CHECK_EQ("-0.0100", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.001, 2, &builder));
    CHECK_EQ("-0.00", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.001, 3, &builder));
    CHECK_EQ("-0.001", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.001, 4, &builder));
    CHECK_EQ("-0.0010", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1.0, 4, &builder));
    CHECK_EQ("-1.0000", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1.0, 1, &builder));
    CHECK_EQ("-1.0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1.0, 0, &builder));
    CHECK_EQ("-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-12.0, 0, &builder));
    CHECK_EQ("-12", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1.1, 0, &builder));
    CHECK_EQ("-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-12.1, 0, &builder));
    CHECK_EQ("-12", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1.12, 0, &builder));
    CHECK_EQ("-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-12.12, 0, &builder));
    CHECK_EQ("-12", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.0000006, 7, &builder));
    CHECK_EQ("-0.0000006", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.00000006, 8, &builder));
    CHECK_EQ("-0.00000006", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.00000006, 9, &builder));
    CHECK_EQ("-0.000000060", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.00000006, 10, &builder));
    CHECK_EQ("-0.0000000600", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0, 0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0, 1, &builder));
    CHECK_EQ("0.0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0, 2, &builder));
    CHECK_EQ("0.00", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1000, 0, &builder));
    CHECK_EQ("1000", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.00001, 0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.00001, 5, &builder));
    CHECK_EQ("0.00001", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.0000000000000000001, 20, &builder));
    CHECK_EQ("0.00000000000000000010", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.00001, 17, &builder));
    CHECK_EQ("0.00001000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1000000000000000128.0, 0, &builder));
    CHECK_EQ("1000000000000000128", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1000000000000000128.0, 1, &builder));
    CHECK_EQ("1000000000000000128.0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1000000000000000128.0, 2, &builder));
    CHECK_EQ("1000000000000000128.00", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1000000000000000128.0, 20, &builder));
    CHECK_EQ("1000000000000000128.00000000000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.0, 0, &builder));
    CHECK_EQ("0", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-42.0, 3, &builder));
    CHECK_EQ("-42.000", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-1000000000000000128.0, 0, &builder));
    CHECK_EQ("-1000000000000000128", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.0000000000000000001, 20, &builder));
    CHECK_EQ("-0.00000000000000000010", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.123123123123123, 20, &builder));
    CHECK_EQ("0.12312312312312299889", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(0.5, 0, &builder));
    CHECK_EQ("1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(-0.5, 0, &builder));
    CHECK_EQ("-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(1.25, 1, &builder));
    CHECK_EQ("1.3", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(234.20405, 4, &builder));
    CHECK_EQ("234.2040", builder.finalize());

    builder.reset();
    CHECK(dc.to_fixed(234.2040506, 4, &builder));
    CHECK_EQ("234.2041", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, -1, &builder));
    CHECK_EQ("1e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.0, -1, &builder));
    CHECK_EQ("1.1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(112.0, -1, &builder));
    CHECK_EQ("1.12e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, 0, &builder));
    CHECK_EQ("1e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.0, 0, &builder));
    CHECK_EQ("1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(112.0, 0, &builder));
    CHECK_EQ("1e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, 1, &builder));
    CHECK_EQ("1.0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.0, 1, &builder));
    CHECK_EQ("1.1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(112.0, 1, &builder));
    CHECK_EQ("1.1e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, 2, &builder));
    CHECK_EQ("1.00e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.0, 2, &builder));
    CHECK_EQ("1.10e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(112.0, 2, &builder));
    CHECK_EQ("1.12e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, 3, &builder));
    CHECK_EQ("1.000e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.0, 3, &builder));
    CHECK_EQ("1.100e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(112.0, 3, &builder));
    CHECK_EQ("1.120e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.1, -1, &builder));
    CHECK_EQ("1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.11, -1, &builder));
    CHECK_EQ("1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.112, -1, &builder));
    CHECK_EQ("1.12e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.1, 0, &builder));
    CHECK_EQ("1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.11, 0, &builder));
    CHECK_EQ("1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.112, 0, &builder));
    CHECK_EQ("1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.1, 1, &builder));
    CHECK_EQ("1.0e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.11, 1, &builder));
    CHECK_EQ("1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.112, 1, &builder));
    CHECK_EQ("1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.1, 2, &builder));
    CHECK_EQ("1.00e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.11, 2, &builder));
    CHECK_EQ("1.10e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.112, 2, &builder));
    CHECK_EQ("1.12e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.1, 3, &builder));
    CHECK_EQ("1.000e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.11, 3, &builder));
    CHECK_EQ("1.100e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.112, 3, &builder));
    CHECK_EQ("1.120e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-1.0, -1, &builder));
    CHECK_EQ("-1e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-11.0, -1, &builder));
    CHECK_EQ("-1.1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-112.0, -1, &builder));
    CHECK_EQ("-1.12e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-1.0, 0, &builder));
    CHECK_EQ("-1e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-11.0, 0, &builder));
    CHECK_EQ("-1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-112.0, 0, &builder));
    CHECK_EQ("-1e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-1.0, 1, &builder));
    CHECK_EQ("-1.0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-11.0, 1, &builder));
    CHECK_EQ("-1.1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-112.0, 1, &builder));
    CHECK_EQ("-1.1e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-1.0, 2, &builder));
    CHECK_EQ("-1.00e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-11.0, 2, &builder));
    CHECK_EQ("-1.10e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-112.0, 2, &builder));
    CHECK_EQ("-1.12e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-1.0, 3, &builder));
    CHECK_EQ("-1.000e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-11.0, 3, &builder));
    CHECK_EQ("-1.100e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-112.0, 3, &builder));
    CHECK_EQ("-1.120e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.1, -1, &builder));
    CHECK_EQ("-1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.11, -1, &builder));
    CHECK_EQ("-1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.112, -1, &builder));
    CHECK_EQ("-1.12e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.1, 0, &builder));
    CHECK_EQ("-1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.11, 0, &builder));
    CHECK_EQ("-1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.112, 0, &builder));
    CHECK_EQ("-1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.1, 1, &builder));
    CHECK_EQ("-1.0e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.11, 1, &builder));
    CHECK_EQ("-1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.112, 1, &builder));
    CHECK_EQ("-1.1e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.1, 2, &builder));
    CHECK_EQ("-1.00e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.11, 2, &builder));
    CHECK_EQ("-1.10e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.112, 2, &builder));
    CHECK_EQ("-1.12e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.1, 3, &builder));
    CHECK_EQ("-1.000e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.11, 3, &builder));
    CHECK_EQ("-1.100e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.112, 3, &builder));
    CHECK_EQ("-1.120e-1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(Double::nan(), 2, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(Double::infinity(), 2, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-Double::infinity(), 2, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(1.0, 0, &builder));
    CHECK_EQ("1e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0, -1, &builder));
    CHECK_EQ("0e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.0, 2, &builder));
    CHECK_EQ("0.00e+0", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.2356, 0, &builder));
    CHECK_EQ("1e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(11.2356, 4, &builder));
    CHECK_EQ("1.1236e+1", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.000112356, 4, &builder));
    CHECK_EQ("1.1236e-4", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.000112356, 4, &builder));
    CHECK_EQ("-1.1236e-4", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(0.000112356, -1, &builder));
    CHECK_EQ("1.12356e-4", builder.finalize());

    builder.reset();
    CHECK(dc.to_exponential(-0.000112356, -1, &builder));
    CHECK_EQ("-1.12356e-4", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(Double::nan(), 1, &builder));
    CHECK_EQ("NaN", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(Double::infinity(), 2, &builder));
    CHECK_EQ("Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-Double::infinity(), 2, &builder));
    CHECK_EQ("-Infinity", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(0.000555, 15, &builder));
    CHECK_EQ("0.000555000000000000", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(0.000000555, 15, &builder));
    CHECK_EQ("5.55000000000000e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-0.000000555, 15, &builder));
    CHECK_EQ("-5.55000000000000e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(123456789.0, 1, &builder));
    CHECK_EQ("1e+8", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(123456789.0, 9, &builder));
    CHECK_EQ("123456789", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(123456789.0, 8, &builder));
    CHECK_EQ("1.2345679e+8", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(123456789.0, 7, &builder));
    CHECK_EQ("1.234568e+8", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-123456789.0, 7, &builder));
    CHECK_EQ("-1.234568e+8", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.0000000012345, 2, &builder));
    CHECK_EQ("-1.2e-9", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.000000012345, 2, &builder));
    CHECK_EQ("-1.2e-8", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.00000012345, 2, &builder));
    CHECK_EQ("-1.2e-7", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.0000012345, 2, &builder));
    CHECK_EQ("-0.0000012", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.000012345, 2, &builder));
    CHECK_EQ("-0.000012", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.00012345, 2, &builder));
    CHECK_EQ("-0.00012", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.0012345, 2, &builder));
    CHECK_EQ("-0.0012", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.012345, 2, &builder));
    CHECK_EQ("-0.012", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-.12345, 2, &builder));
    CHECK_EQ("-0.12", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-1.2345, 2, &builder));
    CHECK_EQ("-1.2", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-12.345, 2, &builder));
    CHECK_EQ("-12", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-123.45, 2, &builder));
    CHECK_EQ("-1.2e+2", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-1234.5, 2, &builder));
    CHECK_EQ("-1.2e+3", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-12345.0, 2, &builder));
    CHECK_EQ("-1.2e+4", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-12345.67, 4, &builder));
    CHECK_EQ("-1.235e+4", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(-12344.67, 4, &builder));
    CHECK_EQ("-1.234e+4", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(1.25, 2, &builder));
    CHECK_EQ("1.3", builder.finalize());

    builder.reset();
    CHECK(dc.to_precision(1.35, 2, &builder));
    CHECK_EQ("1.4", builder.finalize());
}


static double str_to_d16(const uint16_t* str16, int length, int flags,
                       double empty_string_value,
                       int* processed_characters_count, bool* processed_all)
{
    StringToDoubleConverter converter(flags, empty_string_value, Double::nan(),
                                    NULL, NULL);
    double result =
          converter.string_to_double(str16, length, processed_characters_count);
    *processed_all = (length == *processed_characters_count);
    return result;
}


static double str_to_d(const char* str, int flags, double empty_string_value,
                     int* processed_characters_count, bool* processed_all) 
{
    StringToDoubleConverter converter(flags, empty_string_value, Double::nan(),
                                        NULL, NULL);
    double result = converter.string_to_double(str, strlen(str),
                                            processed_characters_count);
    *processed_all =
        ((strlen(str) == static_cast<unsigned>(*processed_characters_count)));

    uint16_t buffer16[256];
    WALLE_ASSERT(strlen(str) < WALLE_ARRAY_SIZE(buffer16), "");
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        buffer16[i] = str[i];
    }
    int processed_characters_count16;
    bool processed_all16;
    double result16 = str_to_d16(buffer16, len, flags, empty_string_value,
                                &processed_characters_count16, &processed_all16);
    CHECK_EQ(result, result16);
    CHECK_EQ(*processed_characters_count, processed_characters_count16);
    return result;
}

TEST(Conversion, StringToDoubleVarious) 
{
    int flags;
    int processed;
    bool all_used;

    flags = StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowSpaceAfterSign |
        StringToDoubleConverter::eAllowTrailingSpace;

    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("  ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("  ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d("42", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d(" + 42 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(-42.0, str_to_d(" - 42 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("42x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" + 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" - 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);


    flags = StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowSpaceAfterSign |
        StringToDoubleConverter::eAllowTrailingSpace |
        StringToDoubleConverter::eAllowTrailingJunk;

    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("  ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("  ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d("42", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d(" + 42 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(-42.0, str_to_d(" - 42 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(42.0, str_to_d("42x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(2, processed);

    CHECK_EQ(42.0, str_to_d("42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ(42.0, str_to_d(" + 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(6, processed);

    CHECK_EQ(-42.0, str_to_d(" - 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(6, processed);


    flags = StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowSpaceAfterSign |
        StringToDoubleConverter::eAllowTrailingJunk;

    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("  ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("  ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d("42", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d(" + 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(5, processed);

    CHECK_EQ(-42.0, str_to_d(" - 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(5, processed);

    CHECK_EQ(Double::nan(), str_to_d("x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(42.0, str_to_d("42x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(2, processed);

    CHECK_EQ(42.0, str_to_d("42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(2, processed);

    CHECK_EQ(42.0, str_to_d(" + 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(5, processed);

    CHECK_EQ(-42.0, str_to_d(" - 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(5, processed);

    flags = StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowTrailingJunk;

    CHECK_EQ(42.0, str_to_d(" +42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(4, processed);

    CHECK_EQ(-42.0, str_to_d(" -42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(4, processed);

    CHECK_EQ(Double::nan(), str_to_d(" + 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" - 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);


    flags = StringToDoubleConverter::eNoFlags;

    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("  ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("  ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(42.0, str_to_d("42", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" + 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" - 42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" x", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("42x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" + 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" - 42 x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);


    flags = StringToDoubleConverter::eAllowLeadingSpace;

    CHECK_EQ(0.0, str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d(" 42", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("42 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);


    flags = StringToDoubleConverter::eAllowTrailingSpace;

    CHECK_EQ(0.0, str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(42.0, str_to_d("42 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" 42", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);
}


TEST(Conversion, StringToDoubleEmptyString) 
{
    int flags;
    int processed;
    bool all_used;

    flags = StringToDoubleConverter::eNoFlags;
    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    flags = StringToDoubleConverter::eAllowSpaceAfterSign;
    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    flags = StringToDoubleConverter::eAllowLeadingSpace;
    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    flags = StringToDoubleConverter::eAllowTrailingSpace;
    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    flags = StringToDoubleConverter::eAllowTrailingJunk;
    CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(1.0, str_to_d("", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" x", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);
}



TEST(Conversion, StringToDoubleHexString) 
{
    int flags;
    int processed;
    bool all_used;

    flags = StringToDoubleConverter::eAllowHex |
        StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowTrailingSpace |
        StringToDoubleConverter::eAllowSpaceAfterSign;

    CHECK_EQ(18.0, str_to_d("0x12", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("0x0", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d("0x123456789", flags, Double::nan(), &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(18.0, str_to_d(" 0x12 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d(" 0x0 ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d(" 0x123456789 ", flags, Double::nan(),
                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xabcdef", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xABCDEF", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d(" 0xabcdef ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d(" 0xABCDEF ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x ", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x 3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x3g", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x3.23", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("x3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x3 foo", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x3 foo", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ 0x3 foo", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("-", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(-5.0, str_to_d("-0x5", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(-5.0, str_to_d(" - 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(5.0, str_to_d(" + 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("- -0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("- +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    flags = StringToDoubleConverter::eAllowHex;

    CHECK_EQ(18.0, str_to_d("0x12", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("0x0", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d("0x123456789", flags, Double::nan(), &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" 0x12 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x0 ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x123456789 ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xabcdef", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xABCDEF", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xabcdef ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xABCDEF ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x ", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x 3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x3g", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x3.23", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("x3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ 0x3 foo", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("-", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(-5.0, str_to_d("-0x5", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" - 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" + 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("- -0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("- +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    flags = StringToDoubleConverter::eAllowTrailingJunk |
        StringToDoubleConverter::eAllowHex;

    CHECK_EQ(18.0, str_to_d("0x12", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("0x0", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d("0x123456789", flags, Double::nan(), &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" 0x12 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x0 ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(18.0, str_to_d("0x12 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(4, processed);

    CHECK_EQ(0.0, str_to_d("0x0 ", flags, 1.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0x123456789 ", flags, Double::nan(),
                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xabcdef", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xABCDEF", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xabcdef ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xABCDEF ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xabcdef ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(8, processed);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xABCDEF ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(8, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xabcdef", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0xABCDEF", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x ", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x 3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(3.0, str_to_d("0x3g", flags, 0.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ(3.0, str_to_d("0x3.234", flags, 0.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x3g", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(),
            str_to_d(" 0x3.234", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("x3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ 0x3 foo", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("-", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(-5.0, str_to_d("-0x5", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" - 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" + 0x5 ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("- -0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("- +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d("+ +0x5", flags, 0.0,  &processed, &all_used));
    CHECK_EQ(0, processed);

    flags = StringToDoubleConverter::eAllowTrailingJunk |
        StringToDoubleConverter::eAllowLeadingSpace |
        StringToDoubleConverter::eAllowTrailingSpace |
        StringToDoubleConverter::eAllowSpaceAfterSign |
        StringToDoubleConverter::eAllowHex;

    CHECK_EQ(18.0, str_to_d("0x12", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d("0x0", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d("0x123456789", flags, Double::nan(), &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(18.0, str_to_d(" 0x12 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d(" 0x0 ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d(" 0x123456789 ", flags, Double::nan(),
                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xabcdef", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d("0xABCDEF", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d(" 0xabcdef ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabcdef),
            str_to_d(" 0xABCDEF ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0xabc),
            str_to_d(" 0xabc def ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(7, processed);

    CHECK_EQ(static_cast<double>(0xabc),
            str_to_d(" 0xABC DEF ", flags, 0.0, &processed, &all_used));
    CHECK_EQ(7, processed);

    CHECK_EQ(static_cast<double>(0x12),
            str_to_d(" 0x12 ", flags, 0.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(0.0, str_to_d(" 0x0 ", flags, 1.0, &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(static_cast<double>(0x123456789),
            str_to_d(" 0x123456789 ", flags, Double::nan(),
                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d(" ", flags, Double::nan(),
                                    &processed, &all_used));
    CHECK(all_used);

    CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x ", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ(Double::nan(), str_to_d(" 0x 3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);

    CHECK_EQ((double)0x3, str_to_d("0x3g", flags, 0.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ((double)0x3, str_to_d("0x3.234", flags, 0.0, &processed, &all_used));
    CHECK_EQ(3, processed);

    CHECK_EQ(Double::nan(), str_to_d("x3", flags, 0.0,
                                    &processed, &all_used));
    CHECK_EQ(0, processed);
}


TEST(Conversion, StringToDoubleOctalString) {
  int flags;
  int processed;
  bool all_used;

  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign;

  CHECK_EQ(10.0, str_to_d("012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d(" 012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("\n012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d(" 00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("\t00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d(" 012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("\n012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d(" 0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d(" 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("\n01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d(" + 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d(" - 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("\n-\t01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d(" 012 ", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d(" 00 ", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d(" 012 ", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d(" 0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d(" 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d(" + 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d(" - 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012 ", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00 ", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012 ", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("01234567e0", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowOctals;
  CHECK_EQ(10.0, str_to_d("012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d("012 ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d("00 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d("012 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d("0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d("01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("01234567e0", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(10.0, str_to_d("012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(10.0, str_to_d("012 ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0, str_to_d("00 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012foo ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0, str_to_d("00foo ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567foo ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567foo", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567foo", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(10.0, str_to_d("012 foo ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0, str_to_d("00 foo ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789 foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567 foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567 foo", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567 foo", flags, Double::nan(), &processed,
                  &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567e0", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567e", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(10.0, str_to_d("012", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 00 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d(" 012 ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" + 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(),
           str_to_d(" - 01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(10.0, str_to_d("012 ", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("00 ", flags, 1.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("01234567 ", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0,
           str_to_d("+01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567", flags, Double::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0, str_to_d("012foo ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0, str_to_d("00foo ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567foo ", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567foo", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567foo", flags, Double::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(10.0, str_to_d("012 foo ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(4, processed);

  CHECK_EQ(0.0, str_to_d("00 foo ", flags, 1.0, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(123456789.0,
           str_to_d("0123456789 foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(11, processed);

  CHECK_EQ(342391.0,
           str_to_d("01234567 foo ", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(342391.0,
           str_to_d("+01234567 foo", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(-342391.0,
           str_to_d("-01234567 foo", flags, Double::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);
}


TEST(Conversion, StringToDoubleSpecialValues) {
  int processed;
  int flags = StringToDoubleConverter::eNoFlags;

  {
    // Use 1.0 as junk_string_value.
    StringToDoubleConverter converter(flags, 0.0, 1.0, "infinity", "NaN");

    CHECK_EQ(Double::nan(), converter.string_to_double("+NaN", 4, &processed));
    CHECK_EQ(4, processed);

    CHECK_EQ(-Double::infinity(),
             converter.string_to_double("-infinity", 9, &processed));
    CHECK_EQ(9, processed);

    CHECK_EQ(1.0, converter.string_to_double("Infinity", 8, &processed));
    CHECK_EQ(0, processed);

    CHECK_EQ(1.0, converter.string_to_double("++NaN", 5, &processed));
    CHECK_EQ(0, processed);
  }

  {
    // Use 1.0 as junk_string_value.
    StringToDoubleConverter converter(flags, 0.0, 1.0, "+infinity", "1NaN");

    // The '+' is consumed before trying to match the infinity string.
    CHECK_EQ(1.0, converter.string_to_double("+infinity", 9, &processed));
    CHECK_EQ(0, processed);

    // The match for "1NaN" triggers, and doesn't let the 1234.0 complete.
    CHECK_EQ(1.0, converter.string_to_double("1234.0", 6, &processed));
    CHECK_EQ(0, processed);
  }
}


TEST(Conversion, StringToDoubleCommentExamples) {
  // Make sure the examples in the comments are correct.
  int flags;
  int processed;
  bool all_used;

  flags = StringToDoubleConverter::eAllowHex;

  CHECK_EQ(4660.0, str_to_d("0x1234", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("0x1234.56", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  flags |= StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(4660.0,
           str_to_d("0x1234.56", flags, 0.0, &processed, &all_used));
  CHECK_EQ(6, processed);

  flags = StringToDoubleConverter::eAllowOctals;
  CHECK_EQ(668.0, str_to_d("01234", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(12349.0, str_to_d("012349", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("01234.56", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 0);

  flags |= StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(668.0,
           str_to_d("01234.56", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 5);

  flags  = StringToDoubleConverter::eAllowSpaceAfterSign;
  CHECK_EQ(-123.2, str_to_d("-   123.2", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  flags  = StringToDoubleConverter::eAllowSpaceAfterSign;
  CHECK_EQ(123.2, str_to_d("+   123.2", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  flags = StringToDoubleConverter::eAllowHex |
      StringToDoubleConverter::eAllowTrailingJunk;

  CHECK_EQ(4660.0, str_to_d("0x1234", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(4660.0, str_to_d("0x1234K", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 6);

  CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), str_to_d(" ", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 0);

  CHECK_EQ(Double::nan(), str_to_d(" 1", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 0);

  CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 0);

  CHECK_EQ(-123.45, str_to_d("-123.45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("--123.45", flags, 0.0, &processed, &all_used));
  CHECK_EQ(processed, 0);

  CHECK_EQ(123e45, str_to_d("123e45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123e45, str_to_d("123E45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123e45, str_to_d("123e+45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123e-45, str_to_d("123e-45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123.0, str_to_d("123e", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123.0, str_to_d("123e-", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  {
    StringToDoubleConverter converter(flags, 0.0, 1.0, "infinity", "NaN");
    CHECK_EQ(Double::nan(), converter.string_to_double("+NaN", 4, &processed));
    CHECK_EQ(4, processed);

    CHECK_EQ(-Double::infinity(),
             converter.string_to_double("-infinity", 9, &processed));
    CHECK_EQ(9, processed);

    CHECK_EQ(1.0, converter.string_to_double("Infinity", 9, &processed));
    CHECK_EQ(0, processed);
  }

  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowLeadingSpace;

  CHECK_EQ(Double::nan(), str_to_d("0x1234", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(668.0, str_to_d("01234", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d("", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0, str_to_d(" ", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0, str_to_d(" 1", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), str_to_d("0x", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d("0123e45", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(1239e45, str_to_d("01239e45", flags, 0.0, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(),
           str_to_d("-infinity", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), str_to_d("NaN", flags, 0.0, &processed, &all_used));
  CHECK_EQ(0, processed);
}


static float StrToF16(const uint16_t* str16, int length, int flags,
                      double empty_string_value,
                      int* processed_characters_count,
                      bool* processed_all) {
  StringToDoubleConverter converter(flags, empty_string_value, Double::nan(),
                                    NULL, NULL);
  double result =
      converter.string_to_float(str16, length, processed_characters_count);
  *processed_all = (length == *processed_characters_count);
  return result;
}


static double StrToF(const char* str, int flags, double empty_string_value,
                     int* processed_characters_count, bool* processed_all) {
  StringToDoubleConverter converter(flags, empty_string_value, Single::nan(),
                                    NULL, NULL);
  float result = converter.string_to_float(str, strlen(str),
                                         processed_characters_count);
  *processed_all =
      ((strlen(str) == static_cast<unsigned>(*processed_characters_count)));

  uint16_t buffer16[256];
  WALLE_ASSERT(strlen(str) < WALLE_ARRAY_SIZE(buffer16), "");
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    buffer16[i] = str[i];
  }
  int processed_characters_count16;
  bool processed_all16;
  float result16 = StrToF16(buffer16, len, flags, empty_string_value,
                            &processed_characters_count16,
                            &processed_all16);
  CHECK_EQ(result, result16);
  CHECK_EQ(*processed_characters_count, processed_characters_count16);
  return result;
}


TEST(Conversion, string_to_floatVarious) {
  int flags;
  int processed;
  bool all_used;

  flags = StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign |
      StringToDoubleConverter::eAllowTrailingSpace;

  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("  ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("  ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF("42", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF(" + 42 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-42.0f, StrToF(" - 42 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF("x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("42x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" + 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" - 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowTrailingJunk;

  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("  ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("  ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF("42", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF(" + 42 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-42.0f, StrToF(" - 42 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF("x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(42.0f, StrToF("42x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(42.0f, StrToF("42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(42.0f, StrToF(" + 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(6, processed);

  CHECK_EQ(-42.0f, StrToF(" - 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(6, processed);


  flags = StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign |
      StringToDoubleConverter::eAllowTrailingJunk;

  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("  ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("  ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF("42", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF(" + 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(5, processed);

  CHECK_EQ(-42.0f, StrToF(" - 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(5, processed);

  CHECK_EQ(Double::nan(), StrToF("x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(42.0f, StrToF("42x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(42.0f, StrToF("42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(42.0f, StrToF(" + 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(5, processed);

  CHECK_EQ(-42.0f, StrToF(" - 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(5, processed);

  flags = StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingJunk;

  CHECK_EQ(42.0f, StrToF(" +42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(4, processed);

  CHECK_EQ(-42.0f, StrToF(" -42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(4, processed);

  CHECK_EQ(Double::nan(), StrToF(" + 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" - 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eNoFlags;

  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF("  ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("  ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(42.0f, StrToF("42", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF(" + 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" - 42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" x", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("42x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF("42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" + 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Double::nan(), StrToF(" - 42 x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowLeadingSpace;

  CHECK_EQ(0.0f, StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF(" 42", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF("42 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowTrailingSpace;

  CHECK_EQ(0.0f, StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(42.0f, StrToF("42 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Double::nan(), StrToF(" 42", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);
}

TEST(Conversion, string_to_floatEmptyString) {
  int flags;
  int processed;
  bool all_used;

  flags = StringToDoubleConverter::eNoFlags;
  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  flags = StringToDoubleConverter::eAllowSpaceAfterSign;
  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  flags = StringToDoubleConverter::eAllowLeadingSpace;
  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  flags = StringToDoubleConverter::eAllowTrailingSpace;
  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  flags = StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(0.0f, StrToF("", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(1.0f, StrToF("", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" x", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);
}

TEST(Conversion, string_to_floatHexString) {
  int flags;
  int processed;
  bool all_used;
  double d;
  float f;

  flags = StringToDoubleConverter::eAllowHex |
      StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign;

  // Check that no double rounding occurs:
  const char* double_rounding_example1 = "0x100000100000008";
  d = str_to_d(double_rounding_example1, flags, 0.0, &processed, &all_used);
  f = StrToF(double_rounding_example1, flags, 0.0f, &processed, &all_used);
  CHECK(f != static_cast<float>(d));
  CHECK_EQ(72057602627862528.0f, StrToF(double_rounding_example1,
                                        flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  const char* double_rounding_example2 = "0x1000002FFFFFFF8";
  d = str_to_d(double_rounding_example2, flags, 0.0, &processed, &all_used);
  f = StrToF(double_rounding_example2, flags, 0.0f, &processed, &all_used);
  CHECK(f != static_cast<float>(d));
  CHECK_EQ(72057602627862528.0f, StrToF(double_rounding_example2,
                                        flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(18.0f, StrToF("0x12", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("0x0", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF("0x123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(18.0f, StrToF(" 0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" 0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF(" 0x123456789 ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xabcdef", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xABCDEF", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF(" 0xabcdef ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF(" 0xABCDEF ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("0x", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x ", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x 3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x3g", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x3.23", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("x3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x3 foo", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x3 foo", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ 0x3 foo", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("-", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(-5.0f, StrToF("-0x5", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-5.0f, StrToF(" - 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(5.0f, StrToF(" + 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("- -0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("- +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  flags = StringToDoubleConverter::eAllowHex;

  CHECK_EQ(18.0f, StrToF("0x12", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("0x0", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF("0x123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" 0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x123456789 ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xabcdef", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xABCDEF", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xabcdef ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xABCDEF ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x ", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x 3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x3g", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x3.23", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("x3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ 0x3 foo", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("-", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(-5.0f, StrToF("-0x5", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" - 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" + 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("- -0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("- +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  flags = StringToDoubleConverter::eAllowTrailingJunk |
      StringToDoubleConverter::eAllowHex;

  CHECK_EQ(18.0f, StrToF("0x12", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("0x0", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF("0x123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" 0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(18.0f, StrToF("0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(4, processed);

  CHECK_EQ(0.0f, StrToF("0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0x123456789 ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xabcdef", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xABCDEF", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xabcdef ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xABCDEF ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xabcdef ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xABCDEF ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xabcdef", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0xABCDEF", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("0x", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x ", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x 3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(3.0f, StrToF("0x3g", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(3.0f, StrToF("0x3.234", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x3g", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0x3.234", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("x3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ 0x3 foo", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("-", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(-5.0f, StrToF("-0x5", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" - 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" + 0x5 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("- -0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("- +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("+ +0x5", flags, 0.0f,  &processed, &all_used));
  CHECK_EQ(0, processed);

  flags = StringToDoubleConverter::eAllowTrailingJunk |
      StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign |
      StringToDoubleConverter::eAllowHex;

  CHECK_EQ(18.0f, StrToF("0x12", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("0x0", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF("0x123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(18.0f, StrToF(" 0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" 0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF(" 0x123456789 ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xabcdef", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF("0xABCDEF", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF(" 0xabcdef ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabcdef),
           StrToF(" 0xABCDEF ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0xabc),
           StrToF(" 0xabc def ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(7, processed);

  CHECK_EQ(static_cast<float>(0xabc),
           StrToF(" 0xABC DEF ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(7, processed);

  CHECK_EQ(static_cast<float>(0x12),
           StrToF(" 0x12 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" 0x0 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(static_cast<float>(0x123456789),
           StrToF(" 0x123456789 ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" ", flags, Single::nan(),
                                 &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF("0x", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x ", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 0x 3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ((float)0x3, StrToF("0x3g", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ((float)0x3, StrToF("0x3.234", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(Single::nan(), StrToF("x3", flags, 0.0f,
                                 &processed, &all_used));
  CHECK_EQ(0, processed);
}


TEST(Conversion, string_to_floatOctalString) {
  int flags;
  int processed;
  bool all_used;
  double d;
  float f;

  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign;

  // Check that no double rounding occurs:
  const char* double_rounding_example1 = "04000000040000000010";
  d = str_to_d(double_rounding_example1, flags, 0.0, &processed, &all_used);
  f = StrToF(double_rounding_example1, flags, 0.0f, &processed, &all_used);
  CHECK(f != static_cast<float>(d));
  CHECK_EQ(72057602627862528.0f, StrToF(double_rounding_example1,
                                        flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  const char* double_rounding_example2 = "04000000137777777770";
  d = str_to_d(double_rounding_example2, flags, 0.0, &processed, &all_used);
  f = StrToF(double_rounding_example2, flags, 0.0f, &processed, &all_used);
  CHECK(f != static_cast<float>(d));
  CHECK_EQ(72057602627862528.0f, StrToF(double_rounding_example2,
                                        flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF(" 012", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" 00", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF(" 012", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF(" 0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF(" 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF(" + 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF(" - 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF(" 012 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF(" 00 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF(" 012 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF(" 0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF(" 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF(" + 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF(" - 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(),
           StrToF("01234567e0", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowOctals;
  CHECK_EQ(10.0f, StrToF("012", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("012 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("00 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF("012 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF("0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF("01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(),
           StrToF("01234567e0", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);


  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(10.0f, StrToF("012", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(10.0f, StrToF("012 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0f, StrToF("00 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012foo ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0f, StrToF("00foo ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567foo ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567foo", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567foo", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(10.0f, StrToF("012 foo ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0f, StrToF("00 foo ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789 foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567 foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567 foo", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567 foo", flags, Single::nan(), &processed,
                  &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567e0", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567e", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  flags = StringToDoubleConverter::eAllowOctals |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowTrailingJunk;
  CHECK_EQ(10.0f, StrToF("012", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 00 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(), StrToF(" 012 ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" + 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(Single::nan(),
           StrToF(" - 01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(0, processed);

  CHECK_EQ(10.0f, StrToF("012 ", flags, 0.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(0.0f, StrToF("00 ", flags, 1.0f, &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("01234567 ", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(342391.0f,
           StrToF("+01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567", flags, Single::nan(), &processed, &all_used));
  CHECK(all_used);

  CHECK_EQ(10.0f, StrToF("012foo ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(0.0f, StrToF("00foo ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(2, processed);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567foo ", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(8, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567foo", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567foo", flags, Single::nan(), &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(10.0f, StrToF("012 foo ", flags, 0.0f, &processed, &all_used));
  CHECK_EQ(4, processed);

  CHECK_EQ(0.0f, StrToF("00 foo ", flags, 1.0f, &processed, &all_used));
  CHECK_EQ(3, processed);

  CHECK_EQ(123456789.0f,
           StrToF("0123456789 foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(11, processed);

  CHECK_EQ(342391.0f,
           StrToF("01234567 foo ", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(9, processed);

  CHECK_EQ(342391.0f,
           StrToF("+01234567 foo", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);

  CHECK_EQ(-342391.0f,
           StrToF("-01234567 foo", flags, Single::nan(),
                  &processed, &all_used));
  CHECK_EQ(10, processed);
}


TEST(Conversion, string_to_floatSpecialValues) {
  int processed;
  int flags = StringToDoubleConverter::eNoFlags;

  {
    // Use 1.0 as junk_string_value.
    StringToDoubleConverter converter(flags, 0.0f, 1.0f, "infinity", "NaN");

    CHECK_EQ(Single::nan(), converter.string_to_double("+NaN", 4, &processed));
    CHECK_EQ(4, processed);

    CHECK_EQ(-Single::infinity(),
             converter.string_to_double("-infinity", 9, &processed));
    CHECK_EQ(9, processed);

    CHECK_EQ(1.0f, converter.string_to_double("Infinity", 8, &processed));
    CHECK_EQ(0, processed);

    CHECK_EQ(1.0f, converter.string_to_double("++NaN", 5, &processed));
    CHECK_EQ(0, processed);
  }

  {
    // Use 1.0 as junk_string_value.
    StringToDoubleConverter converter(flags, 0.0f, 1.0f, "+infinity", "1NaN");

    // The '+' is consumed before trying to match the infinity string.
    CHECK_EQ(1.0f, converter.string_to_double("+infinity", 9, &processed));
    CHECK_EQ(0, processed);

    // The match for "1NaN" triggers, and doesn't let the 1234.0 complete.
    CHECK_EQ(1.0f, converter.string_to_double("1234.0", 6, &processed));
    CHECK_EQ(0, processed);
  }
}


TEST(Conversion, StringToDoubleFloatWhitespace) {
  int flags;
  int processed;
  bool all_used;

  flags = StringToDoubleConverter::eAllowLeadingSpace |
      StringToDoubleConverter::eAllowTrailingSpace |
      StringToDoubleConverter::eAllowSpaceAfterSign;

  const char kWhitespaceAscii[] = {
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20,
    '-',
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20,
    '1', '.', '2',
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20,
    0x00
  };
  CHECK_EQ(-1.2, str_to_d(kWhitespaceAscii, flags, Double::nan(),
                        &processed, &all_used));
  CHECK(all_used);
  CHECK_EQ(-1.2f, StrToF(kWhitespaceAscii, flags, Double::nan(),
                         &processed, &all_used));
  CHECK(all_used);

  const uint16_t kOghamSpaceMark = 0x1680;
  const uint16_t kMongolianVowelSeparator = 0x180E;
  const uint16_t kEnQuad = 0x2000;
  const uint16_t kEmQuad = 0x2001;
  const uint16_t kEnSpace = 0x2002;
  const uint16_t kEmSpace = 0x2003;
  const uint16_t kThreePerEmSpace = 0x2004;
  const uint16_t kFourPerEmSpace = 0x2005;
  const uint16_t kSixPerEmSpace = 0x2006;
  const uint16_t kFigureSpace = 0x2007;
  const uint16_t kPunctuationSpace = 0x2008;
  const uint16_t kThinSpace = 0x2009;
  const uint16_t kHairSpace = 0x200A;
  const uint16_t kNarrowNoBreakSpace = 0x202F;
  const uint16_t kMediumMathematicalSpace = 0x205F;
  const uint16_t kIdeographicSpace = 0x3000;

  const uint16_t kWhitespace16[] = {
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20, 0xA0, 0xFEFF,
    kOghamSpaceMark, kMongolianVowelSeparator, kEnQuad, kEmQuad,
    kEnSpace, kEmSpace, kThreePerEmSpace, kFourPerEmSpace, kSixPerEmSpace,
    kFigureSpace, kPunctuationSpace, kThinSpace, kHairSpace,
    kNarrowNoBreakSpace, kMediumMathematicalSpace, kIdeographicSpace,
    '-',
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20, 0xA0, 0xFEFF,
    kOghamSpaceMark, kMongolianVowelSeparator, kEnQuad, kEmQuad,
    kEnSpace, kEmSpace, kThreePerEmSpace, kFourPerEmSpace, kSixPerEmSpace,
    kFigureSpace, kPunctuationSpace, kThinSpace, kHairSpace,
    kNarrowNoBreakSpace, kMediumMathematicalSpace, kIdeographicSpace,
    '1', '.', '2',
    0x0A, 0x0D, 0x09, 0x0B, 0x0C, 0x20, 0xA0, 0xFEFF,
    kOghamSpaceMark, kMongolianVowelSeparator, kEnQuad, kEmQuad,
    kEnSpace, kEmSpace, kThreePerEmSpace, kFourPerEmSpace, kSixPerEmSpace,
    kFigureSpace, kPunctuationSpace, kThinSpace, kHairSpace,
    kNarrowNoBreakSpace, kMediumMathematicalSpace, kIdeographicSpace,
  };
  const int kWhitespace16Length = WALLE_ARRAY_SIZE(kWhitespace16);
  CHECK_EQ(-1.2, str_to_d16(kWhitespace16, kWhitespace16Length, flags,
                          Double::nan(),
                          &processed, &all_used));
  CHECK(all_used);
  CHECK_EQ(-1.2f, StrToF16(kWhitespace16, kWhitespace16Length, flags,
                           Single::nan(),
                           &processed, &all_used));
  CHECK(all_used);
}


TEST(Conversion, StringToDoubleCaseInsensitiveSpecialValues) {
  int processed = 0;

  int flags = StringToDoubleConverter::eAllowLeadingSpace |
    StringToDoubleConverter::eAllowTrailingJunk |
    StringToDoubleConverter::eAllowTrailingSpace;

  // Use 1.0 as junk_string_value.
  StringToDoubleConverter converter(flags, 0.0, 1.0, "infinity", "nan");

  CHECK_EQ(Double::nan(), converter.string_to_double("+nan", 4, &processed));
  CHECK_EQ(4, processed);
/*
  CHECK_EQ(Double::nan(), converter.string_to_double("-NAN", 4, &processed));
  CHECK_EQ(4, processed);

  CHECK_EQ(Double::nan(), converter.string_to_double("nAN", 3, &processed));
  CHECK_EQ(3, processed);

  CHECK_EQ(Double::nan(), converter.string_to_double("nANabc", 6, &processed));
  CHECK_EQ(3, processed);

  CHECK_EQ(+Double::infinity(),
           converter.string_to_double("+Infinity", 9, &processed));
  CHECK_EQ(9, processed);

  CHECK_EQ(-Double::infinity(),
           converter.string_to_double("-INFinity", 9, &processed));
  CHECK_EQ(9, processed);

  CHECK_EQ(Double::infinity(),
           converter.string_to_double("infINITY", 8, &processed));
  CHECK_EQ(8, processed);

  CHECK_EQ(1.0, converter.string_to_double("INF", 3, &processed));
  CHECK_EQ(0, processed);
*/
  CHECK_EQ(1.0, converter.string_to_double("+inf", 4, &processed));
  CHECK_EQ(0, processed);
}

WALLE_RESTORE_WARNING();