
add_library(wifi-apmanager STATIC "")

set(WIFI_APMANAGER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_APMANAGER_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi)
set(WIFI_APMANAGER_PUBLIC_INCLUDE_PREFIX ${WIFI_APMANAGER_PUBLIC_INCLUDE}/${WIFI_APMANAGER_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-apmanager
    PRIVATE
        AccessPointManager.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_APMANAGER_PUBLIC_INCLUDE}
    FILES
        ${WIFI_APMANAGER_PUBLIC_INCLUDE_PREFIX}/AccessPointManager.hxx
)

target_link_libraries(wifi-apmanager
    PUBLIC
        netremote-protocol
        wifi-core
)

install(
    TARGETS wifi-apmanager
    EXPORT netremote
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${WIFI_APMANAGER_PUBLIC_INCLUDE_SUFFIX}"
)
