
add_executable(wpa-controller-test-unit)

target_sources(wpa-controller-test-unit
    PRIVATE
        detail/WifiVirtualDeviceManager.cxx
        detail/WpaDaemonCatch2EventListener.cxx
        detail/WpaDaemonManager.cxx
        Main.cxx
        TestHostapd.cxx
        TestWpaController.cxx
)

target_include_directories(wpa-controller-test-unit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(wpa-controller-test-unit
    PRIVATE
        Catch2::Catch2
        magic_enum::magic_enum
        plog::plog
        strings
        wpa-controller
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/detail/config/HostapdBinaryInfo.hxx.in
    ${CMAKE_CURRENT_BINARY_DIR}/HostapdBinaryInfo.hxx
    @ONLY
)

catch_discover_tests(wpa-controller-test-unit)
