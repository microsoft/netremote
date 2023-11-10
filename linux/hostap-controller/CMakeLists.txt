
add_library(hostap-controller SHARED "")

if (BUILD_HOSTAP_EXTERNAL)
    add_dependencies(hostap-controller hostap)

    target_include_directories(hostap-controller
        PRIVATE
            ${HOSTAP_LOCAL_INCLUDE_DIR}
    )
endif()

target_sources(hostap-controller
    PRIVATE
        Placeholder.cxx    
)


target_link_libraries(hostap-controller
    INTERFACE
        ${LIBWPA_CLIENT_TARGET}
)
