#!/bin/bash

set -euf -o pipefail

REPOSITORY_ROOT=${1:-${PWD}}
VCPKG_SUBMODULE_ROOT=${PWD}
BUILD_DIR=build

# Verify the repository root is a git repository.
if [[ ! -d ${REPOSITORY_ROOT}/.git ]]; then
  echo "Repository root is not a git repository: ${REPOSITORY_ROOT}"
  exit 1
fi

# Assign cmake presets.
# TODO: make these arguments, and use these as the default values?
PRESET_CONFIGURE=dev-linux
PRESET_BUILD=dev

# Add the workspace directory to the safe directory list.
git config --global --add safe.directory ${REPOSITORY_ROOT}

# Change to the root of the repo.
cd ${REPOSITORY_ROOT}
cmake -B ${BUILD_DIR} --preset ${PRESET_CONFIGURE} -DVCPKG_SUBMODULE_ROOT=${VCPKG_SUBMODULE_ROOT} .
cmake --build --preset ${PRESET_BUILD}
cmake --install ${BUILD_DIR}
