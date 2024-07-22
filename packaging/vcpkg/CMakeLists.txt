
# Define input and output file paths for the vcpkg portfile.
set(VCPKG_PORT_FILE_OUT ${CMAKE_CURRENT_LIST_DIR}/ports/netremote/portfile.cmake)
set(VCPKG_PORT_FILE_IN ${VCPKG_PORT_FILE_OUT}.in)

# Configure variables to be substituted in the vcpkg portfile.
set(GIT_REF_HEAD master)
set(GIT_REF v${CMAKE_PROJECT_VERSION})
set(GIT_REF_URL ${CMAKE_PROJECT_HOMEPAGE_URL}/archive/refs/tags/${GIT_REF}.tar.gz)

# Calculate sha512 hash of the git ref tarball, required for vcpkg portfile.
message(STATUS "Downloading and hashing ${GIT_REF_URL}")
if (BUILD_FOR_LINUX)
    execute_process(
        COMMAND wget -qO- ${GIT_REF_URL}
        COMMAND sha512sum -z
        COMMAND bash -c "awk '{ print $1 }'"
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_VARIABLE GIT_REF_SHA512
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
elseif (BUILD_FOR_WINDOWS)
    execute_process(
        COMMAND powershell ${CMAKE_CURRENT_LIST_DIR}/GetFileHashSha512.ps1 ${GIT_REF_URL}
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_VARIABLE GIT_REF_SHA512
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

message(STATUS "${GIT_REF_URL} sha512 hash='${GIT_REF_SHA512}'")

# Generate portfile.cmake with selected versioning and configuration information.
configure_file(${VCPKG_PORT_FILE_IN} ${VCPKG_PORT_FILE_OUT} @ONLY)
