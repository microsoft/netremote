
add_library(wifi-test-helpers-linux STATIC "")

set(WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE_SUFFIX microsoft/net/wifi/test)
set(WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE_PREFIX ${WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE}/${WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE_SUFFIX})

target_sources(wifi-test-helpers-linux
    PRIVATE
        AccessPointFactoryLinuxTest.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE}
    FILES
        ${WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryLinuxTest.hxx
)

target_include_directories(wifi-test-helpers-linux
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        ${WIFI_TEST_HELPERS_LINUX_PUBLIC_INCLUDE}
)

target_link_libraries(wifi-test-helpers-linux
    PUBLIC
        wifi-core-linux
        wifi-test-helpers
)
