
add_executable(apmonitor-cli-linux)

target_sources(apmonitor-cli-linux
    PRIVATE
        Main.cxx
)

target_link_libraries(apmonitor-cli-linux
    PRIVATE
        plog::plog
        wifi-apmanager-linux
        wifi-apmonitor
)

set_target_properties(apmonitor-cli-linux
    PROPERTIES
        OUTPUT_NAME apmonitor
)
