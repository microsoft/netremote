
add_library(wifi-apmanager STATIC "")

set(WIFI_APMANAGER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_APMANAGER_PUBLIC_INCLUDE_PREFIX ${WIFI_APMANAGER_PUBLIC_INCLUDE}/microsoft/net/wifi)

target_sources(wifi-apmanager
    PRIVATE
        AccessPointManager.cxx
    PUBLIC
        ${WIFI_APMANAGER_PUBLIC_INCLUDE_PREFIX}/AccessPointManager.hxx
)

target_include_directories(wifi-apmanager
    PUBLIC
        ${WIFI_APMANAGER_PUBLIC_INCLUDE}
)

target_link_libraries(wifi-apmanager
    PUBLIC
        netremote-protocol
        wifi-core
)

list(APPEND WIFI_APMANAGER_PUBLIC_HEADERS
    ${WIFI_APMANAGER_PUBLIC_INCLUDE_PREFIX}/AccessPointManager.hxx
)

set_target_properties(wifi-apmanager PROPERTIES
    PUBLIC_HEADER "${WIFI_APMANAGER_PUBLIC_HEADERS}"
)

install(
    TARGETS wifi-apmanager
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${WIFI_APMANAGER_PUBLIC_HEADERS}"
)
