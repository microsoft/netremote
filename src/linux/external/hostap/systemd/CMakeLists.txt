
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd.conf
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd.conf)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd.conf
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/systemd/system
)