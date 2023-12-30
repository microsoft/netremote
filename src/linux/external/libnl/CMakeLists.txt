
# Requires that libnl development package is installed (libnl-3-dev on ubuntu).
find_path(LIBNL_INCLUDE_DIR netlink/netlink.h 
    PATHS 
        /usr/include/libnl3 
        /usr/local/include/libnl3
    REQUIRED
)

# libnl core
find_library(LIBNL_STATIC NAMES libnl-3.a REQUIRED)
add_library(nl STATIC IMPORTED GLOBAL)
set_target_properties(nl PROPERTIES IMPORTED_LOCATION ${LIBNL_STATIC})
set_target_properties(nl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBNL_INCLUDE_DIR})
target_include_directories(nl INTERFACE ${LIBNL_INCLUDE_DIR})

# libnl-genl
find_library(LIBNL_GENL_STATIC NAMES libnl-genl-3.a REQUIRED)
add_library(nl-genl STATIC IMPORTED GLOBAL)
set_target_properties(nl-genl PROPERTIES IMPORTED_LOCATION ${LIBNL_GENL_STATIC})
set_target_properties(nl-genl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBNL_INCLUDE_DIR})
target_include_directories(nl-genl INTERFACE ${LIBNL_INCLUDE_DIR})

# libnl-route
find_library(LIBNL_ROUTE_STATIC NAMES libnl-route-3.a REQUIRED)
add_library(nl-route STATIC IMPORTED GLOBAL)
set_target_properties(nl-route PROPERTIES IMPORTED_LOCATION ${LIBNL_ROUTE_STATIC})
set_target_properties(nl-route PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBNL_INCLUDE_DIR})
target_include_directories(nl-route INTERFACE ${LIBNL_INCLUDE_DIR})
