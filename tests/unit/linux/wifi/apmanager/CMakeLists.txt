
add_executable(wifi-apmanager-linux-test-unit)

target_sources(wifi-apmanager-linux-test-unit
    PRIVATE
        TestAccessPointDiscoveryAgentOperationsNetlink.cxx
)

target_include_directories(wifi-apmanager-linux-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wifi-apmanager-linux-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        magic_enum::magic_enum
        strings
        wifi-apmanager-linux
)

catch_discover_tests(wifi-apmanager-linux-test-unit)
