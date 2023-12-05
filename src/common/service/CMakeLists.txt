
add_library(netremote-service STATIC "")

set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote)
set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}/${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX})

target_sources(netremote-service
    PRIVATE
        NetRemoteService.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/NetRemoteService.hxx
)

target_link_libraries(netremote-service
    PUBLIC
        netremote-protocol
)

install(
    TARGETS netremote-service
    EXPORT netremote
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX}"
)
