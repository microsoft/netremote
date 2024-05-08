
add_executable(${PROJECT_NAME}-net-linux-test-unit)

target_sources(${PROJECT_NAME}-net-linux-test-unit
    PRIVATE
        Main.cxx
        TestNetworkOperationsLinux.cxx
)

target_include_directories(${PROJECT_NAME}-net-linux-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}-net-linux-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        ${PROJECT_NAME}-net-linux
)

catch_discover_tests(${PROJECT_NAME}-net-linux-test-unit)

