
# Requires that libmnl development package is installed (libmnl-dev on ubuntu).
find_library(LIBMNL mnl libmnl REQUIRED)
find_path(LIBMNL_INCLUDE_DIR libmnl/libmnl.h REQUIRED)

add_library(mnl-static STATIC IMPORTED)
set_target_properties(mnl-static PROPERTIES IMPORTED_LOCATION ${LIBMNL})
target_include_directories(mnl-static INTERFACE ${LIBMNL_INCLUDE_DIR})

add_library(mnl-shared SHARED IMPORTED)
add_library(mnl ALIAS mnl-shared)
set_target_properties(mnl-shared PROPERTIES IMPORTED_LOCATION ${LIBMNL})
target_include_directories(mnl-shared INTERFACE ${LIBMNL_INCLUDE_DIR})
