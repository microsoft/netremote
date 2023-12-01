
add_library(wifi-core-linux STATIC "")

set(WIFI_CORE_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX ${WIFI_CORE_LINUX_PUBLIC_INCLUDE}/microsoft/net/wifi)

target_sources(wifi-core-linux
    PRIVATE
        AccessPointLinux.cxx
        AccessPointFactoryLinux.cxx
    PUBLIC
        ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointLinux.hxx
        ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryLinux.hxx
)

target_include_directories(wifi-core-linux
    PUBLIC
        ${WIFI_CORE_LINUX_PUBLIC_INCLUDE}
)

target_link_libraries(wifi-core-linux
    PUBLIC
        wifi-core
)

list(APPEND WIFI_CORE_LINUX_PUBLIC_HEADERS
    ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointLinux.hxx
    ${WIFI_CORE_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryLinux.hxx
)

set_target_properties(wifi-core-linux PROPERTIES
    PUBLIC_HEADER "${WIFI_CORE_LINUX_PUBLIC_HEADERS}"
)

install(
    TARGETS wifi-core-linux
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${WIFI_CORE_LINUX_PUBLIC_HEADERS}"
)
