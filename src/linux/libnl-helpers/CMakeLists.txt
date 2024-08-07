
add_library(libnl-helpers STATIC "")

set(LIBNL_HELPERS_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(LIBNL_HELPERS_PUBLIC_INCLUDE_SUFFIX microsoft/net/netlink)
set(LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX ${LIBNL_HELPERS_PUBLIC_INCLUDE}/${LIBNL_HELPERS_PUBLIC_INCLUDE_SUFFIX})

target_sources(libnl-helpers
    PRIVATE
        Ieee80211Nl80211Adapters.cxx
        Netlink80211.cxx
        Netlink80211Interface.cxx
        Netlink80211ProtocolState.cxx
        Netlink80211Wiphy.cxx
        Netlink80211WiphyBand.cxx
        Netlink80211WiphyBandFrequency.cxx
        NetlinkErrorCategory.cxx
        NetlinkException.cxx
        NetlinkMessage.cxx
        NetlinkRoute.cxx
        NetlinkSocket.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${LIBNL_HELPERS_PUBLIC_INCLUDE}
    FILES
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/NetlinkErrorCategory.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/NetlinkException.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/NetlinkMessage.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/NetlinkSocket.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/nl80211/Ieee80211Nl80211Adapters.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/nl80211/Netlink80211Interface.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/nl80211/Netlink80211ProtocolState.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/nl80211/Netlink80211Wiphy.hxx
        ${LIBNL_HELPERS_PUBLIC_INCLUDE_PREFIX}/route/NetlinkRoute.hxx
)

target_link_libraries(libnl-helpers
    PRIVATE
        magic_enum::magic_enum
        notstd
        plog::plog
        wifi-core
    PUBLIC
        nl
        nl-genl
        nl-route-3
)

install(
    TARGETS libnl-helpers
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${LIBNL_HELPERS_PUBLIC_INCLUDE_SUFFIX}"
)
