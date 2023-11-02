
find_package(Catch2 3 REQUIRED)

# Allow CTest to discover Catch2 tests automatically
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")
include(Catch)

add_executable(wifiremote-test-unit)

target_sources(wifiremote-test-unit
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Main.cxx
        ${CMAKE_CURRENT_LIST_DIR}/TestWifiRemoteServiceClient.cxx
)

target_link_libraries(wifiremote-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        gRPC::grpc++
        wifiremote-service
)

catch_discover_tests(wifiremote-test-unit)
