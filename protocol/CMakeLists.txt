
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_DIR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(PROTO_FILES
    ${PROTO_DIR}/WifiCore.proto
    ${PROTO_DIR}/NetRemote.proto
    ${PROTO_DIR}/NetRemoteService.proto
)

add_library(netremote-protocol STATIC "")

target_sources(netremote-protocol
    PRIVATE
        ${PROTO_FILES}
)

target_include_directories(netremote-protocol
    PUBLIC
        "$<BUILD_INTERFACE:${PROTO_DIR_BINARY}>"
)

target_link_libraries(netremote-protocol
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

protobuf_generate(TARGET netremote-protocol
    LANGUAGE cpp
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
    PROTOC_OUT_FILES ${PROTO_FILES}
)

protobuf_generate(TARGET netremote-protocol
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
)
