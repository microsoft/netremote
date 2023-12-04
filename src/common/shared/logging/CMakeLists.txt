
add_library(logging-utils STATIC "")

set(LOGUTILS_DIR_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(LOGUTILS_DIR_PUBLIC_INCLUDE_PREFIX ${LOGUTILS_DIR_PUBLIC_INCLUDE}/logging)

target_sources(logging-utils
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/LogUtils.cxx
    PUBLIC
        ${LOGUTILS_DIR_PUBLIC_INCLUDE_PREFIX}/LogUtils.hxx
)

target_include_directories(logging-utils
    PUBLIC
        ${LOGUTILS_DIR_PUBLIC_INCLUDE}
)

target_link_libraries(logging-utils 
    PUBLIC 
        plog::plog
)

list(APPEND LOGUTILS_PUBLIC_HEADERS
    ${LOGUTILS_DIR_PUBLIC_INCLUDE_PREFIX}/LogUtils.hxx
)

set_target_properties(logging-utils PROPERTIES PUBLIC_HEADER "${LOGUTILS_PUBLIC_HEADERS}")

install(
    TARGETS logging-utils
    EXPORT logging-utils
    ARCHIVE
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/logging
)
