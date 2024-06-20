
add_executable(libnl-helpers-test-unit)

target_sources(libnl-helpers-test-unit
    PRIVATE
        Main.cxx
        TestNetlink80211Interface.cxx
        TestNetlink80211ProtocolState.cxx
        TestNetlinkException.cxx
        TestNetlinkRoute.cxx
)

target_include_directories(libnl-helpers-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(libnl-helpers-test-unit
    PRIVATE
        Catch2::Catch2
        libnl-helpers
        magic_enum::magic_enum
)

catch_discover_tests(libnl-helpers-test-unit)
