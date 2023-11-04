
add_library(wifiremote-service STATIC "")

set(WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE}/microsoft/net/wifi/remote)

target_sources(wifiremote-service
    PRIVATE
        WifiRemoteService.cxx
    PUBLIC
        ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/WifiRemoteService.hxx
)

target_include_directories(wifiremote-service
    PUBLIC
        ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE}
)

target_link_libraries(wifiremote-service
    PUBLIC
        netremote-protocol
)

list(APPEND WIFI_REMOTE_SERVICE_PUBLIC_HEADERS
    ${WIFI_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/WifiRemoteService.hxx
)

set_target_properties(wifiremote-service PROPERTIES
    PUBLIC_HEADER "${WIFI_REMOTE_SERVICE_PUBLIC_HEADERS}"
)

install(
    TARGETS wifiremote-service
    EXPORT wifiremote
    PUBLIC_HEADER DESTINATION "${WIFI_REMOTE_SERVICE_PUBLIC_HEADERS}"
)
