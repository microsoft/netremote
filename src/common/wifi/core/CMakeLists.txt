
add_library(wifi-core STATIC "")

set(WIFI_CORE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_CORE_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi)
set(WIFI_CORE_PUBLIC_INCLUDE_PREFIX ${WIFI_CORE_PUBLIC_INCLUDE}/${WIFI_CORE_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-core
    PRIVATE
        AccessPoint.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_CORE_PUBLIC_INCLUDE}
    FILES
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPoint.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/IAccessPoint.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/IAccessPointFactory.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/Ieee80211.hxx
)

install(
    TARGETS wifi-core
    EXPORT ${PROJECT_NAME}
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${WIFI_CORE_PUBLIC_INCLUDE_SUFFIX}"
)
