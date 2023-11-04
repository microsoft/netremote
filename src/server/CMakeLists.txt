
add_executable(netremote-server "")

target_sources(netremote-server
    PRIVATE
        Main.cxx
)

target_link_libraries(netremote-server
    PRIVATE
        netremote-service
)

install(
    TARGETS netremote-server
    EXPORT netremote
)
