
add_library(${PROJECT_NAME}-net-test-helpers STATIC "")

set(NET_TEST_HELPERS_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_TEST_HELPERS_PUBLIC_INCLUDE_SUFFIX microsoft/net/test)
set(NET_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX ${NET_TEST_HELPERS_PUBLIC_INCLUDE}/${NET_TEST_HELPERS_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-net-test-helpers
    PRIVATE
        NetworkOperationsTest.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_TEST_HELPERS_PUBLIC_INCLUDE}
    FILES
        ${NET_TEST_HELPERS_PUBLIC_INCLUDE_PREFIX}/NetworkOperationsTest.hxx
)

target_include_directories(${PROJECT_NAME}-net-test-helpers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        ${NET_TEST_HELPERS_PUBLIC_INCLUDE}
)

target_link_libraries(netremote-net-test-helpers
    PUBLIC
        ${PROJECT_NAME}-net
)
