
add_library(netremote-service STATIC "")

set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}/microsoft/net/remote)

target_sources(netremote-service
    PRIVATE
        NetRemoteService.cxx
    PUBLIC
        ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/NetRemoteService.hxx
)

target_include_directories(netremote-service
    PUBLIC
        ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}
)

target_link_libraries(netremote-service
    PUBLIC
        netremote-protocol
)

list(APPEND NET_REMOTE_SERVICE_PUBLIC_HEADERS
    ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/NetRemoteService.hxx
)

set_target_properties(netremote-service PROPERTIES
    PUBLIC_HEADER "${NET_REMOTE_SERVICE_PUBLIC_HEADERS}"
)

install(
    TARGETS netremote-service
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${NET_REMOTE_SERVICE_PUBLIC_HEADERS}"
)
