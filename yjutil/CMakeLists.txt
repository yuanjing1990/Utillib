cmake_minimum_required(VERSION 3.4)

project(yjtuil)
set(CMAKE_CXX_STANDARD 17)
include_directories(include/)
add_compile_options(-D_DEBUG)

aux_source_directory(src SRC_DIRS)
add_library(yjutil SHARED ${SRC_DIRS})
link_libraries(yjutil crypto)

aux_source_directory(test TEST_DIRS)
add_executable(yjutil_test ${TEST_DIRS})
find_package(GTest REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(yjutil_test yjutil GTest::GTest ZLIB::ZLIB OpenSSL::Crypto)
