# Make public version of wil available.
set(WIL_BUILD_TESTS OFF CACHE INTERNAL "Turn off wil tests")
set(WIL_BUILD_PACKAGING OFF CACHE INTERNAL "Turn off wil packaging")

find_package(wil CONFIG REQUIRED)

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

if (NETREMOTE_FEATURE_SERVER)
    add_subdirectory(server)
endif()
