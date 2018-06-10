#include <stdlib.h>
#include "gay_fixed.h"
#include "gay_precision.h"
#include "gay_shortest.h"
#include "gay_shortest_single.h"
#include <walle/math/math.h>

#include <walle/format/detail/double_conversion.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/strings/string_builder.h>
#include <google/gtest/gtest.h>
#include <cstring>
using namespace walle::format_detail;


enum DtoaMode {
  eShortest,
  eShortestSingle,
  eFixd,
  ePrecision
};

static void double_to_ascii(double v, DtoaMode test_mode, int requested_digits,
                          walle::BufferRef<char> buffer, bool* sign, int* length,
                          int* point) 
{
    DoubleToStringConverter::DtoaMode mode = DoubleToStringConverter::eShortest;
    switch (test_mode) {
        case eShortest: mode = DoubleToStringConverter::eShortest; break;
        case eShortestSingle:
            mode = DoubleToStringConverter::eShortestSingle;
            break;
        case eFixd: mode = DoubleToStringConverter::eFixed; break;
        case ePrecision: mode = DoubleToStringConverter::ePrecision; break;
    }
    DoubleToStringConverter::double_to_ascii(v, mode, requested_digits,
                                 buffer.data(), buffer.size(),
                                 sign, length, point);
}

// Removes trailing '0' digits.
static void trim_representation(walle::BufferRef<char> representation) 
{
    int len = strlen(representation.data());
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (representation[i] != '0') break;
    }
    representation[i + 1] = '\0';
}


static const int kBufferSize = 100;


TEST(Dtoa, DtoaVariousDoubles) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;
    bool sign;

    double_to_ascii(0.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("0", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(0.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("0", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(0.0, eFixd, 2, buffer, &sign, &length, &point);
    EXPECT_EQ(1, length);
    EXPECT_EQ("0", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(0.0, ePrecision, 3, buffer, &sign, &length, &point);
    EXPECT_EQ(1, length);
    EXPECT_EQ("0", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.0, eFixd, 3, buffer, &sign, &length, &point);
    EXPECT_EQ(3 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.0, ePrecision, 3, buffer, &sign, &length, &point);
    EXPECT_EQ(3 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.5, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("15", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.5f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("15", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.5, eFixd, 10, buffer, &sign, &length, &point);
    EXPECT_EQ(10 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("15", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double_to_ascii(1.5, ePrecision, 10, buffer, &sign, &length, &point);
    EXPECT_EQ(10 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("15", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(1, point);

    double min_double = 5e-324;
    double_to_ascii(min_double, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("5", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-323, point);

    float min_float = 1e-45f;
    double_to_ascii(min_float, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-44, point);

    double_to_ascii(min_double, eFixd, 5, buffer, &sign, &length, &point);
    EXPECT_EQ(5, length - point);
    trim_representation(buffer);
    EXPECT_EQ("", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-5 >= point, true);

    double_to_ascii(min_double, ePrecision, 5, buffer, &sign, &length, &point);
    EXPECT_EQ(5, length);
    trim_representation(buffer);
    EXPECT_EQ("49407", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-323, point);

    double max_double = 1.7976931348623157e308;
    double_to_ascii(max_double, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("17976931348623157", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(309, point);

    float max_float = 3.4028234e38f;
    double_to_ascii(max_float, eShortestSingle, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ("34028235", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(39, point);

    double_to_ascii(max_double, ePrecision, 7, buffer, &sign, &length, &point);
    EXPECT_EQ(7, length);
    trim_representation(buffer);
    EXPECT_EQ("1797693", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(309, point);

    double_to_ascii(4294967272.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("4294967272", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(4294967272.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("42949673", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(4294967272.0, eFixd, 5, buffer, &sign, &length, &point);
    EXPECT_EQ(5 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("4294967272", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(4294967272.0, ePrecision, 14,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(14 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("4294967272", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(4.1855804968213567e298, eShortest, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ("4185580496821357", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(299, point);

    double_to_ascii(4.1855804968213567e298, ePrecision, 20,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("41855804968213567225", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(299, point);

    double_to_ascii(5.5626846462680035e-309, eShortest, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ("5562684646268003", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-308, point);

    double_to_ascii(5.5626846462680035e-309, ePrecision, 1,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(1 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("6", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-308, point);

    double_to_ascii(-2147483648.0, eShortest, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(1, sign);
    EXPECT_EQ("2147483648", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(-2147483648.0, eShortestSingle, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(1, sign);
    EXPECT_EQ("21474836", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);


    double_to_ascii(-2147483648.0, eFixd, 2, buffer, &sign, &length, &point);
    EXPECT_EQ(2 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ(1, sign);
    EXPECT_EQ("2147483648", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(-2147483648.0, ePrecision, 5,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(5 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ(1, sign);
    EXPECT_EQ("21475", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(10, point);

    double_to_ascii(-3.5844466002796428e+298, eShortest, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(1, sign);
    EXPECT_EQ("35844466002796428", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(299, point);

    double_to_ascii(-3.5844466002796428e+298, ePrecision, 10,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(1, sign);
    EXPECT_EQ(10 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("35844466", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(299, point);

    uint64_t smallest_normal64 = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    double v = Double(smallest_normal64).value();
    double_to_ascii(v, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("22250738585072014", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-307, point);

    uint32_t smallest_normal32 = 0x00800000;
    float f = Single(smallest_normal32).value();
    double_to_ascii(f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("11754944", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-37, point);

    double_to_ascii(v, ePrecision, 20, buffer, &sign, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("22250738585072013831", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-307, point);

    uint64_t largest_denormal64 = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    v = Double(largest_denormal64).value();
    double_to_ascii(v, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("2225073858507201", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-307, point);

    uint32_t largest_denormal32 = 0x007FFFFF;
    f = Single(largest_denormal32).value();
    double_to_ascii(f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("11754942", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-37, point);

    double_to_ascii(v, ePrecision, 20, buffer, &sign, &length, &point);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("2225073858507200889", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-307, point);

    double_to_ascii(4128420500802942e-24, eShortest, 0,
                    buffer, &sign, &length, &point);
    EXPECT_EQ(0, sign);
    EXPECT_EQ("4128420500802942", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-8, point);

    v = -3.9292015898194142585311918e-10;
    double_to_ascii(v, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("39292015898194143", std::string(std::string(std::string(buffer.data()))));

    f = -3.9292015898194142585311918e-10f;
    double_to_ascii(f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ("39292017", std::string(std::string(std::string(buffer.data()))));

    v = 4194304.0;
    double_to_ascii(v, eFixd, 5, buffer, &sign, &length, &point);
    EXPECT_EQ(5 >= length - point, true);
    trim_representation(buffer);
    EXPECT_EQ("4194304", std::string(std::string(std::string(buffer.data()))));

    v = 3.3161339052167390562200598e-237;
    double_to_ascii(v, ePrecision, 19, buffer, &sign, &length, &point);
    EXPECT_EQ(19 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("3316133905216739056", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(-236, point);
}


TEST(Dtoa, DtoaSign) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    double_to_ascii(0.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-0.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(1.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-1.0, eShortest, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(0.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-0.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(1.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-1.0f, eShortestSingle, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(0.0, ePrecision, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-0.0, ePrecision, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(1.0, ePrecision, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-1.0, ePrecision, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(0.0, eFixd, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-0.0, eFixd, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);

    double_to_ascii(1.0, eFixd, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, false);

    double_to_ascii(-1.0, eFixd, 1, buffer, &sign, &length, &point);
    EXPECT_EQ(sign, true);
}


TEST(Dtoa, DtoaCorners) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    double_to_ascii(0.0, ePrecision, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(0, length);
    EXPECT_EQ("", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(sign, false);

    double_to_ascii(1.0, ePrecision, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(0, length);
    EXPECT_EQ("", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(sign, false);

    double_to_ascii(0.0, eFixd, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(1, length);
    EXPECT_EQ("0", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(sign, false);

    double_to_ascii(1.0, eFixd, 0, buffer, &sign, &length, &point);
    EXPECT_EQ(1, length);
    EXPECT_EQ("1", std::string(std::string(std::string(buffer.data()))));
    EXPECT_EQ(sign, false);
}


TEST(Dtoa, DtoaGayShortest) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    walle::BufferRef<const PrecomputedShortest> precomputed =
        precomputed_shortest_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortest current_test = precomputed[i];
        double v = current_test.v;
        double_to_ascii(v, eShortest, 0, buffer, &sign, &length, &point);
        EXPECT_EQ(sign, false);  // All precomputed numbers are positive.
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(std::string(std::string(buffer.data()))));
    }
}


TEST(Dtoa, DtoaGayShortestSingle) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    walle::BufferRef<const PrecomputedShortestSingle> precomputed =
        precomputed_shortest_single_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortestSingle current_test = precomputed[i];
        float v = current_test.v;
        double_to_ascii(v, eShortestSingle, 0, buffer, &sign, &length, &point);
        EXPECT_EQ(sign, false);  // All precomputed numbers are positive.
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(std::string(std::string(buffer.data()))));
    }
}


TEST(Dtoa, DtoaGayFixed) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    walle::BufferRef<const PrecomputedFixed> precomputed =
        precomputed_fixed_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedFixed current_test = precomputed[i];
        double v = current_test.v;
        int number_digits = current_test.number_digits;
        double_to_ascii(v, eFixd, number_digits, buffer, &sign, &length, &point);
        EXPECT_EQ(sign, false);  // All precomputed numbers are positive.
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(number_digits >= length - point, true);
        trim_representation(buffer);
        EXPECT_EQ(current_test.representation, std::string(std::string(std::string(buffer.data()))));
    }
}

TEST(Dota, DtoaGayPrecision) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool sign;
    int length;
    int point;

    walle::BufferRef<const PrecomputedPrecision> precomputed =
        precomputed_precision_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedPrecision current_test = precomputed[i];
        double v = current_test.v;
        int number_digits = current_test.number_digits;
        double_to_ascii(v, ePrecision, number_digits,
                    buffer, &sign, &length, &point);
        EXPECT_EQ(sign, false);  // All precomputed numbers are positive.
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(number_digits, length);
        trim_representation(buffer);
        EXPECT_EQ(current_test.representation, std::string(std::string(buffer.data())));
    }
}
