
add_executable(netremote-server-linux "")

target_sources(netremote-server-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(netremote-server-linux
    PRIVATE
        logging-utils
        netremote-server
        notstd
        plog::plog
)

set_target_properties(netremote-server-linux
    PROPERTIES
        OUTPUT_NAME netremote-server
)

install(
    TARGETS netremote-server-linux
    EXPORT netremote
)

