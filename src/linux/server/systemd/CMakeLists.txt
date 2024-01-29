
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/netremote-server.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/netremote-server.service
    @ONLY
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/netremote-server.service
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
    COMPONENT server
)
