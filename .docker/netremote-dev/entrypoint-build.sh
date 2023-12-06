#!/usr/bin/env bash

set -euf -o pipefail

REPOSITORY_ROOT=$1
BUILD_DIR=build

# Assign cmake presets.
# TODO: make these arguments, and use these as the default values?
PRESET_CONFIGURE=dev-linux
PRESET_BUILD=${PRESET_CONFIGURE}
PRESET_INSTALL=${PRESET_CONFIGURE}

# Ensure the repository root exists.
if [[ ! -d ${REPOSITORY_ROOT} ]]; then
  echo "Repository root does not exist: ${REPOSITORY_ROOT}"
  exit 1
fi

# Change to the root of the repo.
cd ${REPOSITORY_ROOT}
cmake -B ${BUILD_DIR} --preset ${PRESET_CONFIGURE} .
cmake --build --preset ${PRESET_BUILD}
cmake --install ${BUILD_DIR}
