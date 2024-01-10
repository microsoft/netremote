
cmake_minimum_required(VERSION 3.25)

# Instruct CMake to search the cmake directory for include scripts.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(vcpkg)
option(NETREMOTE_VCPKG_BUILD_FOR_PORT "Enable building the project via a vcpkg port" OFF)
option(NETREMOTE_EXCLUDE_SERVER "Disable building the server" OFF)
option(NETREMOTE_EXCLUDE_TOOLS "Disable building the tools" OFF)
option(NETREMOTE_EXCLUDE_TESTS "Disable building the tests" OFF)

# vcpkg build target.
# 
# This controls the versioning information used when the vcpkg is built. 
# When "development" is selected, the HEAD revision of the current branch will be used.  
# When "release" is selected, the latest hard-coded version in packaging/vcpkg/CMakeLists.txt.
#
set(VCPKG_TARGET_DEVELOPMENT "development")
set(VCPKG_TARGET_RELEASE "release")
set(VCPKG_TARGET ${VCPKG_TARGET_DEVELOPMENT} CACHE STRING "vcpkg target: either 'development' or 'release'")

# Tell vcpkg to use the submodule root directory as the vcpkg root, and then configure it.
set(VCPKG_SUBMODULE_ROOT ${CMAKE_CURRENT_LIST_DIR}/vcpkg CACHE PATH "Location of vcpkg submodule root")
vcpkg_configure(SUBMODULE_ROOT ${VCPKG_SUBMODULE_ROOT})

project(netremote
  LANGUAGES CXX
  VERSION 0.0.1
)

# Conditional inclusion of OS-dependent source trees.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(BUILD_FOR_LINUX TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(BUILD_FOR_WINDOWS TRUE)
else ()
  MESSAGE(FATAL_ERROR "No supported target OS detected, SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
endif()

MESSAGE(STATUS "Target OS ${CMAKE_SYSTEM_NAME} detected")

# Set language configutation options. The C++ standard used must be the lowest
# common denominator for all the OS-dependent projects. In practice, since this
# project must build under the Windows build system (build.exe), its toolchain
# is typically the limiting factor.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Don't add CTest targets
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

include(CheckPIESupported)
include(CTest)
include(ExternalProject)
include(GNUInstallDirs)
include(grpc)
include(protoc)

# Enable to debug CMake issues.
set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "Verbose Makefile Generation")

# Pull in external dependencies.
# Look for protobuf-config.cmake.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)
find_package(Git REQUIRED)

# Enable POSITION_INDEPENDENT_CODE variable to control passing PIE flags to the linker.
if (POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
endif()

# Enable position independent executables.
check_pie_supported(LANGUAGES CXX)
if (CMAKE_CXX_LINK_PIE_SUPPORTED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Set compiler specific options
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Common options for both gcc and clang
  add_compile_options(
    -fstack-protector-all
    -fvisibility=hidden
    -fcf-protection
    -fpermissive
    -mshstk
    -Wall 
    -Wshadow
    -Wformat-security
    -Wextra
    -Wpedantic
    -Wconversion
    -Walloca
    -Wvla
  )

  # gcc specific options
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
      -fstack-clash-protection
      -Wtrampolines
      -Wl,-z,noexecstack
      -Wl,-z,now
      -Wl,-z,relro
      -z noexecstack
    )
    if (NETREMOTE_CODE_COVERAGE)
      add_compile_options(
        -fprofile-arcs
        -ftest-coverage
      )
    endif()
  # clang specific options
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (NETREMOTE_CODE_COVERAGE)
      add_compile_options(
        -fprofile-instr-generate
        -fcoverage-mapping
      )
    endif()
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Nothing special needed for MSVC at the moment.
endif()

# Add conditional compile definitions based on build type.
add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")
add_compile_definitions("$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:_FORTIFY_SOURCE=2>")

# Common source directories
set(NETREMOTE_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(NETREMOTE_DIR_LINUX ${NETREMOTE_DIR_SRC}/linux)
set(NETREMOTE_DIR_WINDOWS ${NETREMOTE_DIR_SRC}/windows)
set(NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Add common, global compile definitions.
add_compile_definitions(
    MAGIC_ENUM_RANGE_MIN=0
    MAGIC_ENUM_RANGE_MAX=255
)

if (WERROR)
  add_compile_options(-Werror)
endif()

add_subdirectory(packaging)
add_subdirectory(protocol)
add_subdirectory(src)

if (NOT NETREMOTE_EXCLUDE_TESTS)
  add_subdirectory(tests)
endif()
