
add_subdirectory(common)

# Conditional inclusion of OS-dependent source trees.
if (BUILD_FOR_LINUX)
    add_subdirectory(linux)
elseif (BUILD_FOR_WINDOWS)
    add_subdirectory(windows)
endif()

# Install the common netremote export
install(
    EXPORT netremote
    NAMESPACE netremote::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/netremote
)
