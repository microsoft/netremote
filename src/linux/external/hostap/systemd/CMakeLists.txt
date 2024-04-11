
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd-example.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd-example.conf
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd-debug@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd-debug@.service
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/hostapd-release@.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hostapd-release@.service
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
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd-release@.service
    CONFIGURATIONS Release
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    RENAME hostapd@.service
    COMPONENT hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd-debug@.service
    CONFIGURATIONS Debug RelWithDebInfo MinSizeRel
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    RENAME hostapd@.service
    COMPONENT hostapd
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/hostapd-genconfig@.service
    CONFIGURATIONS Debug RelWithDebInfo MinSizeRel
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    COMPONENT hostapd
)
