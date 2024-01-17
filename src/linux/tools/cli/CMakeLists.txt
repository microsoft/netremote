
add_executable(netremote-cli-linux)

target_sources(netremote-cli-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(netremote-cli-linux
    PRIVATE
        magic_enum::magic_enum
        plog::plog
        ${PROJECT_NAME}-protocol
)

set_target_properties(netremote-cli-linux
    PROPERTIES
        OUTPUT_NAME netremote-cli
)
