#!/usr/bin/env bash

set -euf -o pipefail

SSH_HOME=~/.ssh

if [ -d /tmp/.ssh ]; then
    if [ ! -d ~/.ssh ]; then
        echo "Configuring host ssh keys"
        install -d -m 700 -o ${USER} -g ${USER} ~/
        cp -r /tmp/.ssh ~/.ssh/
        if [ -f ${SSH_HOME}/id_rsa.pub ]; then
            chmod 644 ${SSH_HOME}/id_rsa.pub
        fi
        if [ -f ${SSH_HOME}/id_rsa ]; then
            chmod 600 ${SSH_HOME}/id_rsa
        fi
    else
        echo "Host ssh keys already configured"
    fi
else 
    echo "No host ssh keys found, skipping configuration"
fi

if [ -f /tmp/.gitconfig ]; then
    install -m 644 -o ${USER} -g ${USER} /tmp/.gitconfig ~/
fi

if [ -x /usr/bin/neofetch ]; then
    /usr/bin/neofetch
fi 

exec "$@"
