
add_executable(${PROJECT_NAME}-test-unit)

target_sources(${PROJECT_NAME}-test-unit
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Main.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteCommon.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteServer.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteServiceClient.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteDataStreamingServiceClient.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteDataStreamingReactors.cxx
)

target_link_libraries(${PROJECT_NAME}-test-unit
    PRIVATE
        ${PROJECT_NAME}-net
        ${PROJECT_NAME}-net-test-helpers
        ${PROJECT_NAME}-server
        Catch2::Catch2
        gRPC::grpc++
        magic_enum::magic_enum
        plog::plog
        wifi-test-helpers
)

catch_discover_tests(${PROJECT_NAME}-test-unit)

add_subdirectory(net)

if (BUILD_FOR_LINUX)
    add_subdirectory(linux)
endif()
