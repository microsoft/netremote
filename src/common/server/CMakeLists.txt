
add_library(netremote-server STATIC "")

set(NET_REMOTE_SERVER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}/${NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX})

target_sources(netremote-server
    PRIVATE
        NetRemoteServer.cxx
        NetRemoteServerConfiguration.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServer.hxx
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServerConfiguration.hxx
)

target_link_libraries(netremote-server
    PUBLIC
        netremote-service
    PRIVATE
        CLI11::CLI11
)

install(
    TARGETS netremote-server
    EXPORT netremote
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION ${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX}
)
