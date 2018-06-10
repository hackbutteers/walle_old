LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_simple_fp test_simple_fp.cc)
target_link_libraries(test_simple_fp gtest gtest_main walleStatic pthread)

add_executable(test_ieee test_ieee.cc)
target_link_libraries(test_ieee gtest gtest_main walleStatic pthread)

add_executable(test_bignum test_bignum.cc)
target_link_libraries(test_bignum gtest gtest_main walleStatic pthread)

add_executable(test_bignum_dtoa test_bignum_dtoa.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_bignum_dtoa gtest gtest_main walleStatic pthread)

add_executable(test_dtoa test_dtoa.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_dtoa gtest gtest_main walleStatic pthread)

add_executable(test_fixed_dtoa test_fixed_dtoa.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_fixed_dtoa gtest gtest_main walleStatic pthread)

add_executable(test_fast_dtoa test_fast_dtoa.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_fast_dtoa gtest gtest_main walleStatic pthread)

add_executable(test_strtod test_strtod.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_strtod gtest gtest_main walleStatic pthread)
add_executable(test_conversion test_conversion.cc gay_fixed.cc gay_shortest_single.cc gay_precision.cc gay_shortest.cc)
target_link_libraries(test_conversion gtest gtest_main walleStatic pthread)

add_executable(test_date_time test_date_time.cc)
target_link_libraries(test_date_time gtest gtest_main walleStatic pthread)