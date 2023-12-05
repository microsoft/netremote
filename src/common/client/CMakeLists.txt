
add_library(${PROJECT_NAME}-client STATIC "")

target_sources(${PROJECT_NAME}-client
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Placeholder.cxx
)

target_link_libraries(${PROJECT_NAME}-client
    PUBLIC
        ${PROJECT_NAME}-protocol
)
