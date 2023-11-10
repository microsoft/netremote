
# abeltrano/netremote-dev-hostapd
FROM abeltrano/netremote-dev

# Install packages.
RUN sudo apt-get update && \
    sudo apt-get install -y -qq --no-install-recommends \
        # hostapd build dependencies.
        # libnl-3-dev libssl-dev libnl-genl-3-dev
        libnl-3-dev \
        libssl-dev \
        libnl-genl-3-dev \
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
        && \
# Reduce image size by removing package cache
sudo apt-get clean && sudo rm -rf /var/lib/apt/lists/*
