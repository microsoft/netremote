
add_library(${PROJECT_NAME}-net-linux STATIC "")

set(NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE_SUFFIX microsoft/net)
set(NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE_PREFIX ${NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE}/${NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE_SUFFIX})

target_sources(${PROJECT_NAME}-net-linux
    PRIVATE
        NetworkOperationsLinux.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE}
    FILES
        ${NET_REMOTE_NET_LINUX_PUBLIC_INCLUDE_PREFIX}/NetworkOperationsLinux.hxx
)

target_link_libraries(${PROJECT_NAME}-net-linux
    PUBLIC
        ${PROJECT_NAME}-net
        plog::plog
    PRIVATE
        libnl-helpers
)
