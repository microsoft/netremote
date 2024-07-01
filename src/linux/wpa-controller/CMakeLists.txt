
add_library(wpa-controller STATIC "")

set(WPA_CONTROLLER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WPA_CONTROLLER_PUBLIC_INCLUDE_SUFFIX Wpa)
set(WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX ${WPA_CONTROLLER_PUBLIC_INCLUDE}/${WPA_CONTROLLER_PUBLIC_INCLUDE_SUFFIX})

target_sources(wpa-controller
    PRIVATE
        Hostapd.cxx
        HostapdException.cxx
        ProtocolHostapd.cxx
        ProtocolWpa.cxx
        WpaCommand.cxx
        WpaCommandGet.cxx
        WpaCommandGetConfig.cxx
        WpaCommandSet.cxx
        WpaCommandStatus.cxx
        WpaController.cxx
        WpaControlSocket.cxx
        WpaControlSocketConnection.cxx
        WpaKeyValuePair.cxx
        WpaParsingUtilities.cxx
        WpaParsingUtilities.hxx
        WpaResponse.cxx
        WpaResponseParser.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WPA_CONTROLLER_PUBLIC_INCLUDE}
    FILES
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/Hostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/IHostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolHostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolWpa.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolWpaConfig.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommand.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandGet.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandGetConfig.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandSet.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandStatus.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaController.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaControlSocket.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaControlSocketConnection.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCore.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaKeyValuePair.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponse.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponseParser.hxx
)

target_link_libraries(wpa-controller
    PUBLIC
        libwpa-client
        notstd
        wifi-core
    PRIVATE
        magic_enum::magic_enum
        plog::plog
        strings
)

# Add WPA control path path base for use in ProtocolWpaConfig.hxx.
target_compile_definitions(wpa-controller
    PUBLIC
        CONFIG_WPA_CONTROL_SOCKET_PATH_BASE="${CMAKE_INSTALL_FULL_RUNSTATEDIR}"
)
