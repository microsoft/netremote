
add_executable(wifi-core-test-unit)

target_sources(wifi-core-test-unit
    PRIVATE
        Main.cxx
        TestAccessPoint.cxx
        TestAccessPointOperationStatus.cxx
        TestIeee80211.cxx
)

target_include_directories(wifi-core-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wifi-core-test-unit
    PRIVATE
        Catch2::Catch2
        magic_enum::magic_enum
        plog::plog
        strings
        wifi-core
        wifi-test-helpers
)

catch_discover_tests(wifi-core-test-unit)
