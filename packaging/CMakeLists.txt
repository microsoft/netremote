
if (NOT NETREMOTE_VCPKG_BUILD_FOR_PORT)
  add_subdirectory(vcpkg)
endif()

if (BUILD_FOR_LINUX)
  add_subdirectory(deb)
endif()
