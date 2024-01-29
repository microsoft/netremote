
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd.example.conf
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd.example.conf
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
    @ONLY
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd.example.conf
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
)
