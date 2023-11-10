
if (BUILD_HOSTAP_EXTERNAL)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../external/hostap)
else()
    find_library(LIBWPA_CLIENT
        NAMES libwpa_client.so
        REQUIRED)
endif()

if(LIBWPA_CLIENT)
    set(LIBWPA_CLIENT_TARGET ${LIBWPA_CLIENT})
    MESSAGE(STATUS "Found wpa client: ${LIBWPA_CLIENT}")
endif()
