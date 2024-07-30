
add_library(${PROJECT_NAME}-server STATIC "")

set(NET_REMOTE_SERVER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote/service)
set(NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}/${NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-server
    PRIVATE
        NetRemoteServer.cxx
        NetRemoteServerConfiguration.cxx
        NetRemoteServerJsonConfiguration.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_SERVER_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServer.hxx
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServerConfiguration.hxx
        ${NET_REMOTE_SERVER_PUBLIC_INCLUDE_PREFIX}/NetRemoteServerJsonConfiguration.hxx
)

target_link_libraries(${PROJECT_NAME}-server
    PUBLIC
        ${PROJECT_NAME}-network-manager
        ${PROJECT_NAME}-service
    PRIVATE
        CLI11::CLI11
        nlohmann_json::nlohmann_json
        plog::plog
)

install(
    TARGETS ${PROJECT_NAME}-server
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NET_REMOTE_SERVER_PUBLIC_INCLUDE_SUFFIX}"
)
