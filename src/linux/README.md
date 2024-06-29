# Linux Network Remote Framework

This project tree is meant to exercise and test the OS-agnostic portions of the framework.

## Development Environment Setup

As described in the main project [`README`](/README.md), a C++ 23 compiler and CMake are required. So, any distribution satisfying these requirements may be used. A known working environment is ubuntu 24.04 (noble numbat) with a few development packages. Instructions for setting up this environment are provided below.

### 1. Install Ubuntu 24.04 (noble)

If development on Windows is desired, ubuntu may be installed in WSL using a rootfs image. To install WSL, on newer versions of Windows 11, use the following command: `wsl --install --no-distribution`. For complete instructions, refer to [https://learn.microsoft.com/en-us/windows/wsl/install](https://learn.microsoft.com/en-us/windows/wsl/install). Then follow these steps to install noble for WSL:

1. Choose a location to store the WSL ubuntu root filesystem (rootfs) image and switch to this directory. For optimal performance, use a non-system drive and create a top-level folder for all WSL content such as `d:\wsl`:

    ```Powershell
    md d:\wsl
    cd d:\wsl
    ```

2. Download the ubuntu 24.04 wsl rootfs archive. The development environment has been tested and confirmed to work with the 20240627 image. More recent images may work as well, however, no guarantees are made for such images.

    * Stable: [https://cloud-images.ubuntu.com/wsl/noble/20240627/ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz](https://cloud-images.ubuntu.com/wsl/noble/20240627/ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz)
    * Current: [https://cloud-images.ubuntu.com/wsl/noble/current/ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz](https://cloud-images.ubuntu.com/wsl/noble/current/ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz)

    ```Powershell
    Invoke-WebRequest -Uri https://cloud-images.ubuntu.com/wsl/noble/20240627/ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz -OutFile .\ 
    ```

3. Import the downloaded rootfs image to create a new WSL distribution for noble:

    ```Powershell
    md d:\wsl\noble
    wsl --import noble d:\wsl\noble .\ubuntu-noble-wsl-amd64-24.04lts.rootfs.tar.gz
    wsl -d noble
    ```

4. You are now running in a root shell on a fresh ubuntu 24.04 installation. The rootfs image has no suitable non-root user accounts, so create one, adding them to the `sudo` and `root` groups to enable privileged execution:

    ```bash
    adduser dev --comment dev
    usermod dev -aG root,sudo
    ```

5. Create a [`/etc/wsl.conf`](https://learn.microsoft.com/en-us/windows/wsl/wsl-config#wslconf) file.

    a. Add a `[boot]` section with a `systemd=true` setting to enable systemd.
    b. Add a `[user]` section with a `default=username` setting to specify the default logon user to be the one you just created (eg. `dev`).

    ```ini
    [boot]
    systemd=true

    [user]
    default=dev
    ```

    For example, you can create the file with those contents using `echo` in execution mode:

    ```bash
    echo -e '[boot]\nsystemd=true\n\n[user]\ndefault=dev' > /etc/wsl.conf
    ```

6. Exit ubuntu:

    ```bash
    exit
    ```

7. Shutdown WSL so it can pick up the new default user changes, and re-run ubuntu:

    ```Powershell
    wsl --shutdown
    wsl -d noble 
    ```

You should now be logged on as `dev`.

### 2. Install Linux Development Dependencies

Execute the following commands in a shell to install all the tools required to compile, lint, and debug the project:

1. Update the package cache and upgrade to latest packages:

    ```bash
    sudo apt-get update -y && sudo apt-get upgrade -y
    ```

2. Install core build tools and dependencies:

    ```bash
    sudo apt-get install -y autoconf automake autopoint build-essential ca-certificates cmake curl git gnupg libltdl-dev libmount-dev libtool linux-libc-dev libstdc++-14-dev ninja-build pkg-config python3-jinja2 tar unzip zip 
    ```

3. Install LLVM 18 toolchain dependencies:

    ```bash
    sudo apt-get install -y libllvm-18-ocaml-dev libllvm18 llvm-18 llvm-18-dev llvm-18-doc llvm-18-examples llvm-18-runtime clang-18 clang-tools-18 clang-18-doc libclang-common-18-dev libclang-18-dev libclang1-18 clang-format-18 python3-clang-18 clangd-18 clang-tidy-18 libclang-rt-18-dev libpolly-18-dev  libfuzzer-18-dev lldb-18 libc++-18-dev libc++abi-18-dev libomp-18-dev libclc-18-dev libunwind-18-dev libmlir-18-dev mlir-18-tools libbolt-18-dev bolt-18 flang-18 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libc++-18-dev-wasm32 libc++abi-18-dev-wasm32 libclang-rt-18-dev-wasm32 libclang-rt-18-dev-wasm64 libllvmlibc-18-dev
    ```

4. Install other development dependencies and helpful tools:

    ```bash
    sudo apt install -y --no-install-recommends bc bison dwarves flex libelf-dev dos2unix file gnupg2 iproute2 mtools neofetch rsync ssh sudo gdb kmod nano policycoreutils-python-utils python-is-python3 vim debconf-utils iw
    ```

5. Install [hostapd](git://w1.fi/hostap.git) and wpa_supplicant development dependencies:

    ```bash
    sudo apt-get install -y libnl-3-200-dbg libnl-3-dev libssl-dev libnl-genl-3-dev libdbus-c++-dev libnl-route-3-dev
    ```

> [!NOTE]
> The source of truth for installing development dependencies is found in the GitHub [build-with-host](/.github/actions/build-with-host/action.yml) host action. The instructions there **always** supercede the instructions here as that action is used to build official release packages.

### 3. Install Docker on Windows (optional)

Download and install docker from [https://docs.docker.com/desktop/install/windows-install](https://docs.docker.com/desktop/install/windows-install) ([amd64](https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe)). During the installation, ensure the `Use WSL 2 instead of Hyper-V (recommended)` box is checked.
