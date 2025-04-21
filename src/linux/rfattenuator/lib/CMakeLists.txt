
add_library(${PROJECT_NAME}-rfattenuator STATIC "")

set(NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE_SUFFIX microsoft/net/remote/service)
set(NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE}/${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-rfattenuator
    PRIVATE
        RfAttenuatorAeroflexWeinschle83XX.cxx
        RfAttenuatorExceptionImpl.cxx
        RfAttenuatorFactoryImpl.cxx
        RfAttenuatorProtocol.cxx
        RfAttenuatorSoftwareSimulated.cxx
        RfAttenuatorTransport.cxx
        RfAttenuatorTransportSocketLinux.cxx
        SocketHelpers.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE_PREFIX}/RfAttenuatorFactory.hxx
)

target_link_libraries(${PROJECT_NAME}-rfattenuator
    PUBLIC
        ${PROJECT_NAME}-rfattenuator-interface
    PRIVATE
        strings
)

install(
    TARGETS ${PROJECT_NAME}-rfattenuator
    EXPORT ${PROJECT_NAME}
    COMPONENT dev
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION "${NETREMOTE_DIR_INSTALL_PUBLIC_HEADER_BASE}/${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE_SUFFIX}"
)