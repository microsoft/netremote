
add_library(wifi-core STATIC "")

set(WIFI_CORE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_CORE_PUBLIC_INCLUDE_PREFIX ${WIFI_CORE_PUBLIC_INCLUDE}/microsoft/net/wifi)

target_sources(wifi-core
    PRIVATE
        AccessPoint.cxx
    PUBLIC
        ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPoint.hxx
)

target_include_directories(wifi-core
    PUBLIC
        ${WIFI_CORE_PUBLIC_INCLUDE}
)

list(APPEND WIFI_CORE_PUBLIC_HEADERS
    ${WIFI_CORE_PUBLIC_INCLUDE_PREFIX}/AccessPoint.hxx
)

set_target_properties(wifi-core PROPERTIES
    PUBLIC_HEADER "${WIFI_CORE_PUBLIC_HEADERS}"
)

install(
    TARGETS wifi-core
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${WIFI_CORE_PUBLIC_HEADERS}"
)

