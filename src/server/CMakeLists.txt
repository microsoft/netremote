
add_executable(wifiremote-server "")

target_sources(wifiremote-server
    PRIVATE
        Main.cxx
)

target_link_libraries(wifiremote-server
    PRIVATE
        wifiremote-service
)

install(
    TARGETS wifiremote-server
    EXPORT wifiremote
)
