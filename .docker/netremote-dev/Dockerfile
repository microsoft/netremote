
FROM ubuntu:mantic as netremote

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

FROM netremote as netremote-dev-base

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
COPY entrypoint.sh /bin/entrypoint.sh
COPY 50-sysinfo /etc/update-motd.d/50-sysinfo

# Message of the day settings.
RUN chmod 0755 /etc/update-motd.d/50-sysinfo && \
    rm -f /etc/update-motd.d/50-landscape-sysinfo && \
    # Permission for entrypoint script
    chmod +x /bin/entrypoint.sh

FROM netremote-dev-base as netremote-build-mac80211hwsim

COPY --chmod=755 build-mac80211_hwsim-kmod.sh /tmp/build-mac80211_hwsim-kmod.sh
RUN /tmp/build-mac80211_hwsim-kmod.sh

FROM netremote-dev-base as netremote-dev

# Copy built-modules from build stage, discarding the source tree.
COPY --from=netremote-build-mac80211hwsim --chmod=755 /lib/modules/ /lib/modules/

# Set entrypoint and start interactive shell (bash).
ENTRYPOINT [ "/bin/entrypoint.sh" ]
CMD /bin/bash -i
