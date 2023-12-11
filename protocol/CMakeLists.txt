
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_DIR_BINARY_SUFFIX microsoft/net/remote/protocol) 
set(PROTO_DIR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_DIR_BINARY_SUFFIX})
set(PROTO_FILES
    ${PROTO_DIR}/NetRemote.proto
    ${PROTO_DIR}/NetRemoteService.proto
    ${PROTO_DIR}/NetRemoteWifi.proto
    ${PROTO_DIR}/WifiCore.proto
)

# Create a list of the generated headers.
file(GLOB PROTO_FILES_GENERATED "${PROTO_DIR_BINARY}/*.pb.h")
string(REGEX REPLACE "[.]proto$" ".pb.h" PROTO_HEADERS_GENERATED "${PROTO_FILES_GENERATED}")
string(REGEX REPLACE "[.]proto$" ".pb.grpc.h" PROTO_HEADERS_GENERATED_GRPC "${PROTO_FILES_GENERATED}")

add_library(${PROJECT_NAME}-protocol STATIC "")

target_sources(${PROJECT_NAME}-protocol
    PRIVATE
        ${PROTO_FILES}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
    FILES
        ${PROTO_HEADERS_GENERATED}
        ${PROTO_HEADERS_GENERATED_GRPC}
)

target_link_libraries(${PROJECT_NAME}-protocol
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

protobuf_generate(TARGET ${PROJECT_NAME}-protocol
    LANGUAGE cpp
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
    PROTOC_OUT_FILES ${PROTO_FILES}
)

protobuf_generate(TARGET ${PROJECT_NAME}-protocol
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
)

install(
    TARGETS ${PROJECT_NAME}-protocol
    EXPORT ${PROJECT_NAME}
    FILE_SET HEADERS
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${PROTO_DIR_BINARY_SUFFIX}
)
