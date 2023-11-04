
add_library(netremote-service STATIC "")

set(WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE}/microsoft/net/wifi/remote)

target_sources(netremote-service
    PRIVATE
        WifiRemoteService.cxx
    PUBLIC
        ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/WifiRemoteService.hxx
)

target_include_directories(netremote-service
    PUBLIC
        ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE}
)

target_link_libraries(netremote-service
    PUBLIC
        netremote-protocol
)

list(APPEND WIFI_REMOTE_SERVICE_PUBLIC_HEADERS
    ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/WifiRemoteService.hxx
)

set_target_properties(netremote-service PROPERTIES
    PUBLIC_HEADER "${WIFI_REMOTE_SERVICE_PUBLIC_HEADERS}"
)

install(
    TARGETS netremote-service
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${WIFI_REMOTE_SERVICE_PUBLIC_HEADERS}"
)
