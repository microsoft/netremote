
add_executable(wifiremote-server "")

target_sources(wifiremote-server
    PRIVATE
        Main.cxx
        WifiRemoteServiceImpl.hxx
        WifiRemoteServiceImpl.cxx
)

target_link_libraries(wifiremote-server
    PRIVATE
        wifiremote-protocol
)
