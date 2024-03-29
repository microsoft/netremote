
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
        WpaCommandSet.cxx
        WpaCommandStatus.cxx
        WpaController.cxx
        WpaCore.cxx
        WpaKeyValuePair.cxx
        WpaResponse.cxx
        WpaResponseParser.cxx
        WpaParsingUtilities.hxx
        WpaParsingUtilities.cxx
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${WPA_CONTROLLER_PUBLIC_INCLUDE}
    FILES
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/Hostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/IHostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolHostapd.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolWpa.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommand.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandGet.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandSet.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandStatus.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaController.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCore.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaKeyValuePair.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponse.hxx
        ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponseParser.hxx
)

target_link_libraries(wpa-controller
    PUBLIC
        libwpa-client
        notstd
    PRIVATE
        magic_enum::magic_enum
        plog::plog
)
