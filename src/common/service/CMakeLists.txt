
add_library(${PROJECT_NAME}-service STATIC "")

set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote)
set(NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}/${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-service
    PRIVATE
        NetRemoteService.cxx
        NetRemoteCallbackService.cxx
        NetRemoteApiTrace.hxx
        NetRemoteApiTrace.cxx 
        NetRemoteWifiApiTrace.hxx
        NetRemoteWifiApiTrace.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/NetRemoteService.hxx
        ${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_PREFIX}/NetRemoteCallbackService.hxx
)

target_link_libraries(${PROJECT_NAME}-service
    PUBLIC
        ${PROJECT_NAME}-protocol
        wifi-apmanager
    PRIVATE
        logging-utils
        plog::plog
        wifi-core-adapter-dot11
)

install(
    TARGETS ${PROJECT_NAME}-service
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NET_REMOTE_SERVICE_PUBLIC_INCLUDE_SUFFIX}"
)
