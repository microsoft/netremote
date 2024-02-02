
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
)

target_link_libraries(wpa-controller-test-unit
    PRIVATE
        Catch2::Catch2
        magic_enum::magic_enum
        plog::plog
        strings
        wpa-controller
)

catch_discover_tests(wpa-controller-test-unit)
