#!/usr/bin/env bash

set -euf -o pipefail

REPOSITORY_ROOT=$1

# Ensure the repository root exists.
if [[ ! -d ${REPOSITORY_ROOT} ]]; then
  echo "Repository root does not exist: ${REPOSITORY_ROOT}"
  exit 1
fi

# Change to the root of the repo.
cd ${REPOSITORY_ROOT}
cmake -B build . --preset dev
