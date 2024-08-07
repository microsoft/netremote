
FROM ubuntu:noble as netremote-build

# Set arguments used only in this Dockerfile.
ARG BUILD_DATE="2024-06-30T02:17:53+00:00"
ARG DEBIAN_FRONTEND=noninteractive
ARG APT_ARGS_COMMON="-yqq --no-install-recommends"

LABEL org.opencontainers.image.authors = "netremoteowners@microsoft.com"
LABEL org.label-schema.build-date = "${BUILD_DATE}"
LABEL org.label-schema.name = "NetRemote build environment"
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
# sudo apt install -y --no-install-recommends autoconf automake autopoint build-essential ca-certificates cmake curl dotnet7 git gnupg libltdl-dev libmount-dev libtool linux-libc-dev libstdc++-14-dev ninja-build pkg-config python3-jinja2 tar unzip zip 
#
# 3. Install complete LLVM 18 + clang toolchain:
# sudo apt install -y --no-install-recommends libllvm-18-ocaml-dev libllvm18 llvm-18 llvm-18-dev llvm-18-doc llvm-18-examples llvm-18-runtime clang-18 clang-tools-18 clang-18-doc libclang-common-18-dev libclang-18-dev libclang1-18 clang-format-18 python3-clang-18 clangd-18 clang-tidy-18 libclang-rt-18-dev libpolly-18-dev libfuzzer-18-dev lldb-18 libc++-18-dev libc++abi-18-dev libomp-18-dev libclc-18-dev libunwind-18-dev libmlir-18-dev mlir-18-tools libbolt-18-dev bolt-18 flang-18 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libc++-18-dev-wasm32 libc++abi-18-dev-wasm32 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libllvmlibc-18-dev
# 
# 4. Install other development dependencies and helpful tools:
# sudo apt install -y -qq --no-install-recommends bc bison dwarves flex libelf-dev dos2unix file gnupg2 iproute2 mtools neofetch rsync ssh sudo emacs gdb kmod nano policycoreutils-python-utils python-is-python3 vim
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
        git \
        gnupg \
        libltdl-dev \
        libmount-dev \
        libtool \
        linux-libc-dev \
        libstdc++-14-dev \
        ninja-build \
        pkg-config \
        python3-jinja2 \
        tar \
        unzip \
        zip \
        # LLVM + Clang 18 toolchain.
        # libllvm-18-ocaml-dev libllvm18 llvm-18 llvm-18-dev llvm-18-doc llvm-18-examples llvm-18-runtime clang-18 clang-tools-18 clang-18-doc libclang-common-18-dev libclang-18-dev libclang1-18 clang-format-18 python3-clang-18 clangd-18 clang-tidy-18 libclang-rt-18-dev libpolly-18-dev  libfuzzer-18-dev lldb-18 libc++-18-dev libc++abi-18-dev libomp-18-dev libclc-18-dev libunwind-18-dev libmlir-18-dev mlir-18-tools libbolt-18-dev bolt-18 flang-18 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libc++-18-dev-wasm32 libc++abi-18-dev-wasm32 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libllvmlibc-18-dev
        libllvm-18-ocaml-dev \
        libllvm18 \
        llvm-18 \
        llvm-18-dev \
        llvm-18-doc \
        llvm-18-examples \
        llvm-18-runtime \
        clang-18 \
        clang-tools-18 \
        clang-18-doc \
        libclang-common-18-dev \
        libclang-18-dev \
        libclang1-18 \
        clang-format-18 \
        python3-clang-18 \
        clangd-18 \
        clang-tidy-18 \
        libclang-rt-18-dev \
        libpolly-18-dev \
        libfuzzer-18-dev \
        lldb-18 \
        libc++-18-dev \
        libc++abi-18-dev \
        libomp-18-dev \
        libclc-18-dev \
        libunwind-18-dev \
        libmlir-18-dev \
        mlir-18-tools \
        libbolt-18-dev \
        bolt-18 \
        flang-18 \
        libclang-rt-18-dev-wasm32 \
        libclang-rt-18-dev-wasm64 \
        libc++-18-dev-wasm32 \
        libc++abi-18-dev-wasm32 \
        libclang-rt-18-dev-wasm32 \
        libclang-rt-18-dev-wasm64 \
        libllvmlibc-18-dev \
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

RUN git clone https://github.com/microsoft/vcpkg.git && \
    ./vcpkg/bootstrap-vcpkg.sh && \
    ./vcpkg/vcpkg install --clean-buildtrees-after-build --clean-downloads-after-build

# Copy build entrypoint script.
COPY --chmod=0755 entrypoint-build.sh /bin/entrypoint-build.sh

# Build the repository.
ENTRYPOINT [ "/bin/entrypoint-build.sh" ]

FROM netremote-build as netremote-dev

LABEL org.label-schema.name = "NetRemote development environment"
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
        # iw
        iw \
        # Packaging tools.
        # debconf-utils
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
