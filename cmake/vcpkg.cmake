
include(CMakeParseArguments)

# Configures vcpkg for use locally.
# 
# This will determine if vcpkg is available. If not, it will clone the vcpkg
# Github repository as a submodule into the directory specified by
# SUBMODULE_ROOT, and instruct cmake to use its toolchain file to help satisfy
# dependencies. It also adds a local git configuration rule to ignore all
# changes in the submodule.
function(vcpkg_configure)
  cmake_parse_arguments(
    VCPKG 
    "" 
    "SUBMODULE_ROOT"
    ""
    ${ARGN}
  )

  get_filename_component(VCPKG_SUBMODULE_NAME ${VCPKG_SUBMODULE_ROOT} NAME)

  if ("${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}" STREQUAL "")
    find_package(Git REQUIRED)

    # Initialize vcpkg sub-module if not already done.
    if (NOT EXISTS ${VCPKG_SUBMODULE_ROOT}/.git)
      execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${VCPKG_SUBMODULE_ROOT}
        WORKING_DIRECTORY ${VCPKG_SUBMODULE_ROOT}/../
        COMMAND_ERROR_IS_FATAL ANY)
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_SUBMODULE_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
  endif()

  # Ignore all changes to the submodule tree.
  execute_process(COMMAND ${GIT_EXECUTABLE} "config submodule.${VCPKG_SUBMODULE_NAME}.ignore all"
    WORKING_DIRECTORY ${VCPKG_SUBMODULE_ROOT}../
  )

endfunction()
