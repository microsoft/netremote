# CMake integration for NuGet package building
# This file provides targets for building and packing the NetRemote client NuGet package

find_program(DOTNET_EXECUTABLE dotnet)

if(NOT DOTNET_EXECUTABLE)
    message(WARNING "dotnet CLI not found. NuGet package targets will not be available.")
    return()
endif()

set(NUGET_PROJECT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/NetRemoteClient.csproj")
set(NUGET_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/out/nuget")

# Update the version in the .csproj file based on the project version
set(NUGET_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Custom target to restore NuGet package dependencies
add_custom_target(nuget-restore
    COMMAND ${DOTNET_EXECUTABLE} restore "${NUGET_PROJECT_FILE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Restoring NuGet package dependencies..."
    VERBATIM
)

# Custom target to build the NuGet package project
add_custom_target(nuget-build
    COMMAND ${DOTNET_EXECUTABLE} build "${NUGET_PROJECT_FILE}" -c Release
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS nuget-restore
    COMMENT "Building NetRemote client library..."
    VERBATIM
)

# Custom target to create the NuGet package
add_custom_target(nuget-pack
    COMMAND ${DOTNET_EXECUTABLE} pack "${NUGET_PROJECT_FILE}"
        -c Release
        -o "${NUGET_OUTPUT_DIR}"
        -p:Version=${NUGET_VERSION}
        --no-build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS nuget-build
    COMMENT "Creating NuGet package Microsoft.Net.Remote.Client ${NUGET_VERSION}..."
    VERBATIM
)

# Custom target to clean NuGet build artifacts
add_custom_target(nuget-clean
    COMMAND ${DOTNET_EXECUTABLE} clean "${NUGET_PROJECT_FILE}" -c Release
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_CURRENT_SOURCE_DIR}/obj"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning NuGet build artifacts..."
    VERBATIM
)

# Add a convenience target that does everything
add_custom_target(nuget ALL
    DEPENDS nuget-pack
    COMMENT "NuGet package build complete"
)

message(STATUS "NuGet packaging enabled. Use 'cmake --build . --target nuget-pack' to create the package.")
message(STATUS "NuGet package version: ${NUGET_VERSION}")
message(STATUS "NuGet output directory: ${NUGET_OUTPUT_DIR}")
