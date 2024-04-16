
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/10-network-netremote-all-template.yaml
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/netplan
        COMPONENT config-netplan
)
