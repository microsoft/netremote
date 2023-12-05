
add_library(wifi-core-linux STATIC "")

set(WIFI_CORE_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_CORE_LINUX_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi)
set(WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX ${WIFI_CORE_LINUX_PUBLIC_INCLUDE}/${WIFI_CORE_LINUX_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-core-linux
    PRIVATE
        AccessPointLinux.cxx
        AccessPointFactoryLinux.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_CORE_LINUX_PUBLIC_INCLUDE}
    FILES
        ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointLinux.hxx
        ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryLinux.hxx
)

target_link_libraries(wifi-core-linux
    PUBLIC
        wifi-core
)

install(
    TARGETS wifi-core-linux
    EXPORT netremote
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_PUBLIC_HEADER_BASE}/${WIFI_CORE_LINUX_PUBLIC_INCLUDE_SUFFIX}"
)
