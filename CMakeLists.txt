
cmake_minimum_required(VERSION 3.25)

# Enable to debug CMake issues.
set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "Verbose build system generation")

# Instruct CMake to search the cmake directory for include scripts.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Find and make git available now as many other scripts depend on it.
find_package(Git)

# Configure project version.
include(version)

# Configure vcpkg usage within the project.
include(vcpkg)
option(NETREMOTE_VCPKG_BUILD_FOR_PORT "Enable building the project via a vcpkg port" OFF)

# Add options for vcpkg port file features.
option(NETREMOTE_EXCLUDE_API "Disable providing the protocol buffers + gRPC API source files" OFF)
option(NETREMOTE_EXCLUDE_API_BINDINGS "Disable building the API protocol buffers + gRPC bindings" OFF)
option(NETREMOTE_EXCLUDE_SERVER "Disable building the server" OFF)
option(NETREMOTE_EXCLUDE_TOOLS "Disable building the tools" OFF)
option(NETREMOTE_EXCLUDE_TESTS "Disable building the tests" OFF)

# Tell vcpkg to use the submodule root directory as the vcpkg root, and then configure it.
set(VCPKG_SUBMODULE_ROOT ${CMAKE_CURRENT_LIST_DIR}/vcpkg CACHE PATH "Location of vcpkg submodule root")
vcpkg_configure(SUBMODULE_ROOT ${VCPKG_SUBMODULE_ROOT})

project(netremote
  LANGUAGES CXX
  VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
  DESCRIPTION "A framework for remotely controlling network components such as wi-fi access points."
  HOMEPAGE_URL https://github.com/microsoft/netremote
)

message(STATUS "Configuring netremote version v${CMAKE_PROJECT_VERSION}")

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
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckPIESupported)
include(ExternalProject)
include(GNUInstallDirs)

# Configure dependencies.
include(config-dependencies)

# Configure toolchain.
include(config-toolchain)

# Common source directories
set(NETREMOTE_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(NETREMOTE_DIR_LINUX ${NETREMOTE_DIR_SRC}/linux)
set(NETREMOTE_DIR_WINDOWS ${NETREMOTE_DIR_SRC}/windows)
set(NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

if (WERROR)
  add_compile_options(-Werror)
endif()

add_subdirectory(config)
add_subdirectory(packaging)
add_subdirectory(src)

if (NOT NETREMOTE_EXCLUDE_API)
  add_subdirectory(api)
endif()

if (NOT NETREMOTE_EXCLUDE_TESTS)
  add_subdirectory(tests)
endif()
