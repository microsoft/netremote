
add_subdirectory(external)
add_subdirectory(libnl-helpers)
add_subdirectory(wifi)
add_subdirectory(wpa-controller)

if (NOT NETREMOTE_EXCLUDE_TOOLS)
    add_subdirectory(tools)
endif()

if (NOT NETREMOTE_EXCLUDE_SERVER)
    add_subdirectory(server)
endif()
