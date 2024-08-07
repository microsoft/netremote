
add_library(${PROJECT_NAME}-cli STATIC "")

set(NETREMOTE_CLI_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NETREMOTE_CLI_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote)
set(NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX ${NETREMOTE_CLI_PUBLIC_INCLUDE}/${NETREMOTE_CLI_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-cli
    PRIVATE
        NetRemoteCli.cxx
        NetRemoteCliHandler.cxx
        NetRemoteCliHandlerOperations.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NETREMOTE_CLI_PUBLIC_INCLUDE}
    FILES
        ${NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX}/INetRemoteCliHandlerOperations.hxx
        ${NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX}/NetRemoteCli.hxx
        ${NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX}/NetRemoteCliData.hxx
        ${NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX}/NetRemoteCliHandler.hxx
        ${NETREMOTE_CLI_PUBLIC_INCLUDE_PREFIX}/NetRemoteCliHandlerOperations.hxx
)

target_link_libraries(${PROJECT_NAME}-cli
    PRIVATE
        CLI11::CLI11
        magic_enum::magic_enum
        notstd
        plog::plog
        strings
    PUBLIC
        ${PROJECT_NAME}-client
        wifi-core
        wifi-core-adapter-dot11
)

install(
    TARGETS ${PROJECT_NAME}-cli
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NETREMOTE_CLI_PUBLIC_INCLUDE_SUFFIX}"
)
