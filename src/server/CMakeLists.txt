
add_library(netremote-server STATIC "")

set(NET_REMOTE_SERVER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}/microsoft/net/remote)

target_sources(netremote-server
    PRIVATE
        NetRemoteServer.cxx
        NetRemoteServerConfiguration.cxx
    PUBLIC
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServer.hxx
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServerConfiguration.hxx
)

target_include_directories(netremote-server
    PUBLIC
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}
)

target_link_libraries(netremote-server
    PUBLIC
        netremote-service
    PRIVATE
        CLI11::CLI11
)

list(APPEND NET_REMOTE_SERVER_PUBLIC_HEADERS
    ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServer.hxx
    ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServerConfiguration.hxx
)

set_target_properties(netremote-server PROPERTIES
    PUBLIC_HEADER "${NET_REMOTE_SERVER_PUBLIC_HEADERS}"
)

install(
    TARGETS netremote-server
    EXPORT netremote
)
