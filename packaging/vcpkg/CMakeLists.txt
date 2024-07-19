
# Define input and output file paths for the vcpkg portfile.
set(VCPKG_PORT_FILE_OUT ${CMAKE_CURRENT_LIST_DIR}/ports/netremote/portfile.cmake)
set(VCPKG_PORT_FILE_IN ${VCPKG_PORT_FILE_OUT}.in)

# Obtain current git branch.
execute_process(COMMAND ${GIT_EXECUTABLE} branch --show
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_BRANCH_CURRENT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Obtain git HEAD commit sha from this branch.
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_REF_HEAD
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Obtain git origin url.
execute_process(COMMAND ${GIT_EXECUTABLE} remote get-url origin
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_URL_ORIGIN
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (GIT_REF_URL ${CMAKE_PROJECT_HOMEPAGE_URL}/archive/refs/tags/v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.tar.gz)

if (NOT EXISTS ${VCPKG_PORT_FILE_OUT})
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
            COMMAND powershell .\GetFileHashSha512.ps1 "${GIT_REF_URL}"
            OUTPUT_VARIABLE GIT_REF_SHA512
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
    message(STATUS "${GIT_REF_URL} sha512 hash='${GIT_REF_SHA512}'")
endif()

# Current development versioning information.
set(VCPKG_DEVELOPMENT_URL ${CMAKE_SOURCE_DIR})
set(VCPKG_DEVELOPMENT_GIT_REF ${GIT_REF_HEAD})
set(VCPKG_DEVELOPMENT_GIT_REF_FETCH ${GIT_BRANCH_CURRENT})

# Current release versioning information.
set(VCPKG_RELEASE_TAG v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(VCPKG_RELEASE_URL ${GIT_URL_ORIGIN})
set(VCPKG_RELEASE_GIT_REF ${VCPKG_RELEASE_TAG})
set(VCPKG_RELEASE_GIT_REF_FETCH ${GIT_BRANCH_CURRENT})

# Determine the vcpkg target the codebase is being built for.
if (VCPKG_TARGET STREQUAL VCPKG_TARGET_DEVELOPMENT)
    set(VCPKG_TARGET_SELECTOR DEVELOPMENT)
else()
    set(VCPKG_TARGET_SELECTOR RELEASE)
endif()

# Select configuration based on build target.
set(VCPKG_GIT_URL ${VCPKG_${VCPKG_TARGET_SELECTOR}_URL})
set(VCPKG_GIT_REF ${VCPKG_${VCPKG_TARGET_SELECTOR}_GIT_REF})
set(VCPKG_GIT_REF_FETCH ${VCPKG_${VCPKG_TARGET_SELECTOR}_GIT_REF_FETCH})


# Generate portfile.cmake with selected versioning and configuration information.
configure_file(${VCPKG_PORT_FILE_IN} ${VCPKG_PORT_FILE_OUT} @ONLY)