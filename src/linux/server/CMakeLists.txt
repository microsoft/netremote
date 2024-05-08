
add_executable(${PROJECT_NAME}-server-linux "")

target_sources(${PROJECT_NAME}-server-linux
    PRIVATE
        Main.cxx
        NetRemoteDiscoveryServiceLinuxDnssd.cxx
        NetRemoteDiscoveryServiceLinuxDnssd.hxx
)

target_link_libraries(${PROJECT_NAME}-server-linux
    PRIVATE
        ${PROJECT_NAME}-net-linux
        ${PROJECT_NAME}-server
        logging-utils
        notstd
        plog::plog
        SDBusCpp::sdbus-c++
        wifi-apmanager-linux
        wifi-core-linux
)

set_target_properties(${PROJECT_NAME}-server-linux
    PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}-server
)

install(
    TARGETS ${PROJECT_NAME}-server-linux
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    EXPORT ${PROJECT_NAME}
    COMPONENT server
)

add_subdirectory(systemd)
