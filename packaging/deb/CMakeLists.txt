
# Common CPack configuration for all packages.
set(CPACK_PACKAGE_DESCRIPTION "A framework for remotely controlling network componments such as wi-fi access points.")
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

# .deb specific configuration.
set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "netremoteowners@microsoft.com")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/microsoft/netremote")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_COMPONENTS_ALL netremote-server)

include(CPack)
