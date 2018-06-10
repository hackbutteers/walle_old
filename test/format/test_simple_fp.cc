#include <walle/format/detail/simple_fp.h>
#include <google/gtest/gtest.h>
#include <walle/math/math.h>

WALLE_DISABLE_WARING(-Wsign-compare);
using namespace walle::format_detail;

TEST(SimpleFp, Subtract) {
  SimpleFp diy_fp1 = SimpleFp(3, 0);
  SimpleFp diy_fp2 = SimpleFp(1, 0);
  SimpleFp diff = SimpleFp::minus(diy_fp1, diy_fp2);

  EXPECT_EQ(2 , diff.f());  // NOLINT
  EXPECT_EQ(0, diff.e());
  diy_fp1.subtract(diy_fp2);
  EXPECT_EQ(2, diy_fp1.f());  // NOLINT
  EXPECT_EQ(0, diy_fp1.e());
}
WALLE_RESTORE_WARNING();