
add_library(netremote-server STATIC "")

set(NET_REMOTE_SERVER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}/microsoft/net/remote)

target_sources(netremote-server
    PRIVATE
        NetRemoteServer.cxx
    PUBLIC
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServer.hxx
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

install(
    TARGETS netremote-server
    EXPORT netremote
)
