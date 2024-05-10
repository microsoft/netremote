
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/10-network-netremote-all.yaml
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/netplan
        RENAME 10-network-netremote-all.yaml.template
        PERMISSIONS OWNER_READ OWNER_WRITE
        COMPONENT config-netplan
        CONFIGURATIONS Debug
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/10-network-netremote-all.yaml
        DESTINATION /${CMAKE_INSTALL_SYSCONFDIR}/netplan
        RENAME 10-network-netremote-all.yaml.template
        PERMISSIONS OWNER_READ OWNER_WRITE
        COMPONENT config-netplan
        CONFIGURATIONS Release RelWithDebInfo MinSizeRel
)
