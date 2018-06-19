#include <google/gtest/gtest.h>
#include <walle/wsl/stack_buffer.h>

TEST(stack_buffer, empty)
{
    wsl::stack_buffer<char, 4096> sv;
    EXPECT_EQ(0, sv.size());
}