
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/microsoft/wifi/remote)
set(PROTO_DIR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(PROTO_FILES
    ${PROTO_DIR}/WifiCore.proto
    ${PROTO_DIR}/WifiRemote.proto
    ${PROTO_DIR}/WifiRemoteService.proto
)

add_library(wifiremote-protocol ${PROTO_FILES})

target_link_libraries(wifiremote-protocol
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

target_include_directories(wifiremote-protocol
    PUBLIC
        "$<BUILD_INTERFACE:${PROTO_DIR_BINARY}>"
)

protobuf_generate(TARGET wifiremote-protocol
    LANGUAGE cpp
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
    PROTOC_OUT_FILES ${PROTO_FILES}
)

protobuf_generate(TARGET wifiremote-protocol
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
)
