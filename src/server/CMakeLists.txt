
add_executable(wifiapctrl-server "")

target_sources(wifiapctrl-server
    PRIVATE
        Main.cxx
        WifiApControlServiceImpl.hxx
        WifiApControlServiceImpl.cxx
)

target_link_libraries(wifiapctrl-server
    PRIVATE
        wifiapctrl-protocol
)
