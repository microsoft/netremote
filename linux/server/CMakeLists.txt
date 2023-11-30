
add_executable(netremote-server-linux "")

target_sources(netremote-server-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(netremote-server-linux
    PRIVATE
        netremote-service
)

install(
    TARGETS netremote-server-linux
    EXPORT netremote
)

