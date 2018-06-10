LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_console_channel test_console_channel.cc)
target_link_libraries(test_console_channel gtest gtest_main walleStatic pthread)

add_executable(test_inner_log test_inner_log.cc)
target_link_libraries(test_inner_log gtest gtest_main walleStatic pthread)