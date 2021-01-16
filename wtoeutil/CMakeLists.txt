cmake_minimum_required(VERSION 2.8)
project(wtoeutil)

include_directories(include/)

aux_source_directory(src DIR_SRCS)
add_library(wtoeutil SHARED ${DIR_SRCS})
target_link_libraries(wtoeutil boost_system boost_thread boost_filesystem)

aux_source_directory(test DIR_TESTS)
add_executable(wtoeutil_test ${DIR_TESTS})
target_link_libraries(wtoeutil_test wtoeutil)
