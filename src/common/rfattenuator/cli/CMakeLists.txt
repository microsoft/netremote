
add_executable(${PROJECT_NAME}-rfattenuator-cli-linux)

target_sources(${PROJECT_NAME}-rfattenuator-cli-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(${PROJECT_NAME}-rfattenuator-cli-linux
    PRIVATE
        ${PROJECT_NAME}-rfattenuator-linux
)

set_target_properties(${PROJECT_NAME}-rfattenuator-cli-linux
    PROPERTIES
        OUTPUT_NAME rfattenuator-cli
)
