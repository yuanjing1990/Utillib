cmake_minimum_required(VERSION 3.4)
project(wtoeutil)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src DIR_SRCS)
add_library(wtoeutil SHARED ${DIR_SRCS})
target_include_directories(wtoeutil PUBLIC include/)
target_link_libraries(wtoeutil)

aux_source_directory(test DIR_TESTS)
add_executable(wtoeutil_test ${DIR_TESTS})
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(wtoeutil_test wtoeutil GTest::GTest Threads::Threads)
