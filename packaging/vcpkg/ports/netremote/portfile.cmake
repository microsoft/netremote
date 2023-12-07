
vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL C:/src/microsoft/netremote
    REF 799874f17d1f05ace49e019d2d8389154be18965
    FETCH_REF user/corbinphipps/add-vcpkg-port
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DNETREMOTE_DISABLE_TESTS
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE 
    "${CURRENT_PACKAGES_DIR}/bin"
    "${CURRENT_PACKAGES_DIR}/debug/bin"
    "${CURRENT_PACKAGES_DIR}/debug/include"
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
