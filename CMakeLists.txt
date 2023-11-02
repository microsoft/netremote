
cmake_minimum_required(VERSION 3.25)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(vcpkg)

set(VCPKG_SUBMODULE_ROOT ${CMAKE_CURRENT_LIST_DIR}/vcpkg)

vcpkg_configure(SUBMODULE_ROOT ${VCPKG_SUBMODULE_ROOT})

project(wifiremote
  LANGUAGES CXX
  VERSION 0.0.1
)

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
include(GNUInstallDirs)
include(grpc)
include(protoc)

# Enable verbose output until project is bootstrapped.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Pull in external dependencies.
# Look for protobuf-config.cmake.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Enable POSITION_INDEPENDENT_CODE variable to control passing PIE flags to the linker.
if (POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
endif()

# Enable position independent executables.
check_pie_supported(LANGUAGES CXX)
if (CMAKE_CXX_LINK_PIE_SUPPORTED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (WERROR)
  add_compile_options(-Werror)
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
    if (NOF_CODE_COVERAGE)
      add_compile_options(
        -fprofile-arcs
        -ftest-coverage
      )
    endif()
  # clang specific options
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (NOF_CODE_COVERAGE)
      add_compile_options(
        -fprofile-instr-generate
        -fcoverage-mapping
      )
    endif()
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # TODO
endif()

if (CMAKE_BUILD_TYPE MATCHES "(Release|RelWithDebInfo|MinSizeRel)")
  add_compile_definitions(_FORTIFY_SOURCE=2)
endif()

add_subdirectory(protocol)
add_subdirectory(src)
