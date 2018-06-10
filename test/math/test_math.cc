
#include <walle/math/math.h>
#include <google/gtest/gtest.h>
#include <cstdlib>

TEST(math, ffs)
{
    EXPECT_EQ(walle::math::ffs(0), 0u);
    EXPECT_EQ(walle::math::ffs_template(0), 0u);

    unsigned power = 0;
    for (uint64_t i = 1; i < (1llu << 63); i <<= 1, ++power)
    {
        if (i > 1) {
            EXPECT_EQ(walle::math::ffs(i - 1), 1u);
            EXPECT_EQ(walle::math::ffs_template(i - 1), 1u);
        }

        EXPECT_EQ(walle::math::ffs(i), power + 1);
        EXPECT_EQ(walle::math::ffs_template(i), power + 1);

        if (i > 1) {
            EXPECT_EQ(walle::math::ffs(i + 1), 1u);
            EXPECT_EQ(walle::math::ffs_template(i + 1), 1u);
        }
    }
}