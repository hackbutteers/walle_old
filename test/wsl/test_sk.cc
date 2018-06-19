#include <google/gtest/gtest.h>
#include <walle/wsl/skip_list.h>

TEST(skip_list, insert)
{
    wsl::skip_list<int> sl;
    sl.insert(2);
    sl.insert(4);
    sl.insert(1);
    EXPECT_EQ(3, sl.size());
}

TEST(multi_skip_list, insert)
{
    wsl::multi_skip_list<int> sl;
    sl.insert(2);
    sl.insert(4);
    sl.insert(1);
    sl.insert(1);
    EXPECT_EQ(4, sl.size());
}