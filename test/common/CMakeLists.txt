LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_delegate test_delegate.cc)
target_link_libraries(test_delegate gtest gtest_main walleStatic pthread)