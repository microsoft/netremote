
# Requires that libnl development packages are installed (libnl-3-dev, libnl-genl-3-dev, libnl-route-3-dev on ubuntu).
find_path(LIBNL_INCLUDE_DIR netlink/netlink.h
    PATHS
        /usr/include/libnl3
        /usr/local/include/libnl3
    REQUIRED
)

function(import_libnl_target target_name library_variable)
    add_library(${target_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${target_name} PROPERTIES
        IMPORTED_LOCATION ${${library_variable}}
        INTERFACE_INCLUDE_DIRECTORIES ${LIBNL_INCLUDE_DIR}
    )
endfunction()

find_library(LIBNL_LIBRARY NAMES nl-3 libnl-3 REQUIRED)
find_library(LIBNL_GENL_LIBRARY NAMES nl-genl-3 libnl-genl-3 REQUIRED)
find_library(LIBNL_ROUTE_LIBRARY NAMES nl-route-3 libnl-route-3 REQUIRED)

import_libnl_target(nl-3 LIBNL_LIBRARY)
import_libnl_target(nl-genl-3 LIBNL_GENL_LIBRARY)
import_libnl_target(nl-route-3 LIBNL_ROUTE_LIBRARY)
