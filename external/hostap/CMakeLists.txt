
# GNU make is required to build the hostap project as it does not include CMake configuration.
find_program(MAKE 
    NAMES gmake nmake make
    REQUIRED
)

# Set base installation directory for hostap build artifacts.
set(HOSTAP_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Set build prefix where hostap project source is located.
set(HOSTAP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/hostap-prefix/src/hostap CACHE INTERNAL "Path of external project code")
set(HOSTAP_LOCAL_INCLUDE_DIR ${HOSTAP_INSTALL_DIR}/usr/local/include CACHE INTERNAL "Path of external project includes")
set(HOSTAP_LOCAL_LIB_DIR ${HOSTAP_INSTALL_DIR}/usr/local/lib CACHE INTERNAL "Path of external project built libraries")
set(HOSTAP_LOCAL_BIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/bin CACHE INTERNAL "Path of external project built binaries")
set(HOSTAP_LOCAL_SBIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/sbin CACHE INTERNAL "Path of external project built system/superuser binaries")

# Set helper variables for various hostap sub-project paths.
set(WPA_SUPPLICANT_DIR wpa_supplicant)
set(WPA_SUPPLICANT_PREFIX ${HOSTAP_PREFIX}/${WPA_SUPPLICANT_DIR})
set(HOSTAPD_DIR hostapd)
set(HOSTAPD_PREFIX ${HOSTAP_PREFIX}/${HOSTAPD_DIR})

# Pull in hostap project with a tagged release.
ExternalProject_Add(hostap
    GIT_REPOSITORY "http://w1.fi/hostap.git"
    GIT_TAG "hostap_2_10"
    CONFIGURE_COMMAND
        # Copy local build configuration files for libwpa_client.so and hostapd builds.
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/libwpa_client/.config ${WPA_SUPPLICANT_PREFIX}/.config
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/hostapd/.config ${HOSTAPD_PREFIX}/.config
    BINARY_DIR ${HOSTAP_PREFIX}
    BUILD_COMMAND 
        QUIET=1 ${MAKE} -C ${WPA_SUPPLICANT_DIR} libwpa_client.so
        QUEIT=1 ${MAKE} -C ${HOSTAPD_DIR}
    INSTALL_DIR ${HOSTAP_INSTALL_DIR}
    INSTALL_COMMAND 
        COMMAND QUIET=1 DESTDIR=${HOSTAP_INSTALL_DIR} ${MAKE} -C ${WPA_SUPPLICANT_DIR} install
        COMMAND QUEIT=1 DESTDIR=${HOSTAP_INSTALL_DIR} ${MAKE} -C ${HOSTAPD_DIR} install
)

set(LIBWPA_CLIENT
    ${HOSTAP_LOCAL_LIB_DIR}/libwpa_client.so
    CACHE FILEPATH
    "wpa client shared object"
)

set(HOSTAPD_BIN
    ${HOSTAP_LOCAL_BIN_DIR}/hopstapd
    CACHE FILEPATH    
    "hostapd daemon binary"
)
