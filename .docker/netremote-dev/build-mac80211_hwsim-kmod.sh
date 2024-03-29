#!/bin/bash
#
# Compiles the mac80211_hwsim driver as a module for the WSL2 kernel.
#
# Inspired by:
# - https://gist.github.com/charlie-x/96a92aaaa04346bdf1fb4c3621f3e392
# - https://github.com/microsoft/WSL2-Linux-Kernel#build-instructions
#

set -eu -o pipefail

# Determine active kernel version number.
KERNEL_VERSION=`uname -r | grep -Eo '([0-9]+)*(\.?[0-9]+)*' | head -1`

# URL for kernel sources of active kernel version.
KERNEL_FILE_NAME_PREFIX=linux-msft-wsl-
KERNEL_FILE_NAME_STEM=${KERNEL_FILE_NAME_PREFIX}${KERNEL_VERSION}
KERNEL_FILE_NAME_SUFFIX=.tar.gz
KERNEL_FILE_NAME=${KERNEL_FILE_NAME_STEM}${KERNEL_FILE_NAME_SUFFIX}
KERNEL_URL_BASE=https://github.com/microsoft/WSL2-Linux-Kernel/archive/refs/tags
KERNEL_URL_FILE=${KERNEL_URL_BASE}/${KERNEL_FILE_NAME}

# WSL kernel source directory.
WSL_SRC_DIRECTORY_BASE_DEFAULT=${HOME}/src
WSL_SRC_DIRECTORY_BASE=${WSL_SRC_DIRECTORY_BASE:=${WSL_SRC_DIRECTORY_BASE_DEFAULT}}
WSL_SRC_DIRECTORY_PREFIX=WSL2-Linux-Kernel-
WSL_SRC_DIRECTORY_NAME=${WSL_SRC_DIRECTORY_PREFIX}${KERNEL_FILE_NAME_STEM}
WSL_SRC_DIRECTORY=${WSL_SRC_DIRECTORY_BASE}/${WSL_SRC_DIRECTORY_NAME}

# WSL kernel configuration file.
WSL_SRC_CONFIG_FILE_NAME=config-${KERNEL_VERSION}
WSL_SRC_CONFIG_DIRECTORY=Microsoft
WSL_SRC_CONFIG=${WSL_SRC_CONFIG_DIRECTORY}/${WSL_SRC_CONFIG_FILE_NAME}

# WSL kernel compilation arguments.
KERNEL_COMPILE_ARG_PARALLEL="-j $(expr $(nproc) - 1)"
KERNEL_CONFIG_UTIL=${WSL_SRC_DIRECTORY}/scripts/config

# Additional arguments passed to wget when downloading kernel source.
# Mostly used for testing.
WGET_XTRA_ARGS=

# Install dependencies for building kernel modules.
# (see https://github.com/microsoft/WSL2-Linux-Kernel)
# added bc, python-is-python3
echo "Installing dependencies for building kernel modules..."
sudo apt-get update
sudo apt-get install -y build-essential bc bison flex dwarves kmod libssl-dev libelf-dev python-is-python3 wget

# Download and unpack kernel source.
echo "Downloading and unpacking kernel source..."
if [[ ! -d ${WSL_SRC_DIRECTORY_BASE} ]]; then
    mkdir -p ${WSL_SRC_DIRECTORY_BASE}
fi

if [[ ! -d ${WSL_SRC_DIRECTORY} ]]; then
    cd ${WSL_SRC_DIRECTORY_BASE}
    wget ${KERNEL_URL_FILE} ${WGET_XTRA_ARGS:+"${WGET_XTRA_ARGS}"} -O - | tar xzf -
else
    echo "Kernel source directory already exists, cleaning it"
    cd ${WSL_SRC_DIRECTORY}
    make -C ${WSL_SRC_DIRECTORY} mrproper
fi

cd ${WSL_SRC_DIRECTORY}

# Prepare the kernel source with the configuration for the running kernel.
echo "Preparing kernel source with configuration for running kernel..."
cat /proc/config.gz | gzip -d > .config

# Ensure kernel helper scripts are available.
if [[ ! -f ${KERNEL_CONFIG_UTIL} ]]; then
    make scripts
fi

# Update the configuration to build the mac80211_hwsim module and its dependencies.
echo "Updating kernel configuration to build mac80211_hwsim module and its dependencies..."
${KERNEL_CONFIG_UTIL} \
    --enable CONFIG_WLAN \
    --module CONFIG_RFKILL \
    --module CONFIG_CFG80211 \
    --module CONFIG_MAC80211 \
    --module CONFIG_MAC80211_HWSIM

# Supply defaults for any new/unspecified options in the configuration.
make olddefconfig

echo "Preparing kernel source tree for building external modules..."
make prepare modules_prepare ${KERNEL_COMPILE_ARG_PARALLEL}

echo "Building kernel modules..."
make modules ${KERNEL_COMPILE_ARG_PARALLEL}

echo "Installing kernel modules..."
sudo make modules_install

echo "Done."
