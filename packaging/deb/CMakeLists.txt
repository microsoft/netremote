
# Uncomment below to debug package generation.
# set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

# Common CPack configuration for all packages.
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_PACKAGE_CONTACT "netremoteowners@microsoft.com")

# .deb specific configuration.
set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)
set(CPACK_COMPONENTS_ALL
    server 
    hostapd
    dev
    network-config
)

add_subdirectory(network-config)

include(CPack)
