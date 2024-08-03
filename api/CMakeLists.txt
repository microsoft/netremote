
set(PROTO_DIR ${CMAKE_CURRENT_LIST_DIR}/protos)

# List of all protocol buffer files defining the API.
set(PROTO_FILES
    ${PROTO_DIR}/NetRemoteDataStream.proto
    ${PROTO_DIR}/NetRemoteDataStreamingService.proto
    ${PROTO_DIR}/NetRemoteNetwork.proto
    ${PROTO_DIR}/NetRemoteService.proto
    ${PROTO_DIR}/NetRemoteWifi.proto
    ${PROTO_DIR}/Network8021x.proto
    ${PROTO_DIR}/NetworkCore.proto
    ${PROTO_DIR}/WifiCore.proto
)

# Install the .proto files for consumers that wish to build them themselves.
install(
    FILES
    ${PROTO_FILES}
    DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/api"
)

# Include the API bindings (protocol) if they are not excluded.
if (NOT NETREMOTE_EXCLUDE_API_BINDINGS)
    add_subdirectory(protocol)
endif()
