
vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL /workspaces/netremote
    REF f906b2293822e200008e7e51c574d5dc006bc141
    FETCH_REF user/corbinphipps/add-vcpkg-port
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE 
    "${CURRENT_PACKAGES_DIR}/bin"
    "${CURRENT_PACKAGES_DIR}/debug/bin"
    "${CURRENT_PACKAGES_DIR}/debug/include"
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
