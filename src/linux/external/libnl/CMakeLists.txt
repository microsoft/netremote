
# Requires that libnl development package is installed (libnl-3-dev on ubuntu).
find_path(LIBNL_INCLUDE_DIR netlink/netlink.h 
    PATHS 
        /usr/include/libnl3 
        /usr/local/include/libnl3
    REQUIRED
)

find_library(LIBNL_STATIC NAMES libnl-3.a REQUIRED)
add_library(nl STATIC IMPORTED GLOBAL)
set_target_properties(nl PROPERTIES IMPORTED_LOCATION ${LIBNL_STATIC}/libnl-3.a)
set_target_properties(nl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBNL_INCLUDE_DIR})
target_include_directories(nl INTERFACE ${LIBNL_INCLUDE_DIR})
