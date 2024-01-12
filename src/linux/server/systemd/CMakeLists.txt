
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/netremote-server.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/netremote-server.service
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/netremote-server.service
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/systemd/system
)