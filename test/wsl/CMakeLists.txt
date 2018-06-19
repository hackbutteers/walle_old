LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_string_view test_string_view.cc)
target_link_libraries(test_string_view gtest gtest_main walleStatic pthread)

add_executable(test_delegate test_delegate.cc)
target_link_libraries(test_delegate gtest gtest_main walleStatic pthread)

add_executable(test_stack_buffer test_stack_buffer.cc)
target_link_libraries(test_stack_buffer gtest gtest_main walleStatic pthread)

add_executable(test_sk test_sk.cc)
target_link_libraries(test_sk gtest gtest_main walleStatic pthread)