#include <walle/format/detail/simple_fp.h>
#include <walle/format/detail/bignum.h>
#include <walle/format/detail/strtod.h>
#include <walle/format/detail/ieee.h>
#include <walle/math/math.h>
#include <walle/config/base.h>
#include <google/gtest/gtest.h>
#include <cstdlib>


using namespace walle::format_detail;

static walle::BufferRef<const char> string_to_buffer_ref(const char* str) 
{
    return walle::BufferRef<const char>(str, strlen(str));
}


static double strtod_char(const char* str, int exponent) 
{
  return str_to_double(string_to_buffer_ref(str), exponent);
}


static float strtof_char(const char* str, int exponent) 
{
  return str_to_float(string_to_buffer_ref(str), exponent);
}


TEST(strtod,strtod) {
  walle::BufferRef<const char> vector;

  vector = string_to_buffer_ref("0");
  EXPECT_EQ(0.0, str_to_double(vector, 1));
  EXPECT_EQ(0.0, str_to_double(vector, 2));
  EXPECT_EQ(0.0, str_to_double(vector, -2));
  EXPECT_EQ(0.0, str_to_double(vector, -999));
  EXPECT_EQ(0.0, str_to_double(vector, +999));

  vector = string_to_buffer_ref("1");
  EXPECT_EQ(1.0, str_to_double(vector, 0));
  EXPECT_EQ(10.0, str_to_double(vector, 1));
  EXPECT_EQ(100.0, str_to_double(vector, 2));
  EXPECT_EQ(1e20, str_to_double(vector, 20));
  EXPECT_EQ(1e22, str_to_double(vector, 22));
  EXPECT_EQ(1e23, str_to_double(vector, 23));
  EXPECT_EQ(1e35, str_to_double(vector, 35));
  EXPECT_EQ(1e36, str_to_double(vector, 36));
  EXPECT_EQ(1e37, str_to_double(vector, 37));
  EXPECT_EQ(1e-1, str_to_double(vector, -1));
  EXPECT_EQ(1e-2, str_to_double(vector, -2));
  EXPECT_EQ(1e-5, str_to_double(vector, -5));
  EXPECT_EQ(1e-20, str_to_double(vector, -20));
  EXPECT_EQ(1e-22, str_to_double(vector, -22));
  EXPECT_EQ(1e-23, str_to_double(vector, -23));
  EXPECT_EQ(1e-25, str_to_double(vector, -25));
  EXPECT_EQ(1e-39, str_to_double(vector, -39));

  vector = string_to_buffer_ref("2");
  EXPECT_EQ(2.0, str_to_double(vector, 0));
  EXPECT_EQ(20.0, str_to_double(vector, 1));
  EXPECT_EQ(200.0, str_to_double(vector, 2));
  EXPECT_EQ(2e20, str_to_double(vector, 20));
  EXPECT_EQ(2e22, str_to_double(vector, 22));
  EXPECT_EQ(2e23, str_to_double(vector, 23));
  EXPECT_EQ(2e35, str_to_double(vector, 35));
  EXPECT_EQ(2e36, str_to_double(vector, 36));
  EXPECT_EQ(2e37, str_to_double(vector, 37));
  EXPECT_EQ(2e-1, str_to_double(vector, -1));
  EXPECT_EQ(2e-2, str_to_double(vector, -2));
  EXPECT_EQ(2e-5, str_to_double(vector, -5));
  EXPECT_EQ(2e-20, str_to_double(vector, -20));
  EXPECT_EQ(2e-22, str_to_double(vector, -22));
  EXPECT_EQ(2e-23, str_to_double(vector, -23));
  EXPECT_EQ(2e-25, str_to_double(vector, -25));
  EXPECT_EQ(2e-39, str_to_double(vector, -39));

  vector = string_to_buffer_ref("9");
  EXPECT_EQ(9.0, str_to_double(vector, 0));
  EXPECT_EQ(90.0, str_to_double(vector, 1));
  EXPECT_EQ(900.0, str_to_double(vector, 2));
  EXPECT_EQ(9e20, str_to_double(vector, 20));
  EXPECT_EQ(9e22, str_to_double(vector, 22));
  EXPECT_EQ(9e23, str_to_double(vector, 23));
  EXPECT_EQ(9e35, str_to_double(vector, 35));
  EXPECT_EQ(9e36, str_to_double(vector, 36));
  EXPECT_EQ(9e37, str_to_double(vector, 37));
  EXPECT_EQ(9e-1, str_to_double(vector, -1));
  EXPECT_EQ(9e-2, str_to_double(vector, -2));
  EXPECT_EQ(9e-5, str_to_double(vector, -5));
  EXPECT_EQ(9e-20, str_to_double(vector, -20));
  EXPECT_EQ(9e-22, str_to_double(vector, -22));
  EXPECT_EQ(9e-23, str_to_double(vector, -23));
  EXPECT_EQ(9e-25, str_to_double(vector, -25));
  EXPECT_EQ(9e-39, str_to_double(vector, -39));

  vector = string_to_buffer_ref("12345");
  EXPECT_EQ(12345.0, str_to_double(vector, 0));
  EXPECT_EQ(123450.0, str_to_double(vector, 1));
  EXPECT_EQ(1234500.0, str_to_double(vector, 2));
  EXPECT_EQ(12345e20, str_to_double(vector, 20));
  EXPECT_EQ(12345e22, str_to_double(vector, 22));
  EXPECT_EQ(12345e23, str_to_double(vector, 23));
  EXPECT_EQ(12345e30, str_to_double(vector, 30));
  EXPECT_EQ(12345e31, str_to_double(vector, 31));
  EXPECT_EQ(12345e32, str_to_double(vector, 32));
  EXPECT_EQ(12345e35, str_to_double(vector, 35));
  EXPECT_EQ(12345e36, str_to_double(vector, 36));
  EXPECT_EQ(12345e37, str_to_double(vector, 37));
  EXPECT_EQ(12345e-1, str_to_double(vector, -1));
  EXPECT_EQ(12345e-2, str_to_double(vector, -2));
  EXPECT_EQ(12345e-5, str_to_double(vector, -5));
  EXPECT_EQ(12345e-20, str_to_double(vector, -20));
  EXPECT_EQ(12345e-22, str_to_double(vector, -22));
  EXPECT_EQ(12345e-23, str_to_double(vector, -23));
  EXPECT_EQ(12345e-25, str_to_double(vector, -25));
  EXPECT_EQ(12345e-39, str_to_double(vector, -39));

  vector = string_to_buffer_ref("12345678901234");
  EXPECT_EQ(12345678901234.0, str_to_double(vector, 0));
  EXPECT_EQ(123456789012340.0, str_to_double(vector, 1));
  EXPECT_EQ(1234567890123400.0, str_to_double(vector, 2));
  EXPECT_EQ(12345678901234e20, str_to_double(vector, 20));
  EXPECT_EQ(12345678901234e22, str_to_double(vector, 22));
  EXPECT_EQ(12345678901234e23, str_to_double(vector, 23));
  EXPECT_EQ(12345678901234e30, str_to_double(vector, 30));
  EXPECT_EQ(12345678901234e31, str_to_double(vector, 31));
  EXPECT_EQ(12345678901234e32, str_to_double(vector, 32));
  EXPECT_EQ(12345678901234e35, str_to_double(vector, 35));
  EXPECT_EQ(12345678901234e36, str_to_double(vector, 36));
  EXPECT_EQ(12345678901234e37, str_to_double(vector, 37));
  EXPECT_EQ(12345678901234e-1, str_to_double(vector, -1));
  EXPECT_EQ(12345678901234e-2, str_to_double(vector, -2));
  EXPECT_EQ(12345678901234e-5, str_to_double(vector, -5));
  EXPECT_EQ(12345678901234e-20, str_to_double(vector, -20));
  EXPECT_EQ(12345678901234e-22, str_to_double(vector, -22));
  EXPECT_EQ(12345678901234e-23, str_to_double(vector, -23));
  EXPECT_EQ(12345678901234e-25, str_to_double(vector, -25));
  EXPECT_EQ(12345678901234e-39, str_to_double(vector, -39));

  vector = string_to_buffer_ref("123456789012345");
  EXPECT_EQ(123456789012345.0, str_to_double(vector, 0));
  EXPECT_EQ(1234567890123450.0, str_to_double(vector, 1));
  EXPECT_EQ(12345678901234500.0, str_to_double(vector, 2));
  EXPECT_EQ(123456789012345e20, str_to_double(vector, 20));
  EXPECT_EQ(123456789012345e22, str_to_double(vector, 22));
  EXPECT_EQ(123456789012345e23, str_to_double(vector, 23));
  EXPECT_EQ(123456789012345e35, str_to_double(vector, 35));
  EXPECT_EQ(123456789012345e36, str_to_double(vector, 36));
  EXPECT_EQ(123456789012345e37, str_to_double(vector, 37));
  EXPECT_EQ(123456789012345e39, str_to_double(vector, 39));
  EXPECT_EQ(123456789012345e-1, str_to_double(vector, -1));
  EXPECT_EQ(123456789012345e-2, str_to_double(vector, -2));
  EXPECT_EQ(123456789012345e-5, str_to_double(vector, -5));
  EXPECT_EQ(123456789012345e-20, str_to_double(vector, -20));
  EXPECT_EQ(123456789012345e-22, str_to_double(vector, -22));
  EXPECT_EQ(123456789012345e-23, str_to_double(vector, -23));
  EXPECT_EQ(123456789012345e-25, str_to_double(vector, -25));
  EXPECT_EQ(123456789012345e-39, str_to_double(vector, -39));

  EXPECT_EQ(0.0, strtod_char("0", 12345));
  EXPECT_EQ(0.0, strtod_char("", 1324));
  EXPECT_EQ(0.0, strtod_char("000000000", 123));
  EXPECT_EQ(0.0, strtod_char("2", -324));
  EXPECT_EQ(4e-324, strtod_char("3", -324));
  // It would be more readable to put non-zero literals on the left side (i.e.
  //   EXPECT_EQ(1e-325, strtod_char("1", -325))), but then Gcc complains that
  // they are truncated to zero.
  EXPECT_EQ(0.0, strtod_char("1", -325));
  EXPECT_EQ(0.0, strtod_char("1", -325));
  EXPECT_EQ(0.0, strtod_char("20000", -328));
  EXPECT_EQ(40000e-328, strtod_char("30000", -328));
  EXPECT_EQ(0.0, strtod_char("10000", -329));
  EXPECT_EQ(0.0, strtod_char("90000", -329));
  EXPECT_EQ(0.0, strtod_char("000000001", -325));
  EXPECT_EQ(0.0, strtod_char("000000001", -325));
  EXPECT_EQ(0.0, strtod_char("0000000020000", -328));
  EXPECT_EQ(40000e-328, strtod_char("00000030000", -328));
  EXPECT_EQ(0.0, strtod_char("0000000010000", -329));
  EXPECT_EQ(0.0, strtod_char("0000000090000", -329));

  // It would be more readable to put the literals (and not Double::infinity())
  // on the left side (i.e. EXPECT_EQ(1e309, strtod_char("1", 309))), but then Gcc
  // complains that the floating constant exceeds range of 'double'.
  EXPECT_EQ(Double::infinity(), strtod_char("1", 309));
  EXPECT_EQ(1e308, strtod_char("1", 308));
  EXPECT_EQ(1234e305, strtod_char("1234", 305));
  EXPECT_EQ(1234e304, strtod_char("1234", 304));
  EXPECT_EQ(Double::infinity(), strtod_char("18", 307));
  EXPECT_EQ(17e307, strtod_char("17", 307));
  EXPECT_EQ(Double::infinity(), strtod_char("0000001", 309));
  EXPECT_EQ(1e308, strtod_char("00000001", 308));
  EXPECT_EQ(1234e305, strtod_char("00000001234", 305));
  EXPECT_EQ(1234e304, strtod_char("000000001234", 304));
  EXPECT_EQ(Double::infinity(), strtod_char("0000000018", 307));
  EXPECT_EQ(17e307, strtod_char("0000000017", 307));
  EXPECT_EQ(Double::infinity(), strtod_char("1000000", 303));
  EXPECT_EQ(1e308, strtod_char("100000", 303));
  EXPECT_EQ(1234e305, strtod_char("123400000", 300));
  EXPECT_EQ(1234e304, strtod_char("123400000", 299));
  EXPECT_EQ(Double::infinity(), strtod_char("180000000", 300));
  EXPECT_EQ(17e307, strtod_char("170000000", 300));
  EXPECT_EQ(Double::infinity(), strtod_char("00000001000000", 303));
  EXPECT_EQ(1e308, strtod_char("000000000000100000", 303));
  EXPECT_EQ(1234e305, strtod_char("00000000123400000", 300));
  EXPECT_EQ(1234e304, strtod_char("0000000123400000", 299));
  EXPECT_EQ(Double::infinity(), strtod_char("00000000180000000", 300));
  EXPECT_EQ(17e307, strtod_char("00000000170000000", 300));
  EXPECT_EQ(1.7976931348623157E+308, strtod_char("17976931348623157", 292));
  EXPECT_EQ(1.7976931348623158E+308, strtod_char("17976931348623158", 292));
  EXPECT_EQ(Double::infinity(), strtod_char("17976931348623159", 292));

  // The following number is the result of 89255.0/1e-22. Both floating-point
  // numbers can be accurately represented with doubles. However on Linux,x86
  // the floating-point stack is set to 80bits and the double-rounding
  // introduces an error.
  EXPECT_EQ(89255e-22, strtod_char("89255", -22));

  // Some random values.
  EXPECT_EQ(358416272e-33, strtod_char("358416272", -33));
  EXPECT_EQ(104110013277974872254e-225,
           strtod_char("104110013277974872254", -225));

  EXPECT_EQ(123456789e108, strtod_char("123456789", 108));
  EXPECT_EQ(123456789e109, strtod_char("123456789", 109));
  EXPECT_EQ(123456789e110, strtod_char("123456789", 110));
  EXPECT_EQ(123456789e111, strtod_char("123456789", 111));
  EXPECT_EQ(123456789e112, strtod_char("123456789", 112));
  EXPECT_EQ(123456789e113, strtod_char("123456789", 113));
  EXPECT_EQ(123456789e114, strtod_char("123456789", 114));
  EXPECT_EQ(123456789e115, strtod_char("123456789", 115));

  EXPECT_EQ(1234567890123456789012345e108,
           strtod_char("1234567890123456789012345", 108));
  EXPECT_EQ(1234567890123456789012345e109,
           strtod_char("1234567890123456789012345", 109));
  EXPECT_EQ(1234567890123456789012345e110,
           strtod_char("1234567890123456789012345", 110));
  EXPECT_EQ(1234567890123456789012345e111,
           strtod_char("1234567890123456789012345", 111));
  EXPECT_EQ(1234567890123456789012345e112,
           strtod_char("1234567890123456789012345", 112));
  EXPECT_EQ(1234567890123456789012345e113,
           strtod_char("1234567890123456789012345", 113));
  EXPECT_EQ(1234567890123456789012345e114,
           strtod_char("1234567890123456789012345", 114));
  EXPECT_EQ(1234567890123456789012345e115,
           strtod_char("1234567890123456789012345", 115));

  EXPECT_EQ(1234567890123456789052345e108,
           strtod_char("1234567890123456789052345", 108));
  EXPECT_EQ(1234567890123456789052345e109,
           strtod_char("1234567890123456789052345", 109));
  EXPECT_EQ(1234567890123456789052345e110,
           strtod_char("1234567890123456789052345", 110));
  EXPECT_EQ(1234567890123456789052345e111,
           strtod_char("1234567890123456789052345", 111));
  EXPECT_EQ(1234567890123456789052345e112,
           strtod_char("1234567890123456789052345", 112));
  EXPECT_EQ(1234567890123456789052345e113,
           strtod_char("1234567890123456789052345", 113));
  EXPECT_EQ(1234567890123456789052345e114,
           strtod_char("1234567890123456789052345", 114));
  EXPECT_EQ(1234567890123456789052345e115,
           strtod_char("1234567890123456789052345", 115));

  EXPECT_EQ(5.445618932859895e-255,
           strtod_char("5445618932859895362967233318697132813618813095743952975"
                      "4392982234069699615600475529427176366709107287468930197"
                      "8628345413991790019316974825934906752493984055268219809"
                      "5012176093045431437495773903922425632551857520884625114"
                      "6241265881735209066709685420744388526014389929047617597"
                      "0302268848374508109029268898695825171158085457567481507"
                      "4162979705098246243690189880319928315307816832576838178"
                      "2563074014542859888710209237525873301724479666744537857"
                      "9026553346649664045621387124193095870305991178772256504"
                      "4368663670643970181259143319016472430928902201239474588"
                      "1392338901353291306607057623202353588698746085415097902"
                      "6640064319118728664842287477491068264828851624402189317"
                      "2769161449825765517353755844373640588822904791244190695"
                      "2998382932630754670573838138825217065450843010498555058"
                      "88186560731", -1035));

  // Boundary cases. Boundaries themselves should round to even.
  //
  // 0x1FFFFFFFFFFFF * 2^3 = 72057594037927928
  //                   next: 72057594037927936
  //               boundary: 72057594037927932  should round up.
  EXPECT_EQ(72057594037927928.0, strtod_char("72057594037927928", 0));
  EXPECT_EQ(72057594037927936.0, strtod_char("72057594037927936", 0));
  EXPECT_EQ(72057594037927936.0, strtod_char("72057594037927932", 0));
  EXPECT_EQ(72057594037927928.0, strtod_char("7205759403792793199999", -5));
  EXPECT_EQ(72057594037927936.0, strtod_char("7205759403792793200001", -5));

  // 0x1FFFFFFFFFFFF * 2^10 = 9223372036854774784
  //                    next: 9223372036854775808
  //                boundary: 9223372036854775296 should round up.
  EXPECT_EQ(9223372036854774784.0, strtod_char("9223372036854774784", 0));
  EXPECT_EQ(9223372036854775808.0, strtod_char("9223372036854775808", 0));
  EXPECT_EQ(9223372036854775808.0, strtod_char("9223372036854775296", 0));
  EXPECT_EQ(9223372036854774784.0, strtod_char("922337203685477529599999", -5));
  EXPECT_EQ(9223372036854775808.0, strtod_char("922337203685477529600001", -5));

  // 0x1FFFFFFFFFFFF * 2^50 = 10141204801825834086073718800384
  //                    next: 10141204801825835211973625643008
  //                boundary: 10141204801825834649023672221696 should round up.
  EXPECT_EQ(10141204801825834086073718800384.0,
           strtod_char("10141204801825834086073718800384", 0));
  EXPECT_EQ(10141204801825835211973625643008.0,
           strtod_char("10141204801825835211973625643008", 0));
  EXPECT_EQ(10141204801825835211973625643008.0,
           strtod_char("10141204801825834649023672221696", 0));
  EXPECT_EQ(10141204801825834086073718800384.0,
           strtod_char("1014120480182583464902367222169599999", -5));
  EXPECT_EQ(10141204801825835211973625643008.0,
           strtod_char("1014120480182583464902367222169600001", -5));

  // 0x1FFFFFFFFFFFF * 2^99 = 5708990770823838890407843763683279797179383808
  //                    next: 5708990770823839524233143877797980545530986496
  //                boundary: 5708990770823839207320493820740630171355185152
  // The boundary should round up.
  EXPECT_EQ(5708990770823838890407843763683279797179383808.0,
           strtod_char("5708990770823838890407843763683279797179383808", 0));
  EXPECT_EQ(5708990770823839524233143877797980545530986496.0,
           strtod_char("5708990770823839524233143877797980545530986496", 0));
  EXPECT_EQ(5708990770823839524233143877797980545530986496.0,
           strtod_char("5708990770823839207320493820740630171355185152", 0));
  EXPECT_EQ(5708990770823838890407843763683279797179383808.0,
           strtod_char("5708990770823839207320493820740630171355185151999", -3));
  EXPECT_EQ(5708990770823839524233143877797980545530986496.0,
           strtod_char("5708990770823839207320493820740630171355185152001", -3));

  // The following test-cases got some public attention in early 2011 when they
  // sent Java and PHP into an infinite loop.
  EXPECT_EQ(2.225073858507201e-308, strtod_char("22250738585072011", -324));
  EXPECT_EQ(2.22507385850720138309e-308,
           strtod_char("22250738585072011360574097967091319759348195463516456480"
                      "23426109724822222021076945516529523908135087914149158913"
                      "03962110687008643869459464552765720740782062174337998814"
                      "10632673292535522868813721490129811224514518898490572223"
                      "07285255133155755015914397476397983411801999323962548289"
                      "01710708185069063066665599493827577257201576306269066333"
                      "26475653000092458883164330377797918696120494973903778297"
                      "04905051080609940730262937128958950003583799967207254304"
                      "36028407889577179615094551674824347103070260914462157228"
                      "98802581825451803257070188608721131280795122334262883686"
                      "22321503775666622503982534335974568884423900265498198385"
                      "48794829220689472168983109969836584681402285424333066033"
                      "98508864458040010349339704275671864433837704860378616227"
                      "71738545623065874679014086723327636718751", -1076));
}


TEST(strtod,strtof) {
  walle::BufferRef<const char> vector;

  vector = string_to_buffer_ref("0");
  EXPECT_EQ(0.0f, str_to_float(vector, 1));
  EXPECT_EQ(0.0f, str_to_float(vector, 2));
  EXPECT_EQ(0.0f, str_to_float(vector, -2));
  EXPECT_EQ(0.0f, str_to_float(vector, -999));
  EXPECT_EQ(0.0f, str_to_float(vector, +999));

  vector = string_to_buffer_ref("1");
  EXPECT_EQ(1.0f, str_to_float(vector, 0));
  EXPECT_EQ(10.0f, str_to_float(vector, 1));
  EXPECT_EQ(100.0f, str_to_float(vector, 2));
  EXPECT_EQ(1e20f, str_to_float(vector, 20));
  EXPECT_EQ(1e22f, str_to_float(vector, 22));
  EXPECT_EQ(1e23f, str_to_float(vector, 23));
  EXPECT_EQ(1e35f, str_to_float(vector, 35));
  EXPECT_EQ(1e36f, str_to_float(vector, 36));
  EXPECT_EQ(1e37f, str_to_float(vector, 37));
  EXPECT_EQ(1e-1f, str_to_float(vector, -1));
  EXPECT_EQ(1e-2f, str_to_float(vector, -2));
  EXPECT_EQ(1e-5f, str_to_float(vector, -5));
  EXPECT_EQ(1e-20f, str_to_float(vector, -20));
  EXPECT_EQ(1e-22f, str_to_float(vector, -22));
  EXPECT_EQ(1e-23f, str_to_float(vector, -23));
  EXPECT_EQ(1e-25f, str_to_float(vector, -25));
  EXPECT_EQ(1e-39f, str_to_float(vector, -39));

  vector = string_to_buffer_ref("2");
  EXPECT_EQ(2.0f, str_to_float(vector, 0));
  EXPECT_EQ(20.0f, str_to_float(vector, 1));
  EXPECT_EQ(200.0f, str_to_float(vector, 2));
  EXPECT_EQ(2e20f, str_to_float(vector, 20));
  EXPECT_EQ(2e22f, str_to_float(vector, 22));
  EXPECT_EQ(2e23f, str_to_float(vector, 23));
  EXPECT_EQ(2e35f, str_to_float(vector, 35));
  EXPECT_EQ(2e36f, str_to_float(vector, 36));
  EXPECT_EQ(2e37f, str_to_float(vector, 37));
  EXPECT_EQ(2e-1f, str_to_float(vector, -1));
  EXPECT_EQ(2e-2f, str_to_float(vector, -2));
  EXPECT_EQ(2e-5f, str_to_float(vector, -5));
  EXPECT_EQ(2e-20f, str_to_float(vector, -20));
  EXPECT_EQ(2e-22f, str_to_float(vector, -22));
  EXPECT_EQ(2e-23f, str_to_float(vector, -23));
  EXPECT_EQ(2e-25f, str_to_float(vector, -25));
  EXPECT_EQ(2e-39f, str_to_float(vector, -39));

  vector = string_to_buffer_ref("9");
  EXPECT_EQ(9.0f, str_to_float(vector, 0));
  EXPECT_EQ(90.0f, str_to_float(vector, 1));
  EXPECT_EQ(900.0f, str_to_float(vector, 2));
  EXPECT_EQ(9e20f, str_to_float(vector, 20));
  EXPECT_EQ(9e22f, str_to_float(vector, 22));
  EXPECT_EQ(9e23f, str_to_float(vector, 23));
  EXPECT_EQ(9e35f, str_to_float(vector, 35));
  EXPECT_EQ(9e36f, str_to_float(vector, 36));
  EXPECT_EQ(9e37f, str_to_float(vector, 37));
  EXPECT_EQ(9e-1f, str_to_float(vector, -1));
  EXPECT_EQ(9e-2f, str_to_float(vector, -2));
  EXPECT_EQ(9e-5f, str_to_float(vector, -5));
  EXPECT_EQ(9e-20f, str_to_float(vector, -20));
  EXPECT_EQ(9e-22f, str_to_float(vector, -22));
  EXPECT_EQ(9e-23f, str_to_float(vector, -23));
  EXPECT_EQ(9e-25f, str_to_float(vector, -25));
  EXPECT_EQ(9e-39f, str_to_float(vector, -39));

  vector = string_to_buffer_ref("12345");
  EXPECT_EQ(12345.0f, str_to_float(vector, 0));
  EXPECT_EQ(123450.0f, str_to_float(vector, 1));
  EXPECT_EQ(1234500.0f, str_to_float(vector, 2));
  EXPECT_EQ(12345e20f, str_to_float(vector, 20));
  EXPECT_EQ(12345e22f, str_to_float(vector, 22));
  EXPECT_EQ(12345e23f, str_to_float(vector, 23));
  EXPECT_EQ(12345e30f, str_to_float(vector, 30));
  EXPECT_EQ(12345e31f, str_to_float(vector, 31));
  EXPECT_EQ(12345e32f, str_to_float(vector, 32));
  EXPECT_EQ(12345e-1f, str_to_float(vector, -1));
  EXPECT_EQ(12345e-2f, str_to_float(vector, -2));
  EXPECT_EQ(12345e-5f, str_to_float(vector, -5));
  EXPECT_EQ(12345e-20f, str_to_float(vector, -20));
  EXPECT_EQ(12345e-22f, str_to_float(vector, -22));
  EXPECT_EQ(12345e-23f, str_to_float(vector, -23));
  EXPECT_EQ(12345e-25f, str_to_float(vector, -25));
  EXPECT_EQ(12345e-39f, str_to_float(vector, -39));

  vector = string_to_buffer_ref("12345678901234");
  EXPECT_EQ(12345678901234.0f, str_to_float(vector, 0));
  EXPECT_EQ(123456789012340.0f, str_to_float(vector, 1));
  EXPECT_EQ(1234567890123400.0f, str_to_float(vector, 2));
  EXPECT_EQ(12345678901234e20f, str_to_float(vector, 20));
  EXPECT_EQ(12345678901234e22f, str_to_float(vector, 22));
  EXPECT_EQ(12345678901234e23f, str_to_float(vector, 23));
  EXPECT_EQ(12345678901234e-1f, str_to_float(vector, -1));
  EXPECT_EQ(12345678901234e-2f, str_to_float(vector, -2));
  EXPECT_EQ(12345678901234e-5f, str_to_float(vector, -5));
  EXPECT_EQ(12345678901234e-20f, str_to_float(vector, -20));
  EXPECT_EQ(12345678901234e-22f, str_to_float(vector, -22));
  EXPECT_EQ(12345678901234e-23f, str_to_float(vector, -23));
  EXPECT_EQ(12345678901234e-25f, str_to_float(vector, -25));
  EXPECT_EQ(12345678901234e-39f, str_to_float(vector, -39));

  vector = string_to_buffer_ref("123456789012345");
  EXPECT_EQ(123456789012345.0f, str_to_float(vector, 0));
  EXPECT_EQ(1234567890123450.0f, str_to_float(vector, 1));
  EXPECT_EQ(12345678901234500.0f, str_to_float(vector, 2));
  EXPECT_EQ(123456789012345e20f, str_to_float(vector, 20));
  EXPECT_EQ(123456789012345e22f, str_to_float(vector, 22));
  EXPECT_EQ(123456789012345e23f, str_to_float(vector, 23));
  EXPECT_EQ(123456789012345e-1f, str_to_float(vector, -1));
  EXPECT_EQ(123456789012345e-2f, str_to_float(vector, -2));
  EXPECT_EQ(123456789012345e-5f, str_to_float(vector, -5));
  EXPECT_EQ(123456789012345e-20f, str_to_float(vector, -20));
  EXPECT_EQ(123456789012345e-22f, str_to_float(vector, -22));
  EXPECT_EQ(123456789012345e-23f, str_to_float(vector, -23));
  EXPECT_EQ(123456789012345e-25f, str_to_float(vector, -25));
  EXPECT_EQ(123456789012345e-39f, str_to_float(vector, -39));

  EXPECT_EQ(0.0f, strtof_char("0", 12345));
  EXPECT_EQ(0.0f, strtof_char("", 1324));
  EXPECT_EQ(0.0f, strtof_char("000000000", 123));
  EXPECT_EQ(0.0f, strtof_char("2", -324));
  EXPECT_EQ(1e-45f, strtof_char("1", -45));
  // It would be more readable to put non-zero literals on the left side (i.e.
  //   EXPECT_EQ(1e-46, strtof_char("1", -45))), but then Gcc complains that
  // they are truncated to zero.
  EXPECT_EQ(0.0f, strtof_char("1", -46));
  EXPECT_EQ(0.0f, strtof_char("1", -47));
  EXPECT_EQ(1e-45f, strtof_char("1", -45));
  EXPECT_EQ(1e-45f, strtof_char("8", -46));
  EXPECT_EQ(0.0f, strtof_char("200000", -51));
  EXPECT_EQ(100000e-50f, strtof_char("100000", -50));
  EXPECT_EQ(0.0f, strtof_char("100000", -51));
  EXPECT_EQ(0.0f, strtof_char("900000", -52));
  EXPECT_EQ(0.0f, strtof_char("000000001", -47));
  EXPECT_EQ(0.0f, strtof_char("000000001", -47));
  EXPECT_EQ(0.0f, strtof_char("00000000200000", -51));
  EXPECT_EQ(800000e-50f, strtof_char("000000800000", -50));
  EXPECT_EQ(0.0f, strtof_char("00000000100000", -51));
  EXPECT_EQ(1e-45f, strtof_char("00000000900000", -51));

  // It would be more readable to put the literals (and not Double::infinity())
  // on the left side (i.e. EXPECT_EQ(3e38, strtof_char("3", 38))), but then Gcc
  // complains that the floating constant exceeds range of 'double'.
  EXPECT_EQ(Single::infinity(), strtof_char("3", 39));
  EXPECT_EQ(3e38f, strtof_char("3", 38));
  EXPECT_EQ(3401e35f, strtof_char("3401", 35));
  EXPECT_EQ(3401e34f, strtof_char("3401", 34));
  EXPECT_EQ(Single::infinity(), strtof_char("3410", 35));
  EXPECT_EQ(34e37f, strtof_char("34", 37));
  EXPECT_EQ(Single::infinity(), strtof_char("0000001", 39));
  EXPECT_EQ(3401e35f, strtof_char("0000003401", 35));
  EXPECT_EQ(3401e34f, strtof_char("0000003401", 34));
  EXPECT_EQ(Single::infinity(), strtof_char("0000003410", 35));
  EXPECT_EQ(34e37f, strtof_char("00000034", 37));
  EXPECT_EQ(1e38f, strtof_char("100000", 33));
  EXPECT_EQ(3401e35f, strtof_char("340100000", 30));
  EXPECT_EQ(3401e34f, strtof_char("340100000", 29));
  EXPECT_EQ(Single::infinity(), strtof_char("341000000", 30));
  EXPECT_EQ(34e37f, strtof_char("3400000", 32));
  EXPECT_EQ(1e38f, strtof_char("00000100000", 33));
  EXPECT_EQ(3401e35f, strtof_char("00000340100000", 30));
  EXPECT_EQ(3401e34f, strtof_char("00000340100000", 29));
  EXPECT_EQ(Single::infinity(), strtof_char("00000341000000", 30));
  EXPECT_EQ(34e37f, strtof_char("000003400000", 32));
  EXPECT_EQ(3.4028234e+38f, strtof_char("34028235676", 28));
  EXPECT_EQ(3.4028234e+38f, strtof_char("34028235677", 28));
  EXPECT_EQ(Single::infinity(), strtof_char("34028235678", 28));

  // The following number is the result of 89255.0/1e-22. Both floating-point
  // numbers can be accurately represented with doubles. However on Linux,x86
  // the floating-point stack is set to 80bits and the double-rounding
  // introduces an error.
  EXPECT_EQ(89255e-22f, strtof_char("89255", -22));

  // Boundary cases. Boundaries themselves should round to even.
  //
  // 0x4f012334 = 2166567936
  //      next:   2166568192
  //  boundary:   2166568064 should round down.
  EXPECT_EQ(2166567936.0f, strtof_char("2166567936", 0));
  EXPECT_EQ(2166568192.0f, strtof_char("2166568192", 0));
  EXPECT_EQ(2166567936.0f, strtof_char("2166568064", 0));
  EXPECT_EQ(2166567936.0f, strtof_char("216656806399999", -5));
  EXPECT_EQ(2166568192.0f, strtof_char("216656806400001", -5));
  // Verify that we don't double round.
  // Get the boundary of the boundary.
  EXPECT_EQ(2.1665680640000002384185791015625e9, 2166568064.0);
  // Visual Studio gets this wrong and believes that these two numbers are the
  // same doubles. We want to test our conversion and not the compiler. We
  // therefore disable the check.

  EXPECT_EQ(2166568192.0f, strtof_char("21665680640000002384185791015625", -22));

  // 0x4fffffff = 8589934080
  //      next:   8589934592
  //  boundary:   8589934336 should round up.
  EXPECT_EQ(8589934080.0f, strtof_char("8589934080", 0));
  EXPECT_EQ(8589934592.0f, strtof_char("8589934592", 0));
  EXPECT_EQ(8589934592.0f, strtof_char("8589934336", 0));
  EXPECT_EQ(8589934080.0f, strtof_char("858993433599999", -5));
  EXPECT_EQ(8589934592.0f, strtof_char("858993433600001", -5));
  // Verify that we don't double round.
  // Get the boundary of the boundary.
  // Visual Studio gets this wrong. To avoid failing tests because of a broken
  // compiler we disable the following two tests. They were only testing the
  // compiler. The real test is still active.

  EXPECT_EQ(8589934080.0f, strtof_char("8589934335999999523162841796875", -21));

  // 0x4f000000 = 2147483648
  //      next:   2147483904
  //  boundary:   2147483776 should round down.
  EXPECT_EQ(2147483648.0f, strtof_char("2147483648", 0));
  EXPECT_EQ(2147483904.0f, strtof_char("2147483904", 0));
  EXPECT_EQ(2147483648.0f, strtof_char("2147483776", 0));
  EXPECT_EQ(2147483648.0f, strtof_char("214748377599999", -5));
  EXPECT_EQ(2147483904.0f, strtof_char("214748377600001", -5));

}


static int compareBignumToSimpleFp(const Bignum& bignum_digits,
                                int bignum_exponent,
                                SimpleFp diy_fp) {
  Bignum bignum;
  bignum.assign_bignum(bignum_digits);
  Bignum other;
  other.assign_uint64(diy_fp.f());
  if (bignum_exponent >= 0) {
    bignum.multi_by_power_of_ten(bignum_exponent);
  } else {
    other.multi_by_power_of_ten(-bignum_exponent);
  }
  if (diy_fp.e() >= 0) {
    other.shift_left(diy_fp.e());
  } else {
    bignum.shift_left(-diy_fp.e());
  }
  return Bignum::compare(bignum, other);
}


static bool CheckDouble(walle::BufferRef<const char> buffer,
                        int exponent,
                        double to_check) {
  SimpleFp lower_boundary;
  SimpleFp upper_boundary;
  Bignum input_digits;
  input_digits.assign_decimal_string(buffer);
  if (to_check == 0.0) {
    const double kMinDouble = 4e-324;
    // Check that the buffer*10^exponent < (0 + kMinDouble)/2.
    Double d(kMinDouble);
    d.normalized_boundaries(&lower_boundary, &upper_boundary);
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) <= 0;
  }
  if (to_check == Double::infinity()) {
    const double kMaxDouble = 1.7976931348623157e308;
    // Check that the buffer*10^exponent >= boundary between kMaxDouble and inf.
    Double d(kMaxDouble);
    d.normalized_boundaries(&lower_boundary, &upper_boundary);
    return compareBignumToSimpleFp(input_digits, exponent, upper_boundary) >= 0;
  }
  Double d(to_check);
  d.normalized_boundaries(&lower_boundary, &upper_boundary);
  if ((d.significand() & 1) == 0) {
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) >= 0 &&
        compareBignumToSimpleFp(input_digits, exponent, upper_boundary) <= 0;
  } else {
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) > 0 &&
        compareBignumToSimpleFp(input_digits, exponent, upper_boundary) < 0;
  }
}


// Copied from v8.cc and adapted to make the function deterministic.
static uint32_t DeterministicRandom() {
  // Random number generator using George Marsaglia's MWC algorithm.
  static uint32_t hi = 0;
  static uint32_t lo = 0;

  // Initialization values don't have any special meaning. (They are the result
  // of two calls to random().)
  if (hi == 0) hi = 0xbfe166e7;
  if (lo == 0) lo = 0x64d1c3c9;

  // Mix the bits.
  hi = 36969 * (hi & 0xFFFF) + (hi >> 16);
  lo = 18273 * (lo & 0xFFFF) + (lo >> 16);
  return (hi << 16) + (lo & 0xFFFF);
}


static const int kBufferSize = 1024;
static const int kShortstrtodRandomCount = 2;
static const int kLargestrtodRandomCount = 2;

TEST(strtod,Randomstrtod) {
  char buffer[kBufferSize];
  for (int length = 1; length < 15; length++) {
    for (int i = 0; i < kShortstrtodRandomCount; ++i) {
      int pos = 0;
      for (int j = 0; j < length; ++j) {
        buffer[pos++] = DeterministicRandom() % 10 + '0';
      }
      int exponent = DeterministicRandom() % (25*2 + 1) - 25 - length;
      buffer[pos] = '\0';
      walle::BufferRef<const char> vector(buffer, pos);
      double strtod_result = str_to_double(vector, exponent);
      EXPECT_EQ(CheckDouble(vector, exponent, strtod_result), true);
    }
  }
  for (int length = 15; length < 800; length += 2) {
    for (int i = 0; i < kLargestrtodRandomCount; ++i) {
      int pos = 0;
      for (int j = 0; j < length; ++j) {
        buffer[pos++] = DeterministicRandom() % 10 + '0';
      }
      int exponent = DeterministicRandom() % (308*2 + 1) - 308 - length;
      buffer[pos] = '\0';
      walle::BufferRef<const char> vector(buffer, pos);
      double strtod_result = str_to_double(vector, exponent);
      EXPECT_EQ(CheckDouble(vector, exponent, strtod_result), true);
    }
  }
}
  

static bool CheckFloat(walle::BufferRef<const char> buffer,
                        int exponent,
                        float to_check) {
  SimpleFp lower_boundary;
  SimpleFp upper_boundary;
  Bignum input_digits;
  input_digits.assign_decimal_string(buffer);
  if (to_check == 0.0) {
    const float kMinFloat = 1e-45f;
    // Check that the buffer*10^exponent < (0 + kMinFloat)/2.
    Single s(kMinFloat);
    s.normalized_boundaries(&lower_boundary, &upper_boundary);
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) <= 0;
  }
  if (to_check == static_cast<float>(Double::infinity())) {
    const float kMaxFloat = 3.4028234e38f;
    // Check that the buffer*10^exponent >= boundary between kMaxFloat and inf.
    Single s(kMaxFloat);
    s.normalized_boundaries(&lower_boundary, &upper_boundary);
    return compareBignumToSimpleFp(input_digits, exponent, upper_boundary) >= 0;
  }
  Single s(to_check);
  s.normalized_boundaries(&lower_boundary, &upper_boundary);
  if ((s.significand() & 1) == 0) {
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) >= 0 &&
        compareBignumToSimpleFp(input_digits, exponent, upper_boundary) <= 0;
  } else {
    return compareBignumToSimpleFp(input_digits, exponent, lower_boundary) > 0 &&
        compareBignumToSimpleFp(input_digits, exponent, upper_boundary) < 0;
  }
}


static const int kShortstrtofRandomCount = 2;
static const int kLargestrtofRandomCount = 2;

TEST(strtod,Randomstrtof) {
    char buffer[kBufferSize];
    for (int length = 1; length < 15; length++) {
        for (int i = 0; i < kShortstrtofRandomCount; ++i) {
        int pos = 0;
        for (int j = 0; j < length; ++j) {
            buffer[pos++] = DeterministicRandom() % 10 + '0';
        }
        int exponent = DeterministicRandom() % (5*2 + 1) - 5 - length;
        buffer[pos] = '\0';
        walle::BufferRef<const char> vector(buffer, pos);
        float strtof_result = str_to_float(vector, exponent);
        EXPECT_EQ(CheckFloat(vector, exponent, strtof_result), true);
        }
    }
    for (int length = 15; length < 800; length += 2) {
        for (int i = 0; i < kLargestrtofRandomCount; ++i) {
            int pos = 0;
            for (int j = 0; j < length; ++j) {
                buffer[pos++] = DeterministicRandom() % 10 + '0';
            }
            int exponent = DeterministicRandom() % (38*2 + 1) - 38 - length;
            buffer[pos] = '\0';
            walle::BufferRef<const char> vector(buffer, pos);
            float strtof_result = str_to_float(vector, exponent);
            EXPECT_EQ(CheckFloat(vector, exponent, strtof_result), true);
        }
    }
}
