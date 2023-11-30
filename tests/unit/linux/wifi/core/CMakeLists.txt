
add_executable(wifi-core-linux-test-unit)

target_sources(wifi-core-linux-test-unit
    PRIVATE
        TestAccessPointLinux.cxx
        TestAccessPointFactoryLinux.cxx
)

target_include_directories(wifi-core-linux-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wifi-core-linux-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        wifi-core-linux
        wifi-test-helpers
)

catch_discover_tests(wifi-core-linux-test-unit)
