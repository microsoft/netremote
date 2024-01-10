
add_subdirectory(external)
add_subdirectory(libnl-helpers)
add_subdirectory(wifi)
add_subdirectory(wpa-controller)

if (NETREMOTE_FEATURE_TOOLS)
    add_subdirectory(tools)
endif()

if (NETREMOTE_FEATURE_SERVER)
    add_subdirectory(server)
endif()