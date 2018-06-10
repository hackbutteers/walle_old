#include <walle/config/base.h>
#include <google/gtest/gtest.h>
#include <walle/format/date_time_formatter.h>
#include <cstdlib>
#include <iostream>
#include <walle/format/format.h>
#include <walle/math/math.h>

TEST(DateTime, http_format)
{
    walle::DateTime dt;
    walle::MemoryWriter out;
    walle::DateTimeFormatter::kHttpFormatter.format(out, dt);
    std::cout<<"kHttpFormatter: "<<out.str()<<std::endl;

    walle::MemoryWriter out1;
    walle::DateTimeFormatter::kIso8601Formatter.format(out1, dt);
    std::cout<<"kIso8601Formatter: "<<out1.str()<<std::endl;

    walle::MemoryWriter out2;
    walle::DateTimeFormatter::kIso8601FracFormatter.format(out2, dt);
    std::cout<<"kIso8601FracFormatter: "<<out2.str()<<std::endl;

    walle::MemoryWriter out3;
    walle::DateTimeFormatter::kRfc822Formatter.format(out3, dt);
    std::cout<<"kRfc822Formatter: "<<out3.str()<<std::endl;

    walle::MemoryWriter out4;
    walle::DateTimeFormatter::kRfc1123Formatter.format(out4, dt);
    std::cout<<"kRfc1123Formatter: "<<out4.str()<<std::endl;

    walle::MemoryWriter out5;
    walle::DateTimeFormatter::kRfc850Formatter.format(out5, dt);
    std::cout<<"kRfc850Formatter: "<<out5.str()<<std::endl;

    walle::MemoryWriter out6;
    walle::DateTimeFormatter::kRfc1036Formatter.format(out6, dt);
    std::cout<<"kRfc1036Formatter: "<<out6.str()<<std::endl;

    walle::MemoryWriter out7;
    walle::DateTimeFormatter::kAscTimeFormatter.format(out7, dt);
    std::cout<<"kAscTimeFormatter: "<<out7.str()<<std::endl;

    walle::MemoryWriter out8;
    walle::DateTimeFormatter::kSortAbleFormatter.format(out8, dt);
    std::cout<<"kSortAbleFormatter: "<<out8.str()<<std::endl;
}