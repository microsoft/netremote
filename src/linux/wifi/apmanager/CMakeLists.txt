
add_library(wifi-apmanager-linux STATIC "")

set(WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi)
set(WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE_PREFIX ${WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE}/${WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-apmanager-linux
    PRIVATE
        AccessPointDiscoveryAgentOperationsNetlink.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE}
    FILES
        ${WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointDiscoveryAgentOperationsNetlink.hxx
)

target_link_libraries(wifi-apmanager-linux
    PRIVATE
        nl
        plog::plog
    PUBLIC
        libnl-helpers
        wifi-apmanager
        wifi-core
)

install(
    TARGETS wifi-apmanager-linux
    EXPORT ${PROJECT_NAME}
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${WIFI_APMANAGER_LINUX_PUBLIC_INCLUDE_SUFFIX}"
)
