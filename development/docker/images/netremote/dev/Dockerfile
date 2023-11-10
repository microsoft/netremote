
# abeltrano/netremote-dev
FROM abeltrano/netremote

# Set arguments used only in this Dockerfile.
ARG uid=1001
ARG gid=${uid}
ARG username=build
ARG build_date="2023-11-09T19:51:18Z"
ARG DEBIAN_FRONTEND=noninteractive

LABEL maintainer="Andrew Beltrano (anbeltra@microsoft.com)"
LABEL org.label-schema.build-date = "${build_date}"
LABEL org.label-schema.name = "Microsoft NetRemote build environment"
LABEL org.label-schema.description = "Build environment for development of the NetRemote project"
LABEL org.label-schema.vendor = "Microsoft"
LABEL org.label-schema.version = "1.0.0"
LABEL org.label-schema.schema-version = "1.0"

# Set persistent runtime environment variables for the container.
ENV USER=${username}

# Install packages.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        # Generic tools.
        # curl dos2unix file iproute2 mtools neofetch rsync ssh sudo tar unzip wget
        curl \
        dos2unix \
        file \
        iproute2 \
        mtools \
        neofetch \
        rsync \
        ssh \
        sudo \
        tar \
        unzip \
        wget \
        # Generic development tools.
        # clang-format clang-tidy emacs gdb nano policycoreutils-python-utils python-is-python3 vim
        clang-format \
        clang-tidy \
        emacs \
        gdb \
        nano \
        policycoreutils-python-utils \
        python-is-python3 \
        vim \
        && \
    # Reduce image size by removing package cache
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy outside content into container
COPY entrypoint.sh /bin/entrypoint.sh
COPY 50-sysinfo /etc/update-motd.d/50-sysinfo

# Configure non-root user and permissions
RUN groupadd -g ${gid} ${username} && \
    useradd -m -s /bin/bash -u ${uid} -g ${gid} ${username} -G sudo,root && \
    # Disable sudo password prompt for non-root user
    echo ${username} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${username} && \
    chmod 0440 /etc/sudoers.d/${username} && \
    # Message of the day settings
    chmod 0755 /etc/update-motd.d/50-sysinfo && \
    rm -f /etc/update-motd.d/50-landscape-sysinfo && \
    # Make NetRemote source directory and assign non-root ownership
    install -d /netremote -o ${uid} -g ${gid} -m 0700 && \
    # Permission for entrypoint script
    chmod +x /bin/entrypoint.sh

# Set the active user.
USER ${username}

# Ensure Linux-style line endings are used
RUN git config --global core.autocrlf input && \
    git config --global credential.helper 'cache --timeout=36000' && \
    ln -s /netremote /home/${username}/netremote

# Set entrypoint and start interactive shell (bash).
ENTRYPOINT [ "/bin/entrypoint.sh" ]
WORKDIR /home/${USER}
CMD /bin/bash -i
