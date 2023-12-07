#!/bin/bash

set -euf -o pipefail

REPOSITORY_ROOT=${1:-${PWD}}
BUILD_DIR=build

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

# Assign cmake presets.
# TODO: make these arguments, and use these as the default values?
PRESET_CONFIGURE=dev-linux
PRESET_BUILD=${PRESET_CONFIGURE}

# Add the workspace directory to the safe directory list.
git config --global --add safe.directory ${REPOSITORY_ROOT}

# Change to the root of the repo.
cd ${REPOSITORY_ROOT}
mv ${VCPKG_ROOT} .
export VCPKG_ROOT=${REPOSITORY_ROOT}/vcpkg
cmake -B ${BUILD_DIR} --preset ${PRESET_CONFIGURE}
cmake --build --preset ${PRESET_BUILD}
cmake --install ${BUILD_DIR}
