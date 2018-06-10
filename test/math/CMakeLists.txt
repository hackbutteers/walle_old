LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_math test_math.cc)
target_link_libraries(test_math gtest gtest_main walleStatic pthread)