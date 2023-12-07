
vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL C:/Users/corbinphipps/source/repos/netremote
    REF 92a13dbdda126793ec1fa21c123e835e411e2749
    FETCH_REF 
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
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
