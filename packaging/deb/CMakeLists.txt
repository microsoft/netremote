
# Common CPack configuration for all packages.
set(CPACK_PACKAGE_DESCRIPTION "A framework for remotely controlling network components such as wi-fi access points.")
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_SET_DESTDIR ON)

# .deb specific configuration.
set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "netremoteowners@microsoft.com")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/microsoft/netremote")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_ALL
    server 
    hostapd
)

include(CPack)
