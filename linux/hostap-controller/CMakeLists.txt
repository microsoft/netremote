
add_library(hostap-controller SHARED "")

if (BUILD_HOSTAP_EXTERNAL)
    add_dependencies(hostap-controller hostap)
endif()

target_sources(hostap-controller
    PRIVATE
        Placeholder.cxx    
)

target_link_libraries(hostap-controller
    INTERFACE
        ${LIBWPA_CLIENT_TARGET}
)
