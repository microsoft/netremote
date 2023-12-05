
add_executable(${PROJECT_NAME}-server-windows "")

target_sources(${PROJECT_NAME}-server-windows
    PRIVATE
        Main.cxx
)

target_link_libraries(${PROJECT_NAME}-server-windows
    PRIVATE
        ${PROJECT_NAME}-server
)

set_target_properties(${PROJECT_NAME}-server-windows
    PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}-server
)

install(
    TARGETS ${PROJECT_NAME}-server-windows
    EXPORT ${PROJECT_NAME}
)

