#include <stdlib.h>
#include "gay_fixed.h"
#include "gay_precision.h"
#include "gay_shortest.h"
#include "gay_shortest_single.h"

#include <walle/format/detail/fast_dtoa.h>
#include <walle/format/detail/simple_fp.h>
#include <walle/format/detail/ieee.h>
#include <walle/config/base.h>
#include <walle/strings/string_builder.h>
#include <google/gtest/gtest.h>
#include <cstring>
#include <walle/math/math.h>

using namespace walle::format_detail;

static const int kBufferSize = 100;


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


TEST(FastDtoa, FastDtoaShortestVariousDoubles) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;
    bool status;

    double min_double = 5e-324;
    status = fast_dtoa(min_double, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("5", std::string(buffer.data()));
    EXPECT_EQ(-323, point);

    double max_double = 1.7976931348623157e308;
    status = fast_dtoa(max_double, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("17976931348623157", std::string(buffer.data()));
    EXPECT_EQ(309, point);

    status = fast_dtoa(4294967272.0, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("4294967272", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    status = fast_dtoa(4.1855804968213567e298, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("4185580496821357", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    status = fast_dtoa(5.5626846462680035e-309, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("5562684646268003", std::string(buffer.data()));
    EXPECT_EQ(-308, point);

    status = fast_dtoa(2147483648.0, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("2147483648", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    status = fast_dtoa(3.5844466002796428e+298, FastDtoaMode::eFastDtoaShortest, 0,
                        buffer, &length, &point);
    if (status) {  // Not all fast_dtoa variants manage to compute this number.
        EXPECT_EQ("35844466002796428", std::string(buffer.data()));
        EXPECT_EQ(299, point);
    }

    uint64_t smallest_normal64 = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    double v = Double(smallest_normal64).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortest, 0, buffer, &length, &point);
    if (status) {
        EXPECT_EQ("22250738585072014", std::string(buffer.data()));
        EXPECT_EQ(-307, point);
    }

    uint64_t largest_denormal64 = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    v = Double(largest_denormal64).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortest, 0, buffer, &length, &point);
    if (status) {
        EXPECT_EQ("2225073858507201", std::string(buffer.data()));
        EXPECT_EQ(-307, point);
    }
}


TEST(FastDtoa, FastDtoaShortestVariousFloats) {
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;
    bool status;

    float min_float = 1e-45f;
    status = fast_dtoa(min_float, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(-44, point);


    float max_float = 3.4028234e38f;
    status = fast_dtoa(max_float, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("34028235", std::string(buffer.data()));
    EXPECT_EQ(39, point);

    status = fast_dtoa(4294967272.0f, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("42949673", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    status = fast_dtoa(3.32306998946228968226e+35f, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("332307", std::string(buffer.data()));
    EXPECT_EQ(36, point);

    status = fast_dtoa(1.2341e-41f, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("12341", std::string(buffer.data()));
    EXPECT_EQ(-40, point);

    status = fast_dtoa(3.3554432e7, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("33554432", std::string(buffer.data()));
    EXPECT_EQ(8, point);

    status = fast_dtoa(3.26494756798464e14f, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("32649476", std::string(buffer.data()));
    EXPECT_EQ(15, point);

    status = fast_dtoa(3.91132223637771935344e37f, FastDtoaMode::eFastDtoaShortestSingle, 0,
                        buffer, &length, &point);
    if (status) {  // Not all fast_dtoa variants manage to compute this number.
        EXPECT_EQ("39113222", std::string(buffer.data()));
        EXPECT_EQ(38, point);
    }

    uint32_t smallest_normal32 = 0x00800000;
    float v = Single(smallest_normal32).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortestSingle, 0, buffer, &length, &point);
    if (status) {
        EXPECT_EQ("11754944", std::string(buffer.data()));
        EXPECT_EQ(-37, point);
    }

    uint32_t largest_denormal32 = 0x007FFFFF;
    v = Single(largest_denormal32).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortestSingle, 0, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("11754942", std::string(buffer.data()));
    EXPECT_EQ(-37, point);
}


TEST(FastDtoa, FastDtoaPrecisionVariousDoubles) 
{
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    int length;
    int point;
    bool status;

    status = fast_dtoa(1.0, FastDtoaMode::eFastDtoaPrecision, 3, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ(3 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("1", std::string(buffer.data()));
    EXPECT_EQ(1, point);

    status = fast_dtoa(1.5, FastDtoaMode::eFastDtoaPrecision, 10, buffer, &length, &point);
    if (status) {
        EXPECT_EQ(10 >= length, true);
        trim_representation(buffer);
        EXPECT_EQ("15", std::string(buffer.data()));
        EXPECT_EQ(1, point);
    }

    double min_double = 5e-324;
    status = fast_dtoa(min_double, FastDtoaMode::eFastDtoaPrecision, 5,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("49407", std::string(buffer.data()));
    EXPECT_EQ(-323, point);

    double max_double = 1.7976931348623157e308;
    status = fast_dtoa(max_double, FastDtoaMode::eFastDtoaPrecision, 7,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("1797693", std::string(buffer.data()));
    EXPECT_EQ(309, point);

    status = fast_dtoa(4294967272.0, FastDtoaMode::eFastDtoaPrecision, 14,
                        buffer, &length, &point);
    if (status) {
        EXPECT_EQ(14 >= length, true);
        trim_representation(buffer);
        EXPECT_EQ("4294967272", std::string(buffer.data()));
        EXPECT_EQ(10, point);
    }

    status = fast_dtoa(4.1855804968213567e298, FastDtoaMode::eFastDtoaPrecision, 17,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("41855804968213567", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    status = fast_dtoa(5.5626846462680035e-309, FastDtoaMode::eFastDtoaPrecision, 1,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("6", std::string(buffer.data()));
    EXPECT_EQ(-308, point);

    status = fast_dtoa(2147483648.0, FastDtoaMode::eFastDtoaPrecision, 5,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("21475", std::string(buffer.data()));
    EXPECT_EQ(10, point);

    status = fast_dtoa(3.5844466002796428e+298, FastDtoaMode::eFastDtoaPrecision, 10,
                        buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ(10 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("35844466", std::string(buffer.data()));
    EXPECT_EQ(299, point);

    uint64_t smallest_normal64 = WALLE_UINT64_2PART_C(0x00100000, 00000000);
    double v = Double(smallest_normal64).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, 17, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("22250738585072014", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    uint64_t largest_denormal64 = WALLE_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
    v = Double(largest_denormal64).value();
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, 17, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ(20 >= length, true);
    trim_representation(buffer);
    EXPECT_EQ("22250738585072009", std::string(buffer.data()));
    EXPECT_EQ(-307, point);

    v = 3.3161339052167390562200598e-237;
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, 18, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("331613390521673906", std::string(buffer.data()));
    EXPECT_EQ(-236, point);

    v = 7.9885183916008099497815232e+191;
    status = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, 4, buffer, &length, &point);
    EXPECT_EQ(status, true);
    EXPECT_EQ("7989", std::string(buffer.data()));
    EXPECT_EQ(192, point);
    }


TEST(FastDtoa, FastDtoaGayShortest) {
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool status;
    int length;
    int point;
    int succeeded = 0;
    int total = 0;
    bool needed_max_length = false;

    walle::BufferRef<const PrecomputedShortest> precomputed =
        precomputed_shortest_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortest current_test = precomputed[i];
        total++;
        double v = current_test.v;
        status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortest, 0, buffer, &length, &point);
        EXPECT_EQ(kFastDtoaMaximalLength >= length, true);
        if (!status) continue;
        if (length == kFastDtoaMaximalLength) needed_max_length = true;
        succeeded++;
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
    EXPECT_EQ(succeeded*1.0/total > 0.99, true);
    EXPECT_EQ(needed_max_length, true);
}


TEST(FastDtoa, FastDtoaGayShortestSingle) {
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool status;
    int length;
    int point;
    int succeeded = 0;
    int total = 0;
    bool needed_max_length = false;

    walle::BufferRef<const PrecomputedShortestSingle> precomputed =
        precomputed_shortest_single_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedShortestSingle current_test = precomputed[i];
        total++;
        float v = current_test.v;
        status = fast_dtoa(v, FastDtoaMode::eFastDtoaShortestSingle, 0, buffer, &length, &point);
        EXPECT_EQ(kFastDtoaMaximalSingleLength >= length, true);
        if (!status) continue;
        if (length == kFastDtoaMaximalSingleLength) needed_max_length = true;
        succeeded++;
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
    EXPECT_EQ(succeeded*1.0/total > 0.98, true);
    EXPECT_EQ(needed_max_length, true);
    }


TEST(FastDtoa, FastDtoaGayPrecision) {
    char buffer_container[kBufferSize];
    walle::BufferRef<char> buffer(buffer_container, kBufferSize);
    bool status;
    int length;
    int point;
    int succeeded = 0;
    int total = 0;
    // Count separately for entries with less than 15 requested digits.
    int succeeded_15 = 0;
    int total_15 = 0;

    walle::BufferRef<const PrecomputedPrecision> precomputed =
        precomputed_precision_representations();
    for (int i = 0; i < precomputed.size(); ++i) {
        const PrecomputedPrecision current_test = precomputed[i];
        double v = current_test.v;
        int number_digits = current_test.number_digits;
        total++;
        if (number_digits <= 15) total_15++;
        status = fast_dtoa(v, FastDtoaMode::eFastDtoaPrecision, number_digits,
                        buffer, &length, &point);
        EXPECT_EQ(number_digits >= length, true);
        if (!status) continue;
        succeeded++;
        if (number_digits <= 15) succeeded_15++;
        trim_representation(buffer);
        EXPECT_EQ(current_test.decimal_point, point);
        EXPECT_EQ(current_test.representation, std::string(buffer.data()));
    }
    // The precomputed numbers contain many entries with many requested
    // digits. These have a high failure rate and we therefore expect a lower
    // success rate than for the shortest representation.
    EXPECT_EQ(succeeded*1.0/total > 0.85, true);
    // However with less than 15 digits almost the algorithm should almost always
    // succeed.
    EXPECT_EQ(succeeded_15*1.0/total_15 > 0.9999, true);
}
