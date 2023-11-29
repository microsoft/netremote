
# GNU make is required to build the hostap project as it does not include CMake build configuration.
find_program(MAKE 
    NAMES gmake nmake make
    REQUIRED
)

# Default cmake directories for external projects.
# See https://cmake.org/cmake/help/latest/module/ExternalProject.html#id3.
# TMP_DIR      = <prefix>/tmp
# STAMP_DIR    = <prefix>/src/<name>-stamp
# DOWNLOAD_DIR = <prefix>/src
# SOURCE_DIR   = <prefix>/src/<name>
# BINARY_DIR   = <prefix>/src/<name>-build
# INSTALL_DIR  = <prefix>
# LOG_DIR      = <STAMP_DIR>

# Create local variables for use within ExternalProject_Add() commands.
set(HOSTAP_EP_NAME hostap)
set(HOSTAP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${HOSTAP_EP_NAME}-prefix)
set(HOSTAP_TMP_DIR ${HOSTAP_PREFIX}/tmp)
set(HOSTAP_STAMP_DIR ${HOSTAP_PREFIX}/src/${HOSTAP_EP_NAME}-stamp)
set(HOSTAP_DOWNLOAD_DIR ${HOSTAP_PREFIX}/src)
set(HOSTAP_SOURCE_DIR ${HOSTAP_PREFIX}/src/${HOSTAP_EP_NAME})
set(HOSTAP_BINARY_DIR ${HOSTAP_PREFIX}/src/${HOSTAP_EP_NAME}-build)
set(HOSTAP_INSTALL_DIR ${HOSTAP_PREFIX})
set(HOSTAP_LOG_DIR ${HOSTAP_STAMP_DIR})

# Create local variables for referencing build artifacts.
set(HOSTAP_INSTALL_INCLUDE_DIR ${HOSTAP_INSTALL_DIR}/usr/local/include)
set(HOSTAP_INSTALL_LIB_DIR ${HOSTAP_INSTALL_DIR}/usr/local/lib)
set(HOSTAP_INSTALL_BIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/bin)
set(HOSTAP_INSTALL_SBIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/sbin)

# Create helper variables for hostap sub-project paths to be built.
set(WPA_SUPPLICANT_DIR_NAME wpa_supplicant)
set(WPA_SUPPLICANT_PREFIX ${HOSTAP_SOURCE_DIR}/${WPA_SUPPLICANT_DIR_NAME})
set(HOSTAPD_DIR_NAME hostapd)
set(HOSTAPD_PREFIX ${HOSTAP_SOURCE_DIR}/${HOSTAPD_DIR_NAME})

# Pull in hostap project with a tagged release.
ExternalProject_Add(hostap
    GIT_REPOSITORY "http://w1.fi/hostap.git"
    GIT_TAG "hostap_2_10"
    GIT_SHALLOW TRUE
    CONFIGURE_COMMAND
        # Copy local build configuration files for libwpa_client.so and hostapd builds.
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/libwpa_client/.config ${WPA_SUPPLICANT_PREFIX}/.config
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/hostapd/.config ${HOSTAPD_PREFIX}/.config
    BINARY_DIR ${HOSTAP_SOURCE_DIR}
    BUILD_COMMAND 
        COMMAND Q=1 ${MAKE} -C ${WPA_SUPPLICANT_DIR_NAME} libwpa_client.so
        COMMAND Q=1 ${MAKE} -C ${HOSTAPD_DIR_NAME}
    INSTALL_COMMAND 
        COMMAND Q=1 DESTDIR=${HOSTAP_INSTALL_DIR} ${MAKE} -C ${WPA_SUPPLICANT_DIR_NAME} install
        COMMAND Q=1 DESTDIR=${HOSTAP_INSTALL_DIR} ${MAKE} -C ${HOSTAPD_DIR_NAME} install
    INSTALL_BYPRODUCTS
        ${HOSTAP_INSTALL_BIN_DIR}/hostapd
        ${HOSTAP_INSTALL_BIN_DIR}/hostapd_cli
        ${HOSTAP_INSTALL_INCLUDE_DIR}/wpa_ctrl.h
        ${HOSTAP_INSTALL_LIB_DIR}/libwpa_client.so
        ${HOSTAP_INSTALL_SBIN_DIR}/wpa_cli
        ${HOSTAP_INSTALL_SBIN_DIR}/wpa_passphrase
        ${HOSTAP_INSTALL_SBIN_DIR}/wpa_supplicant
)

# Define an interface library for libwpa_client.so that other cmake targets can use in target_link_libraries to pull it in as a dependency.
add_library(libwpa-client INTERFACE)
target_include_directories(libwpa-client INTERFACE ${HOSTAP_INSTALL_INCLUDE_DIR})
target_link_libraries(libwpa-client INTERFACE ${HOSTAP_INSTALL_LIB_DIR}/libwpa_client.so)

add_dependencies(libwpa-client hostap)

set(LIBWPA_CLIENT
    ${HOSTAP_INSTALL_LIB_DIR}/libwpa_client.so
    CACHE FILEPATH
    "wpa client shared object"
)

set(HOSTAPD_BIN
    ${HOSTAP_INSTALL_BIN_DIR}/hostapd
    CACHE FILEPATH    
    "hostapd daemon binary"
)
