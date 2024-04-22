
FROM ubuntu:mantic as netremote-build

# Set arguments used only in this Dockerfile.
ARG BUILD_DATE="2024-02-28T21:09:49+00:00"
ARG DEBIAN_FRONTEND=noninteractive
ARG APT_ARGS_COMMON="-yqq --no-install-recommends"

LABEL maintainer="Andrew Beltrano (anbeltra@microsoft.com)"
LABEL org.label-schema.build-date = "${BUILD_DATE}"
LABEL org.label-schema.name = "Microsoft NetRemote build environment"
LABEL org.label-schema.description = "Build environment for the NetRemote project"
LABEL org.label-schema.vendor = "Microsoft"
LABEL org.label-schema.version = "1.0.0"
LABEL org.label-schema.schema-version = "1.0"

#############################
# Manual installation steps #
#############################
#
# 1. Update package cache.
# sudo apt -y update && sudo apt -y upgrade
#
# 2. Install core build tools and dependencies:
# sudo apt install -y --no-install-recommends autoconf automake autopoint build-essential ca-certificates cmake curl dotnet7 git gnupg libltdl-dev libmount-dev libtool linux-libc-dev ninja-build pkg-config python3-jinja2 tar unzip zip 
#
# 3. Install complete LLVM 17 + clang 17 toolchain:
# sudo apt install -y --no-install-recommends libllvm-17-ocaml-dev libllvm17 llvm-17 llvm-17-dev llvm-17-doc llvm-17-examples llvm-17-runtime clang-17 clang-tools-17 clang-17-doc libclang-common-17-dev libclang-17-dev libclang1-17 clang-format-17 python3-clang-17 clangd-17 clang-tidy-17 libclang-rt-17-dev libpolly-17-dev libfuzzer-17-dev lldb-17 lld-17 libc++-17-dev libc++abi-17-dev libomp-17-dev libclc-17-dev libunwind-17-dev libmlir-17-dev mlir-17-tools libbolt-17-dev bolt-17 flang-17 libclang-rt-17-dev-wasm32 libclang-rt-17-dev-wasm64 libc++-17-dev-wasm32 libc++abi-17-dev-wasm32 libclang-rt-17-dev-wasm32 libclang-rt-17-dev-wasm64
# 
# 4. Install other development dependencies and helpful tools:
# sudo apt install -y --no-install-recommends bc bison dwarves flex libelf-dev dos2unix file gnupg2 iproute2 mtools neofetch rsync ssh sudo emacs gdb kmod nano policycoreutils-python-utils python-is-python3 vim
#
# 5. Install hostapd development dependencies and tools:
# sudo apt install -y libnl-3-dev libssl-dev libnl-genl-3-dev libnl-3-dev libdbus-c++-dev libnl-route-3-dev flex bison dwarves libelf-dev bc iw debconf-utils
#

# Install packages.
RUN apt-get $APT_ARGS_COMMON update && \
    apt-get $APT_ARGS_COMMON upgrade && \
    apt-get $APT_ARGS_COMMON install  \
        # Core project build dependencies.
        # autoconf automake autopoint build-essential ca-certificates cmake curl dotnet7 git gnupg libltdl-dev libmount-dev libtool linux-libc-dev ninja-build pkg-config python3-jinja2 tar unzip zip 
        autoconf \
        automake \
        autopoint \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        dotnet7 \
        git \
        gnupg \
        libltdl-dev \
        libmount-dev \
        libtool \
        linux-libc-dev \
        ninja-build \
        pkg-config \
        python3-jinja2 \
        tar \
        unzip \
        zip \
        # LLVM + Clang toolchain.
        # libllvm-17-ocaml-dev libllvm17 llvm-17 llvm-17-dev llvm-17-doc llvm-17-examples llvm-17-runtime clang-17 clang-tools-17 clang-17-doc libclang-common-17-dev libclang-17-dev libclang1-17 clang-format-17 python3-clang-17 clangd-17 clang-tidy-17 libclang-rt-17-dev libpolly-17-dev libfuzzer-17-dev lldb-17 lld-17 libc++-17-dev libc++abi-17-dev libomp-17-dev libclc-17-dev libunwind-17-dev libmlir-17-dev mlir-17-tools libbolt-17-dev bolt-17 flang-17 libclang-rt-17-dev-wasm32 libclang-rt-17-dev-wasm64 libc++-17-dev-wasm32 libc++abi-17-dev-wasm32 libclang-rt-17-dev-wasm32 libclang-rt-17-dev-wasm64
        bolt-17 \
        clang-17 \
        clang-17-doc \
        clang-format-17 \
        clang-tidy-17 \
        clang-tools-17 \
        clangd-17 \
        flang-17 \
        libbolt-17-dev \
        libc++-17-dev \
        libc++-17-dev-wasm32 \
        libc++abi-17-dev \
        libc++abi-17-dev-wasm32 \
        libclang-17-dev \
        libclang-common-17-dev \
        libclang-rt-17-dev \
        libclang-rt-17-dev-wasm32 \
        libclang-rt-17-dev-wasm32 \
        libclang-rt-17-dev-wasm64 \
        libclang-rt-17-dev-wasm64 \
        libclang1-17 \
        libclc-17-dev \
        libfuzzer-17-dev \
        libllvm-17-ocaml-dev \
        libllvm17 \
        libmlir-17-dev \
        libomp-17-dev \
        libpolly-17-dev \
        libunwind-17-dev \
        lld-17 \
        lldb-17 \
        llvm-17 \
        llvm-17-dev \
        llvm-17-doc \
        llvm-17-examples \
        llvm-17-runtime \
        mlir-17-tools \
        python3-clang-17 \
        # hostapd build dependencies.
        # libnl-3-200-dbg libnl-3-dev libssl-dev libnl-genl-3-dev
        libnl-3-200-dbg \
        libnl-3-dev \
        libnl-genl-3-dev \
        libssl-dev \
        # wpa_supplicant build dependencies.
        # libnl-3-dev libssl-dev libnl-genl-3-dev libdbus-c++-dev libnl-route-3-dev
        libdbus-c++-dev \
        libnl-route-3-dev

# Reduce image size by removing package cache
RUN apt-get -yqq clean && \
    rm -rf /var/lib/apt/lists/*

# Set environment variables for external vcpkg to support binary caching
ENV VCPKG_ROOT_BASE=/vcpkg
ENV VCPKG_BINARY_CACHE=${VCPKG_ROOT_BASE}/cache
ENV VCPKG_ROOT=${VCPKG_ROOT_BASE}/vcpkg
ENV VCPKG_DEFAULT_BINARY_CACHE=${VCPKG_BINARY_CACHE}

WORKDIR ${VCPKG_ROOT_BASE}

RUN mkdir -p ${VCPKG_BINARY_CACHE}

# Obtain vcpkg, bootstrap it, and install dependencies.
COPY vcpkg.json .

RUN git clone https://github.com/microsoft/vcpkg.git --depth 1 && \
    ./vcpkg/bootstrap-vcpkg.sh && \
    ./vcpkg/vcpkg install --clean-buildtrees-after-build --clean-downloads-after-build

# Copy build entrypoint script.
COPY --chmod=0755 entrypoint-build.sh /bin/entrypoint-build.sh

# Build the repository.
ENTRYPOINT [ "/bin/entrypoint-build.sh" ]

FROM netremote-build as netremote-dev

LABEL org.label-schema.name = "Microsoft NetRemote development environment"
LABEL org.label-schema.description = "Development environment for the NetRemote project"

# Install packages.
RUN apt-get $APT_ARGS_COMMON update && \
    apt-get $APT_ARGS_COMMON install \
        # WSL2 kernel development dependencies.
        # build-essential flex bison dwarves libssl-dev libelf-dev bc
        bc \
        bison \
        dwarves \
        flex \
        libelf-dev \
        # Generic tools.
        # dos2unix file gnupg2 iproute2 mtools neofetch rsync ssh sudo wget
        dos2unix \
        file \
        gnupg2 \
        iproute2 \
        mtools \
        neofetch \
        rsync \
        ssh \
        sudo \
        # Generic development tools.
        # emacs gdb kmod nano policycoreutils-python-utils python-is-python3 vim
        emacs \
        gdb \
        kmod \
        nano \
        policycoreutils-python-utils \
        python-is-python3 \
        vim \
        # Wireless tools.
        iw \
        # Packaging tools.
        debconf-utils \
        && \
    # Reduce image size by removing package cache.
    apt-get -yqq clean && \
    rm -rf /var/lib/apt/lists/*

# Copy outside content into container.
COPY --chmod=0755 entrypoint.sh /bin/entrypoint.sh
COPY --chmod=0755 50-sysinfo /etc/update-motd.d/50-sysinfo
COPY --chmod=0755 build-mac80211_hwsim-kmod.sh /tmp/build-mac80211_hwsim-kmod.sh

# Message of the day settings.
RUN rm -f /etc/update-motd.d/50-landscape-sysinfo

# Build the mac80211_hwsim kernel module.
RUN /tmp/build-mac80211_hwsim-kmod.sh && \
    mv /tmp/build-mac80211_hwsim-kmod.sh ${HOME}/

# Set entrypoint and start interactive shell (bash).
ENTRYPOINT [ "/bin/entrypoint.sh" ]
CMD /bin/bash -i
