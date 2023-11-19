
add_executable(wpa-controller-test-unit)

target_sources(wpa-controller-test-unit
    PRIVATE
        TestWpaController.cxx
        WpaDaemonCatch2EventListener.cxx
        WpaDaemonManager.cxx
)

target_link_libraries(wpa-controller-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        magic_enum::magic_enum
        wpa-controller
)

catch_discover_tests(wpa-controller-test-unit)
