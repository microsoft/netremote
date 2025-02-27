
add_library(${PROJECT_NAME}-rfattenuator-linux STATIC "")

set(NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources(${PROJECT_NAME}-rfattenuator-linux
    PRIVATE
        RfAttenuatorExceptionImpl.cxx
        RfAttenuatorFactoryImpl.cxx
        StringHelpers.cxx
        RfAttenuatorSoftwareSimulated.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE}/RfAttenuator.hxx
        ${NET_REMOTE_RFATTENUATOR_LINUX_PUBLIC_INCLUDE}/RfAttenuatorFactory.hxx
)
