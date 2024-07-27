
add_library(wifi-core STATIC "")

set(WIFI_CORE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_CORE_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi)
set(WIFI_CORE_PUBLIC_INCLUDE_PREFIX ${WIFI_CORE_PUBLIC_INCLUDE}/${WIFI_CORE_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-core
    PRIVATE
        AccessPoint.cxx
        AccessPointController.cxx
        AccessPointOperationStatus.cxx
        AccessPointOperationStatusLogOnExit.cxx
        Ieee80211.cxx
        Ieee80211AccessPointCapabilities.cxx
        Ieee80211Authentication.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_CORE_PUBLIC_INCLUDE}
    FILES
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPoint.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPointAttributes.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPointController.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPointOperationStatus.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPointOperationStatusLogOnExit.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/IAccessPoint.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/IAccessPointController.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/Ieee80211.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/Ieee80211AccessPointCapabilities.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/Ieee80211AccessPointConfiguration.hxx
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/Ieee80211Authentication.hxx
)

target_link_libraries(wifi-core
    PRIVATE
        magic_enum::magic_enum
        notstd
        strings
    PUBLIC
        ${PROJECT_NAME}-net
        plog::plog
)

install(
    TARGETS wifi-core
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${WIFI_CORE_PUBLIC_INCLUDE_SUFFIX}"
)
