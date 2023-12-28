
# Requires that libmnl development package is installed (libmnl-dev on ubuntu).
find_path(LIBMNL_INCLUDE_DIR libmnl/libmnl.h REQUIRED)

find_library(LIBMNL NAMES libmnl.a REQUIRED)
add_library(mnl STATIC IMPORTED)
set_target_properties(mnl PROPERTIES IMPORTED_LOCATION ${LIBMNL_STATIC}/libmnl.a)
target_include_directories(mnl INTERFACE ${LIBMNL_INCLUDE_DIR})
