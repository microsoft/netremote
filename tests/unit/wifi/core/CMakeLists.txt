
add_executable(wifi-core-test-unit)

target_sources(wifi-core-test-unit
    PRIVATE
        detail/AccessPointFactoryTest.cxx
        detail/AccessPointFactoryTest.hxx
        TestAccessPoint.cxx
)

target_include_directories(wifi-core-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wifi-core-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        magic_enum::magic_enum
        strings
        wifi-core
)

catch_discover_tests(wifi-core-test-unit)
