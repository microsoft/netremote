
find_program(MAKE_EXE 
    NAMES gmake nmake make
    REQUIRED
)

set(HOSTAP_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(HOSTAP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/hostap-prefix/src/hostap/wpa_supplicant CACHE INTERNAL "Path of external project code")
set(HOSTAP_LOCAL_INCLUDE_DIR ${HOSTAP_INSTALL_DIR}/usr/local/include CACHE INTERNAL "Path of external project includes")
set(HOSTAP_LOCAL_LIB_DIR ${HOSTAP_INSTALL_DIR}/usr/local/lib CACHE INTERNAL "Path of external project built libraries")
set(HOSTAP_LOCAL_SBIN_DIR ${HOSTAP_INSTALL_DIR}/usr/local/sbin CACHE INTERNAL "Path of external project built system binaries")

ExternalProject_Add(hostap
    GIT_REPOSITORY "http://w1.fi/hostap.git"
    GIT_TAG "hostap_2_10"
    CONFIGURE_COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/.config ${HOSTAP_PREFIX}/.config
    BINARY_DIR ${HOSTAP_PREFIX}
    BUILD_COMMAND QUIET=1 ${MAKE_EXE} libwpa_client.so
    INSTALL_DIR ${HOSTAP_INSTALL_DIR}
    INSTALL_COMMAND QUIET=1 DESTDIR=${HOSTAP_INSTALL_DIR} ${MAKE_EXE} install
)

set(LIBWPA_CLIENT
    ${HOSTAP_LOCAL_LIB_DIR}/libwpa_client.so
    CACHE FILEPATH
    "wpa client shared object"
)
