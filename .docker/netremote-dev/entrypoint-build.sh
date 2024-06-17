#!/bin/bash

set -euf -o pipefail

readonly REPOSITORY_ROOT="${1:-${PWD}}"
readonly CMAKE_PRESET_CONFIGURE="${2}"
readonly CMAKE_PRESET_BUILD="${3}"
readonly CMAKE_PRESET_PACKAGE="${4}"
readonly CMAKE_BUILD_CONFIG="${5}"

echo "------------------------------------"
echo "Build entrypoint script parameters:"
echo "------------------------------------"
echo "Source Repository Root=${REPOSITORY_ROOT}"
echo "CMake"
echo " Build Configuration=${CMAKE_BUILD_CONFIG}"
echo " Presets"
echo "  Configure='${CMAKE_PRESET_CONFIGURE}'"
echo "  Build='${CMAKE_PRESET_BUILD}'"
echo "  Package='${CMAKE_PRESET_PACKAGE}'"

# Verify the repository root is a git repository.
if [[ ! -d ${REPOSITORY_ROOT}/.git ]]; then
  echo "Repository root is not a git repository: ${REPOSITORY_ROOT}"
  exit 1
fi

# Verify the vcpkg toolchain file is present.
if [[ ! -f ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake ]]; then
  echo "vcpkg cmake toolchain file not found: ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
  exit 1
fi

# Add the workspace directory to the safe directory list.
git config --global --add safe.directory "${REPOSITORY_ROOT}"

# Change to the root of the repo.
cd "${REPOSITORY_ROOT}"
cmake --preset "${CMAKE_PRESET_CONFIGURE}"
cmake --build --preset "${CMAKE_PRESET_BUILD}"
cmake --install ../netremote-cmake/build/"${CMAKE_PRESET_CONFIGURE}" --config "${CMAKE_BUILD_CONFIG}"
