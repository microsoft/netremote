
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd-example.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd-example.conf
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd-genconfig@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd-genconfig@.service
    @ONLY
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd-example.conf
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/hostapd
    COMPONENT hostapd
)

install(
    DIRECTORY
    DESTINATION ${CMAKE_INSTALL_RUNSTATEDIR}/hostapd
    COMPONENT hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd@.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    COMPONENT hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd-genconfig@.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    COMPONENT hostapd
)
