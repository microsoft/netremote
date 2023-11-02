
add_executable(wifiremote-server "")

target_sources(wifiremote-server
    PRIVATE
        Main.cxx
        WifiRemoteService.hxx
        WifiRemoteService.cxx
)

target_link_libraries(wifiremote-server
    PRIVATE
        wifiremote-protocol
)
