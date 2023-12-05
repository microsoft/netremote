# Linux Network Remote Framework

This project tree is meant to exercise and test the OS-agnostic portions of the framework.

## Development Environment Setup

As described in the main project [`README`](/README.md), a C++ 20 compiler and CMake are required. So, any distribution satisfying these requirements may be used. A known working environment is ubuntu 23.10 (mantic) with a few development packages. Instructions for setting up this environment are provided below.

### 1. Install Ubuntu 23.10 (mantic)

If development on Windows is desired, ubuntu may be installed in WSL using a rootfs image. To install WSL, on newer versions of Windows 11, use the following command: `wsl --install --no-distribution`. For complete instructions, refer to [https://learn.microsoft.com/en-us/windows/wsl/install](https://learn.microsoft.com/en-us/windows/wsl/install). Then follow these steps to install mantic on WSL:

1. Download the ubuntu 23.10 wsl rootfs archive [https://cloud-images.ubuntu.com/wsl/mantic/current/ubuntu-mantic-wsl-amd64-wsl.rootfs.tar.gz](https://cloud-images.ubuntu.com/wsl/mantic/current/ubuntu-mantic-wsl-amd64-wsl.rootfs.tar.gz).
2. Choose a location to store the WSL ubuntu filesystem image. A good way to keep organized is to use a top-level directory for all wsl filesystem images such as `c:\wsl`, then add a sub-directory for each distribution installed, such as `c:\wsl\mantic`, `c:\wsl\fedora35`, etc. It's also recommended to do this on a non-system drive if one is available for performance reasons.

3. Start an elevated command-prompt, and enter the following series of commands:

```Shell
mkdir c:\wsl\mantic
wsl --import mantic c:\wsl\mantic c:\users\myusername\Downloads\ubuntu-mantic-wsl-amd64-wsl.rootfs.tar.gz
wsl -d mantic
```

4. You are now running in a root shell on a fresh ubuntu 23.10 installation. Set up a non-root user to use by default:

```bash
adduser mycoolusername
# <accept all defaults by successively hitting 'Enter', and set a password>
usermod -G sudo mycoolusername
# install an editor if you don't like the ones pre-installed (vi and nano are available out of the box)
```

5. Create the file `/etc/wsl.conf` (need sudo), and save the following contents to it:

```ini 
[user]
default=mycoolusername
```

6. Exit ubuntu:

```bash
exit
```

7. Shutdown WSL so it can pick up the new default user changes, and re-run ubuntu:

```Shell
wsl --shutdown
wsl -d mantic
```

You should now be logged on as `mycoolusername`.

### 2. Install Linux Development Dependencies

Execute the following commands in a shell:

```bash
sudo apt update
sudo apt-get install -y build-essential ca-certificates cmake curl dotnet7 git gnupg2 linux-libc-dev ninja-build pkg-config tar unzip zip
```

Then remove the pre-installed version of llvm-16, and install llvm-17 with the automated install script:

```bash
sudo apt-get remove -y --purge clang-16* lldb-16* llvm-16*
sudo apt-get install -y --no-install-recommends lsb-release software-properties-common wget
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 17 all
rm llvm.sh
```

The above will install all the tools required to compile, lint, and debug the project. If hostapd will also be compiled ([git://w1.fi/hostap.git](git://w1.fi/hostap.git)), install the following additional development dependencies:

```bash
sudo apt-get install -y libnl-3-dev libssl-dev libnl-genl-3-dev libnl-3-dev libdbus-c++-dev libnl-route-3-dev flex bison dwarves libelf-dev bc
```

> [!NOTE]
> The source of truth for installing development dependencies is found in the development environment [Dockerfile](/.docker/netremote-dev/Dockerfile). The instructions there always trump the instructions here as the contianer described by the `Dockerfile` is used to build the official package.

### 3. Install Docker on Windows

Download and install docker from [https://docs.docker.com/desktop/install/windows-install](https://docs.docker.com/desktop/install/windows-install) ([amd64](https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe)). During the installation, ensure the `Use WSL 2 instead of Hyper-V (recommended)` box is checked.
