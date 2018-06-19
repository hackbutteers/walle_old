#include <google/gtest/gtest.h>
#include <walle/wsl/string_view.h>

TEST(string_view, empty)
{
    wsl::string_view sv;
    EXPECT_EQ(true, sv.empty());
    const char *abc = "asd";
    wsl::string_view s1(abc);
    wsl::cstring_view s2(abc);
    EXPECT_EQ(true, s1 == s2);
    EXPECT_EQ(false, s1 != s2);
}