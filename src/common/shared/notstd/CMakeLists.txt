
add_library(notstd INTERFACE)

set(NOTSTD_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NOTSTD_PUBLIC_INCLUDE_SUFFIX notstd)
set(NOTSTD_PUBLIC_INCLUDE_PREFIX ${NOTSTD_PUBLIC_INCLUDE}/${NOTSTD_PUBLIC_INCLUDE_SUFFIX})

target_sources(notstd
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NOTSTD_PUBLIC_INCLUDE}
    FILES
        ${NOTSTD_PUBLIC_INCLUDE_PREFIX}/Exceptions.hxx
        ${NOTSTD_PUBLIC_INCLUDE_PREFIX}/Memory.hxx
        ${NOTSTD_PUBLIC_INCLUDE_PREFIX}/Scope.hxx
)

install(
    TARGETS notstd
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/{NOTSTD_PUBLIC_INCLUDE_SUFFIX}"
)
