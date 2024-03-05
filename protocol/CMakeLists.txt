
set(NETREMOTE_PROTOCOL_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NETREMOTE_PROTOCOL_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote/protocol)
set(NETREMOTE_PROTOCOL_PUBLIC_INCLUDE_PREFIX ${NETREMOTE_PROTOCOL_PUBLIC_INCLUDE}/${NETREMOTE_PROTOCOL_PUBLIC_INCLUDE_SUFFIX})

set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_DIR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${NETREMOTE_PROTOCOL_PUBLIC_INCLUDE_SUFFIX})
set(PROTO_FILES
    ${PROTO_DIR}/NetRemote.proto
    ${PROTO_DIR}/NetRemoteDataStream.proto
    ${PROTO_DIR}/NetRemoteDataStreamingService.proto
    ${PROTO_DIR}/NetRemoteService.proto
    ${PROTO_DIR}/NetRemoteWifi.proto
    ${PROTO_DIR}/WifiCore.proto
)

# Create a list of the generated headers. The below code isn't used because
# we've encountered intermittent errors when building with external vcpkg (eg.
# via port) which results in empty file lists and thus, the generated headers
# are not installed. It appears to be some sort of race condition in CMake
# itself.
#
# file(GLOB PROTO_FILES_GENERATED "${PROTO_DIR_BINARY}/*.pb.h")
# string(REGEX REPLACE "[.]proto$" ".pb.h" PROTO_HEADERS_GENERATED "${PROTO_FILES_GENERATED}")
# string(REGEX REPLACE "[.]proto$" ".pb.grpc.h" PROTO_HEADERS_GENERATED_GRPC "${PROTO_FILES_GENERATED}")
#
set(PROTO_HEADERS_GENERATED
    ${PROTO_DIR_BINARY}/NetRemote.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteDataStream.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteDataStreamingService.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteService.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteWifi.pb.h
    ${PROTO_DIR_BINARY}/WifiCore.pb.h
)

set(PROTO_HEADERS_GENERATED_GRPC
    ${PROTO_DIR_BINARY}/NetRemote.grpc.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteDataStream.grpc.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteDataStreamingService.grpc.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteService.grpc.pb.h
    ${PROTO_DIR_BINARY}/NetRemoteWifi.grpc.pb.h
    ${PROTO_DIR_BINARY}/WifiCore.grpc.pb.h
)

add_library(${PROJECT_NAME}-protocol STATIC "")

target_sources(${PROJECT_NAME}-protocol
    PRIVATE
        ${PROTO_FILES}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS 
        ${CMAKE_CURRENT_BINARY_DIR}
        ${NETREMOTE_PROTOCOL_PUBLIC_INCLUDE}
    FILES
        ${PROTO_HEADERS_GENERATED}
        ${PROTO_HEADERS_GENERATED_GRPC}
        ${NETREMOTE_PROTOCOL_PUBLIC_INCLUDE_PREFIX}/NetRemoteProtocol.hxx
)

target_link_libraries(${PROJECT_NAME}-protocol
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc_unsecure
        gRPC::grpc++_unsecure
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
    COMPONENT dev
    FILE_SET HEADERS
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${PROTO_DIR_BINARY_SUFFIX}"
)
