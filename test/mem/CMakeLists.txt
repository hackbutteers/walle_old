LINK_DIRECTORIES("/usr/local/lib")
add_executable(test_allocators test_allocators.cc)
target_link_libraries(test_allocators gtest gtest_main walleStatic pthread)

add_executable(test_pool test_pool.cc)
target_link_libraries(test_pool gtest gtest_main walleStatic pthread)