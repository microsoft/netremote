
find_package(sdbus-c++ REQUIRED)

# The d-bus vcpkg port fails to export the libsystemd dependency properly via an imported target which causes the build
# to fail. This is a workaround to fix that by finding the libsystemd dependency via pkg-config and defining a global
# imported target.
include(FindPkgConfig)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET GLOBAL libsystemd)

add_subdirectory(external)
add_subdirectory(libnl-helpers)
add_subdirectory(net)
add_subdirectory(wifi)

if (NOT NETREMOTE_EXCLUDE_TOOLS)
    add_subdirectory(tools)
endif()

if (NOT NETREMOTE_EXCLUDE_SERVER)
    add_subdirectory(server)
endif()
