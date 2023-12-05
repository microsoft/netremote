
add_executable(${PROJECT_NAME}-server-linux "")

target_sources(${PROJECT_NAME}-server-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(${PROJECT_NAME}-server-linux
    PRIVATE
        logging-utils
        ${PROJECT_NAME}-server
        notstd
        plog::plog
)

set_target_properties(${PROJECT_NAME}-server-linux
    PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}-server
)

install(
    TARGETS ${PROJECT_NAME}-server-linux
    EXPORT ${PROJECT_NAME}
)

add_subdirectory(systemd)
