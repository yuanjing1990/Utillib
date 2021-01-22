cmake_minimum_required(VERSION 2.8)

project(yjtuil)

include_directories(include/)
add_compile_options(-D_DEBUG)

aux_source_directory(src SRC_DIRS)
add_library(yjutil SHARED ${SRC_DIRS})
link_libraries(yjutil crypto)

aux_source_directory(test TEST_DIRS)
add_executable(yjutil_test ${TEST_DIRS})
link_libraries(yjutil_test yjutil)
target_link_libraries(yjutil_test gtest z pthread boost_system boost_filesystem)