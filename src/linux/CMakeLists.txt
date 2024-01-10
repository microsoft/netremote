
add_subdirectory(external)
add_subdirectory(libnl-helpers)
add_subdirectory(wifi)
add_subdirectory(wpa-controller)

if (NOT NETREMOTE_VCPKG_BUILD_FOR_PORT OR NETREMOTE_FEATURE_TOOLS)
    add_subdirectory(tools)
endif()

if (NOT NETREMOTE_VCPKG_BUILD_FOR_PORT OR NETREMOTE_FEATURE_SERVER)
    add_subdirectory(server)
endif()