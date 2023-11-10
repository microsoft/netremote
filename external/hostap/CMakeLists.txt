
find_program(MAKE_EXE 
    NAMES gmake nmake make
    REQUIRED
)

set(HOSTAP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/hostap-prefix/src/hostap/wpa_supplicant)

ExternalProject_Add(hostap
    GIT_REPOSITORY "http://w1.fi/hostap.git"
    GIT_TAG "hostap_2_10"
    CONFIGURE_COMMAND cp -n ${CMAKE_CURRENT_SOURCE_DIR}/.config ${HOSTAP_PREFIX}/.config
    BINARY_DIR ${HOSTAP_PREFIX}
    BUILD_COMMAND QUIET=1 ${MAKE_EXE} libwpa_client.so
    INSTALL_DIR ${HOSTAP_PREFIX}
    INSTALL_COMMAND QUIET=1 DESTDIR=${HOSTAP_PREFIX} ${MAKE_EXE} install
)

set(LIBWPA_CLIENT
    ${HOSTAP_PREFIX}/libwpa_client.so
    CACHE FILEPATH
    "wpa client shared object"
)
