cmake_minimum_required(VERSION 2.8)
project(wtoeutil)

include_directories(include/)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src DIR_SRCS)
add_library(wtoeutil SHARED ${DIR_SRCS})
target_link_libraries(wtoeutil)

aux_source_directory(test DIR_TESTS)
add_executable(wtoeutil_test ${DIR_TESTS})
target_link_libraries(wtoeutil_test wtoeutil gtest pthread)
