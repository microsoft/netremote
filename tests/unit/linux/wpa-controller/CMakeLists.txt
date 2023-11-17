
add_executable(wpa-controller-test-unit)

target_sources(wpa-controller-test-unit
    PRIVATE
        Main.cxx
        TestWpaController.cxx
)

target_link_libraries(wpa-controller-test-unit
    PRIVATE
        Catch2::Catch2WithMain
        wpa-controller
)

catch_discover_tests(wpa-controller-test-unit)
