#!/usr/bin/bash

set -euo pipefail

# Uncomment the below lines to help debug script execution.
set -x
readonly DEBCONF_DEBUG=developer
export DEBCONF_DEBUG

# Include helper libraries.
. /usr/share/debconf/confmodule

# shellcheck source=./confnetdiscovery
. confnetdiscovery

# Main script execution entrypoint.
#
# Arguments:
#   Must be those passed to the script (ie. "$@")
#
function main() {
    db_x_loadtemplatefile ./templates
    network_service_discovery_configure_interactive
}

main "$@"
