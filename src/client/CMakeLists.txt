
add_library(wifiremote-client STATIC "")

target_sources(wifiremote-client
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Placeholder.cxx
)

target_link_libraries(wifiremote-client
    PUBLIC
        netremote-protocol
)
