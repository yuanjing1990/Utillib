set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(LINUX_COMPILER_FLAGS)
set(LINUX_COMPILER_FLAGS_CXX)
set(LINUX_COMPILER_FLAGS_DEBUG)
set(LINUX_COMPILER_FLAGS_RELEASE)
set(LINUX_LINKER_FLAGS)
set(LINUX_LINKER_FLAGS_EXE)

# STL.
set(LINUX_TOOLCHAIN_PATH /opt/arm-bcm2708/arm-linux-gnueabihf)
list(APPEND CMAKE_FIND_ROOT_PATH "${LINUX_TOOLCHAIN_PATH}")
list(APPEND CMAKE_FIND_ROOT_PATH "/opt/arm_lib_grpc")

# Sysroot.
set(CMAKE_SYSROOT ${LINUX_TOOLCHAIN_PATH}/arm-linux-gnueabihf/sysroot)

# Toolchain.
set(LINUX_C_COMPILER   "/usr/bin/arm-linux-gnueabihf-gcc-9")
set(LINUX_CXX_COMPILER "/usr/bin/arm-linux-gnueabihf-g++-9")
set(LINUX_ASM_COMPILER "/usr/bin/arm-linux-gnueabihf-as")

set(CMAKE_C_COMPILER_ID_RUN TRUE)
set(CMAKE_CXX_COMPILER_ID_RUN TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)
set(CMAKE_C_COMPILER_VERSION 9.4)
set(CMAKE_CXX_COMPILER_VERSION 9.4)
set(CMAKE_C_STANDARD_COMPUTED_DEFAULT 11)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT 98)
set(CMAKE_CXX_STANDARD 11)
# Generic flags.
list(APPEND LINUX_COMPILER_FLAGS
-DLINUX
-ffunction-sections
-funwind-tables
-fstack-protector-strong
-no-canonical-prefixes)
list(APPEND LINUX_LINKER_FLAGS
-Wl,--build-id
-Wl,--warn-shared-textrel
-Wl,--fatal-warnings)
list(APPEND LINUX_LINKER_FLAGS_EXE
-Wl,--gc-sections
-Wl,-z,nocopyreloc)

# Debug and release flags.
list(APPEND LINUX_COMPILER_FLAGS_DEBUG
-g
-O0)

list(APPEND LINUX_COMPILER_FLAGS_RELEASE
-O2)

list(APPEND LINUX_COMPILER_FLAGS_RELEASE
-DNDEBUG)

# Toolchain and ABI specific flags.
# Configuration specific flags.
# if(LINUX_PIE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
list(APPEND LINUX_LINKER_FLAGS_EXE
-pie
-fPIE)
# endif()

list(APPEND LINUX_LINKER_FLAGS
-Wl,--no-undefined)

# Convert these lists into strings.
string(REPLACE ";" " " LINUX_COMPILER_FLAGS         "${LINUX_COMPILER_FLAGS}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_CXX     "${LINUX_COMPILER_FLAGS_CXX}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_DEBUG   "${LINUX_COMPILER_FLAGS_DEBUG}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_RELEASE "${LINUX_COMPILER_FLAGS_RELEASE}")
string(REPLACE ";" " " LINUX_LINKER_FLAGS           "${LINUX_LINKER_FLAGS}")
string(REPLACE ";" " " LINUX_LINKER_FLAGS_EXE       "${LINUX_LINKER_FLAGS_EXE}")

set(CMAKE_C_COMPILER        "${LINUX_C_COMPILER}")
set(CMAKE_CXX_COMPILER      "${LINUX_CXX_COMPILER}")

# Set or retrieve the cached flags.
# This is necessary in case the user sets/changes flags in subsequent
# configures. If we included the Android flags in here, they would get
# overwritten.
set(CMAKE_C_FLAGS ""
CACHE STRING "Flags used by the compiler during all build types.")
set(CMAKE_CXX_FLAGS ""
CACHE STRING "Flags used by the compiler during all build types.")
set(CMAKE_ASM_FLAGS ""
CACHE STRING "Flags used by the compiler during all build types.")
set(CMAKE_C_FLAGS_DEBUG ""
CACHE STRING "Flags used by the compiler during debug builds.")
set(CMAKE_CXX_FLAGS_DEBUG ""
CACHE STRING "Flags used by the compiler during debug builds.")
set(CMAKE_ASM_FLAGS_DEBUG ""
CACHE STRING "Flags used by the compiler during debug builds.")
set(CMAKE_C_FLAGS_RELEASE ""
CACHE STRING "Flags used by the compiler during release builds.")
set(CMAKE_CXX_FLAGS_RELEASE ""
CACHE STRING "Flags used by the compiler during release builds.")
set(CMAKE_ASM_FLAGS_RELEASE ""
CACHE STRING "Flags used by the compiler during release builds.")
set(CMAKE_MODULE_LINKER_FLAGS ""
CACHE STRING "Flags used by the linker during the creation of modules.")
set(CMAKE_SHARED_LINKER_FLAGS ""
CACHE STRING "Flags used by the linker during the creation of dll's.")
set(CMAKE_EXE_LINKER_FLAGS ""
CACHE STRING "Flags used by the linker.")

set(CMAKE_C_FLAGS             "${LINUX_COMPILER_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS           "${LINUX_COMPILER_FLAGS} ${LINUX_COMPILER_FLAGS_CXX} ${CMAKE_CXX_FLAGS}")
set(CMAKE_ASM_FLAGS           "${LINUX_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG       "${LINUX_COMPILER_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG     "${LINUX_COMPILER_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_DEBUG     "${LINUX_COMPILER_FLAGS_DEBUG} ${CMAKE_ASM_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE     "${LINUX_COMPILER_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE   "${LINUX_COMPILER_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_ASM_FLAGS_RELEASE   "${LINUX_COMPILER_FLAGS_RELEASE} ${CMAKE_ASM_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${LINUX_LINKER_FLAGS} ${LINUX_LINKER_FLAGS_EXE} ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"./$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"./$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")

set(BUILD_SHARED_LIBS OFF)