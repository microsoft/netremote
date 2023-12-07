
FROM ubuntu:mantic as netremote-build

# Set arguments used only in this Dockerfile.
ARG build_date="2023-11-09T19:51:18Z"
ARG DEBIAN_FRONTEND=noninteractive
ARG LLVM_VERSION=17

LABEL maintainer="Andrew Beltrano (anbeltra@microsoft.com)"
LABEL org.label-schema.build-date = "${build_date}"
LABEL org.label-schema.name = "Microsoft NetRemote build environment"
LABEL org.label-schema.description = "Build environment for development of the NetRemote project"
LABEL org.label-schema.vendor = "Microsoft"
LABEL org.label-schema.version = "1.0.0"
LABEL org.label-schema.schema-version = "1.0"

#############################
# Manual installation steps #
#############################
#
# 1. Update package cache.
# sudo apt-get update
#
# 2. Install core build tools:
# sudo apt-get install -y --no-install-recommends build-essential ca-certificates cmake curl dotnet7 git gnupg linux-libc-dev ninja-build pkg-config tar unzip zip
#
# 3. Remove llvm 16 toolchain packages to avoid conflicts with llvm 17 toolchain.
# sudo apt-get remove -y --purge clang-16* lldb-16* llvm-16*
# 
# 4. Install complete llvm toolchain using automatic installation script:
# sudo apt-get install -y --no-install-recommends lsb-release software-properties-common wget
# wget https://apt.llvm.org/llvm.sh
# chmod +x llvm.sh
# sudo ./llvm.sh 17 all
# rm llvm.sh
#
# 5. Install development dependency packages:
# sudo apt-get install -y --no-install-recommends libnl-3-dev libnl-genl-3-dev libssl-dev libdbus-c++-dev libnl-route-3-dev bc bison dwarves flex libelf-dev dos2unix file gnupg2 iproute2 mtools neofetch rsync ssh sudo emacs gdb kmod nano policycoreutils-python-utils python-is-python3 vim
#

# Install packages.
RUN apt-get update && \
    apt-get install -qq -y --no-install-recommends \
        # Project build dependencies.
        # build-essential ca-certificates cmake curl dotnet7 git gnupg linux-libc-dev ninja-build pkg-config tar unzip zip
        build-essential \
        ca-certificates \
        cmake \
        curl \
        dotnet7 \
        git \
        gnupg \
        linux-libc-dev \
        ninja-build \
        pkg-config \
        tar \
        unzip \
        zip

# Install complete llvm toolchain using automatic installation script.
# lsb-release, software-properties-common, and wget are required by the script.
RUN apt-get install -qq -y --no-install-recommends \
        lsb-release \
        software-properties-common \
        wget && \
    # Remove llvm 16 toolchain packages to avoid conflicts with llvm 17 toolchain.
    apt-get remove -y --purge clang-16* lldb-16* llvm-16* && \
    # Use automatic installation script to install llvm 17 toolchain.
    wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh ${LLVM_VERSION} all && \
    # Delete the script.
    rm llvm.sh

# Reduce image size by removing package cache
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Set environment variables for external vcpkg to support binary caching
ENV VCPKG_ROOT_BASE=/
ENV VCPKG_BINARY_CACHE=${VCPKG_ROOT_BASE}cache
ENV VCPKG_ROOT=${VCPKG_ROOT_BASE}vcpkg
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

ENV VCPKG_INSTALL_OPTIONS=--debug

# Build the repository.
ENTRYPOINT [ "/bin/entrypoint-build.sh" ]

FROM netremote-build as netremote-dev

# Install packages.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        # hostapd build dependencies.
        # libnl-3-dev libssl-dev libnl-genl-3-dev
        libnl-3-dev \
        libnl-genl-3-dev \
        libssl-dev \
        # wpa_supplicant build dependencies.
        # libnl-3-dev libssl-dev libnl-genl-3-dev libdbus-c++-dev libnl-route-3-dev
        libdbus-c++-dev \
        libnl-route-3-dev \
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
        && \
    # Reduce image size by removing package cache.
    apt-get clean && \
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
