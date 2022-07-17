cmake_minimum_required(VERSION 3.5.1)

project(HelloWorld C CXX)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/arm_linux_gnueabi.cmake)
find_package(Threads REQUIRED)

if(GRPC_FETCHCONTENT)
  # Another way is to use CMake's FetchContent module to clone gRPC at
  # configure time. This makes gRPC's source code available to your project,
  # similar to a git submodule.
  message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
  include(FetchContent)
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    # when using gRPC, you will actually set this to an existing tag, such as
    # v1.25.0, v1.26.0 etc..
    # For the purpose of testing, we override the tag used to the commit
    # that's currently under test.
    GIT_TAG        vGRPC_TAG_VERSION_OF_YOUR_CHOICE)
  FetchContent_MakeAvailable(grpc)

  # Since FetchContent uses add_subdirectory under the hood, we can use
  # the grpc targets directly from this build.
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
  set(_GRPC_GRPCPP grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
  endif()
else()
  # This branch assumes that gRPC and all its dependencies are already installed
  # on this system, so they can be located by find_package().

  # Find Protobuf installation
  # Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
  set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${Protobuf_VERSION}")

  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
  set(_REFLECTION gRPC::grpc++_reflection)
  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()
  message(STATUS ${_PROTOBUF_PROTOC})

  # Find gRPC installation
  # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()
  message(STATUS ${_GRPC_CPP_PLUGIN_EXECUTABLE})
endif()

# Proto file
get_filename_component(hw_proto_path "proto" ABSOLUTE)
file(GLOB hw_proto_files "${hw_proto_path}/*.proto")

foreach(_hw_proto_file ${hw_proto_files})
    get_filename_component(_proto_file_name ${_hw_proto_file} NAME_WE)
    set(_hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file_name}.pb.cc")
    set(_hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file_name}.pb.h")
    set(_hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file_name}.grpc.pb.cc")
    set(_hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${__proto_file_name}.grpc.pb.h")
    message(STATUS ${_hw_proto_srcs} ${_hw_proto_file})
    add_custom_command(
        OUTPUT "${_hw_proto_srcs}" "${_hw_proto_hdrs}" "${_hw_grpc_srcs}" "${_hw_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${hw_proto_path}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${_hw_proto_file}"
        DEPENDS "${_hw_proto_file}")
    add_custom_target(${_proto_file_name} ALL DEPENDS "${_hw_proto_srcs}" "${_hw_proto_hdrs}" "${_hw_grpc_srcs}" "${_hw_grpc_hdrs}")
endforeach()

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")