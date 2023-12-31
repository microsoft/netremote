
add_library(wpa-controller STATIC "")

set(WPA_CONTROLLER_PUBLIC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX ${WPA_CONTROLLER_PUBLIC_INCLUDE}/Wpa)

target_sources(wpa-controller
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Hostapd.cxx
        ${CMAKE_CURRENT_LIST_DIR}/ProtocolHostapd.cxx
        ${CMAKE_CURRENT_LIST_DIR}/ProtocolWpa.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaCommand.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaCommandGet.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaCommandSet.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaCommandStatus.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaController.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaCore.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaKeyValuePair.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaResponse.cxx
        ${CMAKE_CURRENT_LIST_DIR}/WpaResponseParser.cxx
    PUBLIC
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

target_include_directories(wpa-controller
    PUBLIC
        ${WPA_CONTROLLER_PUBLIC_INCLUDE}
)

target_link_libraries(wpa-controller
    PUBLIC
        libwpa-client
        notstd
    PRIVATE
        magic_enum::magic_enum
)

list(APPEND WPA_CONTROLLER_PUBLIC_HEADERS
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/Hostapd.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolWpa.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/ProtocolHostapd.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/IHostapd.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommand.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandGet.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandSet.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCommandStatus.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaController.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaCore.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaKeyValuePair.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponse.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponseGet.hxx
    ${WPA_CONTROLLER_PUBLIC_INCLUDE_PREFIX}/WpaResponseParser.hxx
)

set_target_properties(wpa-controller PROPERTIES
    PUBLIC_HEADER "${WPA_CONTROLLER_PUBLIC_HEADERS}"
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME wpa-controller
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
