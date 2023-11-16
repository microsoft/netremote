
find_program(MAKE_EXE 
    NAMES gmake nmake make
    REQUIRED
)

set(HOSTAP_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(HOSTAP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/hostap-prefix/src/hostap CACHE INTERNAL "Path of external project code")
set(HOSTAP_LOCAL_INCLUDE_DIR ${HOSTAP_INSTALL_DIR}/usr/local/include CACHE INTERNAL "Path of external project includes")
set(HOSTAP_LOCAL_LIB_DIR ${HOSTAP_INSTALL_DIR}/usr/local/lib CACHE INTERNAL "Path of external project built libraries")
set(HOSTAP_LOCAL_SBIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/sbin CACHE INTERNAL "Path of external project built system binaries")

set(WPA_SUPPLICANT_DIR wpa_supplicant)
set(WPA_SUPPLICANT_PREFIX ${HOSTAP_PREFIX}/${WPA_SUPPLICANT_DIR})
set(HOSTAPD_DIR hostapd)
set(HOSTAPD_PREFIX ${HOSTAP_PREFIX}/${HOSTAPD_DIR})

ExternalProject_Add(hostap
    GIT_REPOSITORY "http://w1.fi/hostap.git"
    GIT_TAG "hostap_2_10"
    CONFIGURE_COMMAND 
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/libwpa_client/.config ${WPA_SUPPLICANT_PREFIX}/.config
        COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/hostapd/.config ${HOSTAPD_PREFIX}/.config
    BINARY_DIR ${HOSTAP_PREFIX}
    BUILD_COMMAND 
        COMMAND QUIET=1 $(MAKE) -C ${WPA_SUPPLICANT_DIR} libwpa_client.so
        COMMAND QUEIT=1 $(MAKE) -C ${HOSTAPD_DIR}
    INSTALL_DIR ${HOSTAP_INSTALL_DIR}
    INSTALL_COMMAND 
        COMMAND QUIET=1 DESTDIR=${HOSTAP_INSTALL_DIR} $(MAKE) -C ${WPA_SUPPLICANT_DIR} install
        COMMAND QUIET=1 DESTDIR=${HOSTAP_INSTALL_DIR} $(MAKE) -C ${HOSTAPD} install
)

set(LIBWPA_CLIENT
    ${HOSTAP_LOCAL_LIB_DIR}/libwpa_client.so
    CACHE FILEPATH
    "wpa client shared object"
)
