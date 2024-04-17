
install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/99-disable-network-config.cfg
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/cloud/cloud.cfg.d
        COMPONENT config-netplan
)
