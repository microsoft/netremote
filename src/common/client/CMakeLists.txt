
add_library(netremote-client STATIC "")

target_sources(netremote-client
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Placeholder.cxx
)

target_link_libraries(netremote-client
    PUBLIC
        netremote-protocol
)
