
# TODO: consider a specific version of ubuntu, like mantic-20231011.
# abeltrano/netremote-dev
FROM ubuntu:mantic

# Install packages.
RUN apt-get update && \
# TODO: add -qq flags to 'apt-get install' to reduce output once container is stable.
    apt-get install -y --no-install-recommends \
        # Project build dependencies.
        # build-essential clang dotnet7 git gnupg lldb llvm ninja-build pkg-config
        build-essential \
        clang \
        dotnet7 \
        git \
        gnupg \
        lldb \
        llvm \
        ninja-build \
        pkg-config \
        && \
# Reduce image size by removing package cache
apt-get clean && rm -rf /var/lib/apt/lists/*
