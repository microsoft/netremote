
# Pull in external dependencies.

# Global, common dependencies.
find_package(Threads REQUIRED)

if (NOT NETREMOTE_EXCLUDE_SERVER)
    set(NEEDS_CLI11 TRUE)
    set(NEEDS_PLOG TRUE)
    set(NEEDS_NLOHMANN_JSON TRUE)
    set(NEEDS_MAGIC_ENUM TRUE)
    set(NEEDS_PROTOBUF TRUE)
    set(NEEDS_GRPC TRUE)
endif()

if (NOT NETREMOTE_EXCLUDE_API)
    set(NEEDS_PROTOBUF TRUE)
endif()

if (NOT NETREMOTE_EXCLUDE_API_BINDINGS)
    set(NEEDS_PROTOBUF TRUE)
    set(NEEDS_GRPC TRUE)
endif()

if (NOT NETREMOTE_EXCLUDE_TESTS)
    set(NEEDS_GRPC TRUE)
    set(NEEDS_PLOG TRUE)
    set(NEEDS_NLOHMANN_JSON TRUE)
    set(NEEDS_CATCH2 TRUE)
endif()

if (NEEDS_CLI11)
    include(cli11)
endif()

if (NEEDS_PLOG)
    include(plog)
endif()

if (NEEDS_NLOHMANN_JSON)
    include(nlohmann-json)
endif()

if (NEEDS_MAGIC_ENUM)
    include(magic-enum)
endif()

if (NEEDS_PROTOBUF)
    include(protoc)
endif()

if (NEEDS_GRPC)
    include(grpc)
endif()

if (NEEDS_CATCH2)
    include(catch2)
endif()
