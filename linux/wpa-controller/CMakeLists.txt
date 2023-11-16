
add_library(wpa-controller STATIC "")

set(WPA_CONTROLLER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX ${WPA_CONTROLLER_PUBLIC_INCLUDE}/Wpa)

if (BUILD_HOSTAP_EXTERNAL)
    add_dependencies(wpa-controller hostap)

    target_include_directories(wpa-controller
        PRIVATE
            ${HOSTAP_LOCAL_INCLUDE_DIR}
    )
endif()

target_sources(wpa-controller
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/WpaController.cxx
)

target_include_directories(wpa-controller
    PUBLIC
        ${WPA_CONTROLLER_PUBLIC_INCLUDE}
)

target_link_libraries(wpa-controller
    INTERFACE
        ${LIBWPA_CLIENT_TARGET}
)

list(APPEND WPA_CONTROLLER_PUBLIC_HEADERS
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaController.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCore.hxx
)

set_target_properties(wpa-controller PROPERTIES
    PUBLIC_HEADER "${WPA_CONTROLLER_PUBLIC_HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME wpa-controller
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
