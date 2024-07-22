#!/bin/bash
# shellcheck disable=SC2034
#
# Update the version number references in the project. This currently updates the version number in the CMake version
# script file and vcpkg.json files.

set -eu -o pipefail

SCRIPT_DIR="$(dirname "${0}")"
PROJECT_ROOT="$(dirname "$(dirname "$(readlink -fm "${0}")")")"

# Set constants.
readonly SCRIPT_DIR
readonly PROJECT_ROOT
readonly CMAKE_SCRIPT_VERSION_FILE="${PROJECT_ROOT}/cmake/version.cmake"
readonly VCPKG_JSON_FILES=( "${PROJECT_ROOT}/vcpkg.json" "${PROJECT_ROOT}/.docker/netremote-dev/vcpkg.json" )
readonly VERSION_TYPES=(MAJOR MINOR PATCH)
readonly VERSION="${1:-}"

# Verify the version is provided.
if [[ -z "${VERSION}" ]]; then
  echo "Usage: $0 <version major>.<version minor>.<version patch>"
  exit 1
fi

# Parse and verify the version is a valid semantic version with major, minor, and patch components.
if ! [[ "${VERSION}" =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)$ ]]; then
  echo "Invalid version: ${VERSION}"
  echo "Usage: ${0} <version major>.<version minor>.<version patch>"
  exit 1
fi

# Parse the specified version into major, minor, and patch components.
readonly VERSION_MAJOR="${BASH_REMATCH[1]}"
readonly VERSION_MINOR="${BASH_REMATCH[2]}"
readonly VERSION_PATCH="${BASH_REMATCH[3]}"

# Update the lines with 'set(VERSION_<MAJOR|MINOR|PATCH> <value>)' in the version.cmake file.
printf 'Updating CMake version script file %s ... ' "${CMAKE_SCRIPT_VERSION_FILE}"
for version_type in "${VERSION_TYPES[@]}"; do
    VERSION_VALUE="VERSION_${version_type}"
    sed -i "s/set(VERSION_${version_type} [0-9]\+)/set(VERSION_${version_type} ${!VERSION_VALUE})/" "${CMAKE_SCRIPT_VERSION_FILE}"
done
printf '\xE2\x9C\x94\n'

# Update the version in the vcpkg.json files, with format "version-string": "<version>".
echo -n "Updating version is vcpkg manifest files ... "
for vcpkg_json_file in "${VCPKG_JSON_FILES[@]}"; do
  sed -i "s/\"version-string\": \"[0-9]\+\.[0-9]\+\.[0-9]\+\"/\"version-string\": \"${VERSION}\"/" "${vcpkg_json_file}"
done
printf '\xE2\x9C\x94\n'

echo "NetRemote version updated to v${VERSION}"
