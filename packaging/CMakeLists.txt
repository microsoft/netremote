
add_subdirectory(vcpkg)

if (BUILD_FOR_LINUX)
  add_subdirectory(deb)
endif()
