
add_subdirectory(client)
add_subdirectory(dotnet)
add_subdirectory(server)
add_subdirectory(service)
add_subdirectory(shared)
add_subdirectory(wifi)

# Conditional inclusion of OS-dependent source trees.
if (BUILD_FOR_LINUX)
    add_subdirectory(linux)
elseif (BUILD_FOR_WINDOWS)
    add_subdirectory(windows)
endif()
