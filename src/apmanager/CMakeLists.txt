
add_library(apmanager STATIC "")

set(APMANAGER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(APMANAGER_PUBLIC_INCLUDE_PREFIX ${APMANAGER_PUBLIC_INCLUDE}/microsoft/net/wifi)

target_sources(apmanager
    PRIVATE
        AccessPointManager.cxx
    PUBLIC
        ${APMANAGER_PUBLIC_INCLUDE_PREFIX}/AccessPointManager.hxx
)

target_include_directories(apmanager
    PUBLIC
        ${APMANAGER_PUBLIC_INCLUDE}
)

target_link_libraries(apmanager
    PUBLIC
        netremote-protocol
        wifi-core
)

list(APPEND APMANAGER_PUBLIC_HEADERS
    ${APMANAGER_PUBLIC_INCLUDE_PREFIX}/AccessPointManager.hxx
)

set_target_properties(apmanager PROPERTIES
    PUBLIC_HEADER "${APMANAGER_PUBLIC_HEADERS}"
)

install(
    TARGETS apmanager
    EXPORT netremote
    PUBLIC_HEADER DESTINATION "${APMANAGER_PUBLIC_HEADERS}"
)
