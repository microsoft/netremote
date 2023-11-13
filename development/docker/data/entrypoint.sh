#!/usr/bin/env bash

set -euf -o pipefail

if [ -x /usr/bin/neofetch ]; then
    /usr/bin/neofetch
fi 

exec "$@"
