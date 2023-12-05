
add_subdirectory(common)

# Conditional inclusion of OS-dependent source trees.
if (BUILD_FOR_LINUX)
    add_subdirectory(linux)
elseif (BUILD_FOR_WINDOWS)
    add_subdirectory(windows)
endif()

# Install the common netremote export
install(
    EXPORT ${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
