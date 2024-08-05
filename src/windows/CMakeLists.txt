
# Add Windows-specific global compile definitions.
add_compile_definitions(
    _UNICODE
    UNICODE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

# Allow more than default number of sections in object files.
add_compile_options(
    /bigobj
)

if (NOT NETREMOTE_EXCLUDE_SERVER)
    add_subdirectory(server)
endif()
