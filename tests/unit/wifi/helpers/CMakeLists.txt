
add_library(wifi-test-helpers STATIC "")

set(WIFI_TEST_HELPERS_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_TEST_HELPERS_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi/test)
set(WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE}/${WIFI_TEST_HELPERS_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-test-helpers
    PRIVATE
        AccessPointControllerTest.cxx
        AccessPointTest.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE}
    FILES
        ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX}/AccessPointControllerTest.hxx
        ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX}/AccessPointTest.hxx
)

target_include_directories(wifi-test-helpers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE}
)

target_link_libraries(wifi-test-helpers
    PUBLIC
        wifi-core
)
