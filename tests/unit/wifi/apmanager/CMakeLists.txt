
add_executable(wifi-wifi-apmanager-test-unit)

target_sources(wifi-wifi-apmanager-test-unit
    PRIVATE
        TestAccessPointManager.cxx
)

target_include_directories(wifi-wifi-apmanager-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wifi-wifi-apmanager-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        magic_enum::magic_enum
        strings
        wifi-apmanager
        wifi-test-helpers
)

catch_discover_tests(wifi-wifi-apmanager-test-unit)
