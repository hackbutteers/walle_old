#include <stdlib.h>
#include "gay_fixed.h"
#include "gay_precision.h"
#include "gay_shortest.h"
#include "gay_shortest_single.h"
#include <walle/math/math.h>
#include <walle/format/detail/bignum_dtoa.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/strings/string_builder.h>
#include <google/gtest/gtest.h>
#include <cstring>
using namespace walle::format_detail;


// Removes trailing '0' digits.
// Can return the empty string if all digits are 0.
static void trim_representation(walle::BufferRef<char> representation) 
{
    int len = walle::str_length(representation.data());
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (representation[i] != '0') break;
    }
    representation[i + 1] = '\0';
}


static const int kBufferSize = 100;


TEST(BignumDtoa, BignumDtoaVariousDoubles) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    bignum_dtoa(1.0, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    bignum_dtoa(1.0, BignumDtoaMode::eBignumDtoaFixed, 3, buffer, &length, &point);
    EXPECT_EQ(3 >=length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    bignum_dtoa(1.0, BignumDtoaMode::eBignumDtoaPrecision, 3, buffer, &length, &point);
    EXPECT_EQ(3 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    bignum_dtoa(1.5, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("15", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    bignum_dtoa(1.5, BignumDtoaMode::eBignumDtoaFixed, 10, buffer, &length, &point);
    EXPECT_EQ(10 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("15", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    bignum_dtoa(1.5, BignumDtoaMode::eBignumDtoaPrecision, 10, buffer, &length, &point);
    EXPECT_EQ(10 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("15", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    double min_double = 5e-324;
    bignum_dtoa(min_double, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("5", std::string(buffer.data()));
    EXPECT_EQ(-323, point);

    bignum_dtoa(min_double, BignumDtoaMode::eBignumDtoaFixed, 5, buffer, &length, &point);
    EXPECT_EQ(5 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("", std::string(buffer.data()));

    bignum_dtoa(min_double, BignumDtoaMode::eBignumDtoaPrecision, 5, buffer, &length, &point);
    EXPECT_EQ(5 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("49407", std::string(buffer.data()));
    EXPECT_EQ(-323, point);

    double max_double = 1.7976931348623157e308;
    bignum_dtoa(max_double, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("17976931348623157", std::string(buffer.data()));
    EXPECT_EQ(309, point);

    bignum_dtoa(max_double, BignumDtoaMode::eBignumDtoaPrecision, 7, buffer, &length, &point);
    EXPECT_EQ(7 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("1797693", std::string(buffer.data()));
    EXPECT_EQ(309, point);

    bignum_dtoa(4294967272.0, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("4294967272", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    bignum_dtoa(4294967272.0, BignumDtoaMode::eBignumDtoaFixed, 5, buffer, &length, &point);
    EXPECT_EQ("429496727200000", std::string(buffer.data()));
    EXPECT_EQ(10, point);


    bignum_dtoa(4294967272.0, BignumDtoaMode::eBignumDtoaPrecision, 14, buffer, &length, &point);
    EXPECT_EQ(14 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("4294967272", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    bignum_dtoa(4.1855804968213567e298, BignumDtoaMode::eBignumDtoaShortest, 0,
                buffer, &length, &point);
    EXPECT_EQ("4185580496821357", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    bignum_dtoa(4.1855804968213567e298, BignumDtoaMode::eBignumDtoaPrecision, 20,
                buffer, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("41855804968213567225", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    bignum_dtoa(5.5626846462680035e-309, BignumDtoaMode::eBignumDtoaShortest, 0,
                buffer, &length, &point);
    EXPECT_EQ("5562684646268003", std::string(buffer.data()));
    EXPECT_EQ(-308, point);

    bignum_dtoa(5.5626846462680035e-309, BignumDtoaMode::eBignumDtoaPrecision, 1,
                buffer, &length, &point);
    EXPECT_EQ(1 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("6", std::string(buffer.data()));
    EXPECT_EQ(-308, point);

    bignum_dtoa(2147483648.0, BignumDtoaMode::eBignumDtoaShortest, 0,
                buffer, &length, &point);
    EXPECT_EQ("2147483648", std::string(buffer.data()));
    EXPECT_EQ(10, point);


    bignum_dtoa(2147483648.0, BignumDtoaMode::eBignumDtoaFixed, 2,
                buffer, &length, &point);
    EXPECT_EQ(2 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("2147483648", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    bignum_dtoa(2147483648.0, BignumDtoaMode::eBignumDtoaPrecision, 5,
                buffer, &length, &point);
    EXPECT_EQ(5 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("21475", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    bignum_dtoa(3.5844466002796428e+298, BignumDtoaMode::eBignumDtoaShortest, 0,
                buffer, &length, &point);
    EXPECT_EQ("35844466002796428", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    bignum_dtoa(3.5844466002796428e+298, BignumDtoaMode::eBignumDtoaPrecision, 10,
                buffer, &length, &point);
    EXPECT_EQ(10 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("35844466", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    uint64_t smallest_normal64 = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    double v = Double(smallest_normal64).value();
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("22250738585072014", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaPrecision, 20, buffer, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("22250738585072013831", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    uint64_t largest_denormal64 = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    v = Double(largest_denormal64).value();
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("2225073858507201", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaPrecision, 20, buffer, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("2225073858507200889", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    bignum_dtoa(4128420500802942e-24, BignumDtoaMode::eBignumDtoaShortest, 0,
                buffer, &length, &point);
    EXPECT_EQ("4128420500802942", std::string(buffer.data()));
    EXPECT_EQ(-8, point);

    v = 3.9292015898194142585311918e-10;
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
    EXPECT_EQ("39292015898194143", std::string(buffer.data()));

    v = 4194304.0;
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaFixed, 5, buffer, &length, &point);
    EXPECT_EQ(5 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("4194304", std::string(buffer.data()));

    v = 3.3161339052167390562200598e-237;
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaPrecision, 19, buffer, &length, &point);
    EXPECT_EQ(19 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("3316133905216739056", std::string(buffer.data()));
    EXPECT_EQ(-236, point);

    v = 7.9885183916008099497815232e+191;
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaPrecision, 4, buffer, &length, &point);
    EXPECT_EQ(4 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("7989", std::string(buffer.data()));
    EXPECT_EQ(192, point);

    v = 1.0000000000000012800000000e+17;
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaFixed, 1, buffer, &length, &point);
    EXPECT_EQ(1 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("100000000000000128", std::string(buffer.data()));
    EXPECT_EQ(18, point);
}


TEST(BignumDtoa, BignumDtoaShortestVariousFloats) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    float min_float = 1e-45f;
    bignum_dtoa(min_float, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-44, point);


    float max_float = 3.4028234e38f;
    bignum_dtoa(max_float, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("34028235", std::string(buffer.data()));
    EXPECT_EQ(39, point);

    bignum_dtoa(4294967272.0f, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("42949673", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    bignum_dtoa(3.32306998946228968226e+35f, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("332307", std::string(buffer.data()));
    EXPECT_EQ(36, point);

    bignum_dtoa(1.2341e-41f, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("12341", std::string(buffer.data()));
    EXPECT_EQ(-40, point);

    bignum_dtoa(3.3554432e7, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("33554432", std::string(buffer.data()));
    EXPECT_EQ(8, point);

    bignum_dtoa(3.26494756798464e14f, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("32649476", std::string(buffer.data()));
    EXPECT_EQ(15, point);

    bignum_dtoa(3.91132223637771935344e37f, BignumDtoaMode::eBignumDtoaShortestSingle, 0,
                buffer, &length, &point);
    EXPECT_EQ("39113222", std::string(buffer.data()));
    EXPECT_EQ(38, point);

    uint32_t smallest_normal32 = 0x00800000;
    double v = Single(smallest_normal32).value();
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortestSingle, 0, buffer, &length, &point);
    EXPECT_EQ("11754944", std::string(buffer.data()));
    EXPECT_EQ(-37, point);

    uint32_t largest_denormal32 = 0x007FFFFF;
    v = Single(largest_denormal32).value();
    bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortestSingle, 0, buffer, &length, &point);
    EXPECT_EQ("11754942", std::string(buffer.data()));
    EXPECT_EQ(-37, point);
}


TEST(BignumDtoa, BignumDtoaGayShortest) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    walle::BufferRef<const PrecomputedShortest> precomputed =
        precomputed_shortest_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortest current_test = precomputed[i];
        double v = current_test.v;
        bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortest, 0, buffer, &length, &point);
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
}


TEST(BignumDtoa, BignumDtoaGayShortestSingle) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    walle::BufferRef<const PrecomputedShortestSingle> precomputed =
        precomputed_shortest_single_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortestSingle current_test = precomputed[i];
        float v = current_test.v;
        bignum_dtoa(v, BignumDtoaMode::eBignumDtoaShortestSingle, 0, buffer, &length, &point);
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
}


TEST(BignumDtoa, BignumDtoaGayFixed) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    walle::BufferRef<const PrecomputedFixed> precomputed =
        precomputed_fixed_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedFixed current_test = precomputed[i];
        double v = current_test.v;
        int number_digits = current_test.number_digits;
        bignum_dtoa(v, BignumDtoaMode::eBignumDtoaFixed, number_digits, buffer, &length, &point);
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(number_digits >= length - point, true);
        trim_representation(buffer);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
}


TEST(IEEE, BignumDtoaGayPrecision) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;

    walle::BufferRef<const PrecomputedPrecision> precomputed =
        precomputed_precision_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
            const PrecomputedPrecision current_test = precomputed[i];
            double v = current_test.v;
            int number_digits = current_test.number_digits;
            bignum_dtoa(v, BignumDtoaMode::eBignumDtoaPrecision, number_digits,
                    buffer, &length, &point);
            EXPECT_EQ(current_test.decimal_point, point);
            EXPECT_EQ(number_digits >= length, true);
            trim_representation(buffer);
            EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
}
