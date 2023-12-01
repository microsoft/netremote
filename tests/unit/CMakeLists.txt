
find_package(Catch2 3 REQUIRED)

# Allow CTest to discover Catch2 tests automatically
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")
include(Catch)

add_executable(netremote-test-unit)

target_sources(netremote-test-unit
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteCommon.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteServer.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestNetRemoteServiceClient.cxx
)

target_link_libraries(netremote-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        gRPC::grpc++
        magic_enum::magic_enum
        netremote-server
)

catch_discover_tests(netremote-test-unit)

add_subdirectory(wifi)

if (BUILD_FOR_LINUX)
    add_subdirectory(linux)
endif()
