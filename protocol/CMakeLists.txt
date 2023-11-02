
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_DIR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(PROTO_FILES
    ${PROTO_DIR}/WifiApControl.proto
    ${PROTO_DIR}/WifiApControlService.proto
    ${PROTO_DIR}/WifiCore.proto
)

add_library(wifiapctrl-protocol ${PROTO_FILES})

target_link_libraries(wifiapctrl-protocol
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

target_include_directories(wifiapctrl-protocol
    PUBLIC
        "$<BUILD_INTERFACE:${PROTO_DIR_BINARY}>"
)

protobuf_generate(TARGET wifiapctrl-protocol
    LANGUAGE cpp
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
    PROTOC_OUT_FILES ${PROTO_FILES}
)

protobuf_generate(TARGET wifiapctrl-protocol
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${PROTO_DIR_BINARY}
)
