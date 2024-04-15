
set(CPACK_DEBIAN_NETWORK-CONFIG_PACKAGE_CONTROL_EXTRA 
    "${CMAKE_CURRENT_LIST_DIR}/scripts/config"
    "${CMAKE_CURRENT_LIST_DIR}/scripts/postinst"
    "${CMAKE_CURRENT_LIST_DIR}/scripts/postrm"
    "${CMAKE_CURRENT_LIST_DIR}/scripts/templates" 
    PARENT_SCOPE
)
