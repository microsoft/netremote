
add_library(wpa-controller SHARED "")

if (BUILD_HOSTAP_EXTERNAL)
    add_dependencies(wpa-controller hostap)

    target_include_directories(wpa-controller
        PRIVATE
            ${HOSTAP_LOCAL_INCLUDE_DIR}
    )
endif()

target_sources(wpa-controller
    PRIVATE
        Placeholder.cxx    
)


target_link_libraries(wpa-controller
    INTERFACE
        ${LIBWPA_CLIENT_TARGET}
)
