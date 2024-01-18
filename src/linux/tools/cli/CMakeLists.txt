
add_executable(${PROJECT_NAME}-cli-linux)

target_sources(${PROJECT_NAME}-cli-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(${PROJECT_NAME}-cli-linux
    PRIVATE
        magic_enum::magic_enum
        plog::plog
        ${PROJECT_NAME}-cli
        ${PROJECT_NAME}-client
)

set_target_properties(${PROJECT_NAME}-cli-linux
    PROPERTIES
        OUTPUT_NAME netremote-cli
)

install(
    TARGETS ${PROJECT_NAME}-cli-linux
    EXPORT ${PROJECT_NAME}
)
