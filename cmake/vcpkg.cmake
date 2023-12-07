
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

  # Enable debug output from the vcpkg install command.
  set(VCPKG_INSTALL_OPTIONS "--debug") 

  # If the vcpkg root has been specified externally, use it.
  if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
  # Otherwise, use the specified submodule root.
  else()
    set(VCPKG_ROOT ${VCPKG_SUBMODULE_ROOT})

    find_package(Git REQUIRED)

    # Initialize vcpkg sub-module if not already done.
    if (NOT EXISTS ${VCPKG_SUBMODULE_ROOT}/.git)
      execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${VCPKG_SUBMODULE_ROOT}
        WORKING_DIRECTORY ${VCPKG_SUBMODULE_ROOT}/../
        COMMAND_ERROR_IS_FATAL ANY)
    endif()

    # Ignore all changes to the submodule tree.
    get_filename_component(VCPKG_SUBMODULE_NAME ${VCPKG_SUBMODULE_ROOT} NAME)
    execute_process(COMMAND ${GIT_EXECUTABLE} "config submodule.${VCPKG_SUBMODULE_NAME}.ignore all"
      WORKING_DIRECTORY ${VCPKG_SUBMODULE_ROOT}../
    )
  endif()

  # Set the CMake toolchain file to use vcpkg.
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "CMake toolchain file")

endfunction()
