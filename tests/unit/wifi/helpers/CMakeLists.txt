
add_library(wifi-test-helpers STATIC "")

set(WIFI_TEST_HELPERS_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE}/microsoft/net/wifi/test)

target_sources(wifi-test-helpers
    PRIVATE
        AccessPointFactoryTest.cxx
    PUBLIC
        ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryTest.hxx
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

list(APPEND WIFI_TEST_HELPERS_PUBLIC_HEADERS
    ${WIFI_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX}/AccessPointFactoryTest.hxx
)

set_target_properties(wifi-test-helpers PROPERTIES
    PUBLIC_HEADER "${WIFI_TEST_HELPERS_PUBLIC_HEADERS}"
)
