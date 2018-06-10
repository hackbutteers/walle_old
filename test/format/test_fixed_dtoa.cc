#include <stdlib.h>
#include "gay_fixed.h"
#include "gay_precision.h"
#include "gay_shortest.h"
#include "gay_shortest_single.h"

#include <walle/format/detail/fixed_dtoa.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/strings/string_builder.h>
#include <google/gtest/gtest.h>
#include <cstring>
#include <walle/math/math.h>


using namespace walle::format_detail;

static const int kBufferSize = 500;

TEST(FixedDtoa, FastFixedVariousDoubles) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    EXPECT_EQ(fast_fixed_dtoa(1.0, 1, buffer, &length, &point),true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1.0, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1.0, 0, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0xFFFFFFFF, 5, buffer, &length, &point), true);
    EXPECT_EQ("4294967295", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    EXPECT_EQ(fast_fixed_dtoa(4294967296.0, 5, buffer, &length, &point), true);
    EXPECT_EQ("4294967296", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    EXPECT_EQ(fast_fixed_dtoa(1e21, 5, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    // EXPECT_EQ(22, point);
    EXPECT_EQ(22, point);

    EXPECT_EQ(fast_fixed_dtoa(999999999999999868928.00, 2, buffer, &length, &point), true);
    EXPECT_EQ("999999999999999868928", std::string(buffer.data()));
    EXPECT_EQ(21, point);

    EXPECT_EQ(fast_fixed_dtoa(6.9999999999999989514240000e+21, 5, buffer,
                        &length, &point), true);
    EXPECT_EQ("6999999999999998951424", std::string(buffer.data()));
    EXPECT_EQ(22, point);

    EXPECT_EQ(fast_fixed_dtoa(1.5, 5, buffer, &length, &point), true);
    EXPECT_EQ("15", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1.55, 5, buffer, &length, &point), true);
    EXPECT_EQ("155", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1.55, 1, buffer, &length, &point), true);
    EXPECT_EQ("16", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1.00000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("100000001", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.1, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(0, point);

    EXPECT_EQ(fast_fixed_dtoa(0.01, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-2, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-3, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-4, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-5, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-6, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-7, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000001, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-8, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-9, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-10, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-11, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-12, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000000001, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-13, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-14, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-15, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-16, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-17, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-18, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000000000000001, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-19, point);

    EXPECT_EQ(fast_fixed_dtoa(0.10000000004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(0, point);

    EXPECT_EQ(fast_fixed_dtoa(0.01000000004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00100000004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-2, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00010000004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-3, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00001000004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-4, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000100004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-5, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000010004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-6, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000001004, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-7, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000104, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-8, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000001000004, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-9, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000100004, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-10, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000010004, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-11, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000001004, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-12, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000000104, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-13, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000001000004, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-14, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000100004, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-15, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000010004, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-16, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000001004, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-17, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000000104, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-18, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000000014, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-19, point);

    EXPECT_EQ(fast_fixed_dtoa(0.10000000006, 10, buffer, &length, &point), true);
    EXPECT_EQ("1000000001", std::string(buffer.data()));
    EXPECT_EQ(0, point);

    EXPECT_EQ(fast_fixed_dtoa(0.01000000006, 10, buffer, &length, &point), true);
    EXPECT_EQ("100000001", std::string(buffer.data()));
    EXPECT_EQ(-1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00100000006, 10, buffer, &length, &point), true);
    EXPECT_EQ("10000001", std::string(buffer.data()));
    EXPECT_EQ(-2, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00010000006, 10, buffer, &length, &point), true);
    EXPECT_EQ("1000001", std::string(buffer.data()));
    EXPECT_EQ(-3, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00001000006, 10, buffer, &length, &point), true);
    EXPECT_EQ("100001", std::string(buffer.data()));
    EXPECT_EQ(-4, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000100006, 10, buffer, &length, &point), true);
    EXPECT_EQ("10001", std::string(buffer.data()));
    EXPECT_EQ(-5, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000010006, 10, buffer, &length, &point), true);
    EXPECT_EQ("1001", std::string(buffer.data()));
    EXPECT_EQ(-6, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000001006, 10, buffer, &length, &point), true);
    EXPECT_EQ("101", std::string(buffer.data()));
    EXPECT_EQ(-7, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000000106, 10, buffer, &length, &point), true);
    EXPECT_EQ("11", std::string(buffer.data()));
    EXPECT_EQ(-8, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000001000006, 15, buffer, &length, &point), true);
    EXPECT_EQ("100001", std::string(buffer.data()));
    EXPECT_EQ(-9, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000100006, 15, buffer, &length, &point), true);
    EXPECT_EQ("10001", std::string(buffer.data()));
    EXPECT_EQ(-10, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000010006, 15, buffer, &length, &point), true);
    EXPECT_EQ("1001", std::string(buffer.data()));
    EXPECT_EQ(-11, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000001006, 15, buffer, &length, &point), true);
    EXPECT_EQ("101", std::string(buffer.data()));
    EXPECT_EQ(-12, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000000000000106, 15, buffer, &length, &point), true);
    EXPECT_EQ("11", std::string(buffer.data()));
    EXPECT_EQ(-13, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000001000006, 20, buffer, &length, &point), true);
    EXPECT_EQ("100001", std::string(buffer.data()));
    EXPECT_EQ(-14, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000100006, 20, buffer, &length, &point), true);
    EXPECT_EQ("10001", std::string(buffer.data()));
    EXPECT_EQ(-15, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000010006, 20, buffer, &length, &point), true);
    EXPECT_EQ("1001", std::string(buffer.data()));
    EXPECT_EQ(-16, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000001006, 20, buffer, &length, &point), true);
    EXPECT_EQ("101", std::string(buffer.data()));
    EXPECT_EQ(-17, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000000106, 20, buffer, &length, &point), true);
    EXPECT_EQ("11", std::string(buffer.data()));
    EXPECT_EQ(-18, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000000000000000016, 20, buffer, &length, &point), true);
    EXPECT_EQ("2", std::string(buffer.data()));
    EXPECT_EQ(-19, point);

    EXPECT_EQ(fast_fixed_dtoa(0.6, 0, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.96, 1, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.996, 2, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.9996, 3, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.99996, 4, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.999996, 5, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.9999996, 6, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.99999996, 7, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.999999996, 8, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.9999999996, 9, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.99999999996, 10, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.999999999996, 11, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.9999999999996, 12, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.99999999999996, 13, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.999999999999996, 14, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.9999999999999996, 15, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00999999999999996, 16, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-1, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000999999999999996, 17, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-2, point);

    EXPECT_EQ(fast_fixed_dtoa(0.0000999999999999996, 18, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-3, point);

    EXPECT_EQ(fast_fixed_dtoa(0.00000999999999999996, 19, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-4, point);

    EXPECT_EQ(fast_fixed_dtoa(0.000000999999999999996, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-5, point);

    EXPECT_EQ(fast_fixed_dtoa(323423.234234, 10, buffer, &length, &point), true);
    EXPECT_EQ("323423234234", std::string(buffer.data()));
    EXPECT_EQ(6, point);

    EXPECT_EQ(fast_fixed_dtoa(12345678.901234, 4, buffer, &length, &point), true);
    EXPECT_EQ("123456789012", std::string(buffer.data()));
    EXPECT_EQ(8, point);

    EXPECT_EQ(fast_fixed_dtoa(98765.432109, 5, buffer, &length, &point), true);
    EXPECT_EQ("9876543211", std::string(buffer.data()));
    EXPECT_EQ(5, point);

    EXPECT_EQ(fast_fixed_dtoa(42, 20, buffer, &length, &point), true);
    EXPECT_EQ("42", std::string(buffer.data()));
    EXPECT_EQ(2, point);

    EXPECT_EQ(fast_fixed_dtoa(0.5, 0, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-23, 10, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-10, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-123, 2, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-2, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-123, 0, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(0, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-23, 20, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-20, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-21, 20, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-20, point);

    EXPECT_EQ(fast_fixed_dtoa(1e-22, 20, buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-20, point);

    EXPECT_EQ(fast_fixed_dtoa(6e-21, 20, buffer, &length, &point), true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-19, point);

    EXPECT_EQ(fast_fixed_dtoa(9.1193616301674545152000000e+19, 0,
                        buffer, &length, &point), true);
    EXPECT_EQ("91193616301674545152", std::string(buffer.data()));
    EXPECT_EQ(20, point);

    EXPECT_EQ(fast_fixed_dtoa(4.8184662102767651659096515e-04, 19,
                        buffer, &length, &point), true);
    EXPECT_EQ("4818466210276765", std::string(buffer.data()));
    EXPECT_EQ(-3, point);

    EXPECT_EQ(fast_fixed_dtoa(1.9023164229540652612705182e-23, 8,
                        buffer, &length, &point), true);
    EXPECT_EQ("", std::string(buffer.data()));
    EXPECT_EQ(-8, point);

    EXPECT_EQ(fast_fixed_dtoa(1000000000000000128.0, 0,
                        buffer, &length, &point), true);
    EXPECT_EQ("1000000000000000128", std::string(buffer.data()));
    EXPECT_EQ(19, point);

    EXPECT_EQ(fast_fixed_dtoa(2.10861548515811875e+15, 17, buffer, &length, &point), true);
    EXPECT_EQ("210861548515811875", std::string(buffer.data()));
    EXPECT_EQ(16, point);
}


TEST(FixedDtoa, fast_fixed_dtoaGayFixed) {
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool status;
    int length;
    int point;

    walle::BufferRef<const PrecomputedFixed> precomputed =
        precomputed_fixed_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedFixed current_test = precomputed[i];
        double v = current_test.v;
        int number_digits = current_test.number_digits;
        status = fast_fixed_dtoa(v, number_digits,
                            buffer, &length, &point);
        EXPECT_EQ(status, true);
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(number_digits >= length - point, true);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
}
