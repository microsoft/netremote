find_package(Catch2 3 REQUIRED)

# Allow CTest to discover Catch2 tests automatically
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")
include(Catch)

add_executable(${PROJECT_NAME}-rfattenuator-linux-test-unit)

target_sources(${PROJECT_NAME}-rfattenuator-linux-test-unit
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Main.cxx
        ${CMAKE_CURRENT_LIST_DIR}/NetRemoteRfAttenuatorServiceClient.cxx
        ${CMAKE_CURRENT_LIST_DIR}/../../TestNetRemoteCommon.cxx
)

target_link_libraries(${PROJECT_NAME}-rfattenuator-linux-test-unit
    PRIVATE
        ${PROJECT_NAME}-net
        ${PROJECT_NAME}-net-test-helpers
        ${PROJECT_NAME}-server
        ${PROJECT_NAME}-rfattenuator
        Catch2::Catch2
        gRPC::grpc++
        magic_enum::magic_enum
        plog::plog
        wifi-test-helpers
)

catch_discover_tests(${PROJECT_NAME}-rfattenuator-linux-test-unit)
