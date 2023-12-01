
add_executable(netremote-server-windows "")

target_sources(netremote-server-windows
    PRIVATE
        Main.cxx
)

target_link_libraries(netremote-server-windows
    PRIVATE
        netremote-server
)

set_target_properties(netremote-server-windows
    PROPERTIES
        OUTPUT_NAME netremote-server
)

install(
    TARGETS netremote-server-windows
    EXPORT netremote
)

