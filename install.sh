#!/bin/bash
## :: @Copyright 2026
## :: Author    : Lucas Cruz
## :: Title     : MySupervisor HPC Collector (MSHPCC)
## :: Version   : 0.0.1
## :: Maintainer: Lucas Cruz
## :: Email     : lucruz@posgrad.lncc.br
## :: Type      : Research
##
##
## :: Message from maintainer : 
##      Please, if you have any questions about the implementation, don't send it to me!
##      Just God knowns what I've tried to do in here. I don't remember anything anymore.
##      ...
##      I'm just kidding! Besides, just God knowns, you can send me your questions!

set -euo pipefail

VERSION="0.0.1"

# cd "$(dirname "${BASH_SOURCE[0]}")/.."

# step
# runstep=${RUN_STEP:-help}

# function threat_option() {
#     for arg in "$@"; do
#         echo $arg
#     done
# }

option=${1:-0}
# threat_option $@

case "$option" in
help)
    echo ":: @Copyright 2026"
    echo ":: Author    : Lucas Cruz"
    echo ":: Title     : MySupervisor HPC Collector (MSHPCC)"
    echo ":: Version   : ${VERSION}"
    echo ":: Maintainer: Lucas Cruz"
    echo ":: Email     : lucruz@posgrad.lncc.br"
    echo ":: Type      : Research"
    echo ""
    echo "Usage: ./install.sh [help|install]"
    echo ""
    echo "This script install MySupervisor HPC Collector (MSHPCC)."
    echo ""
    echo "options:"
    echo -e "  help\t\tshow this help message and exit"
    echo -e "  install\tinstall the MSHPCC. By default it will be installed on ~/.local/bin"
    ;;
install)
    echo "Installing MSHPCC"

    INSTALL_DIR="${HOME}/.opt/apps/mshpcc/${VERSION}"
    WRAPPER_SCRIPT="${HOME}/.local/bin/mshpcc"

    echo "Creating a new directory on: ${INSTALL_DIR}"
    mkdir -p "${INSTALL_DIR}"

    echo "Copying files \"main.py\" and \"collector.py\" into ${INSTALL_DIR}"
    cp main.py collector.py "${INSTALL_DIR}/"

    echo "Creating wrapper script \"mshpcc\" on ${WRAPPER_SCRIPT}"
    
    cat > "${WRAPPER_SCRIPT}" <<EOF
#!/bin/bash

python3 ${INSTALL_DIR}/main.py --collector=${INSTALL_DIR}/collector.py -- "\$@"
EOF
    chmod +x "${WRAPPER_SCRIPT}"

    echo ""
    echo "MSHPCC ${VERSION} was installed successfully!"
    echo "Installation directory: ${INSTALL_DIR}"
    echo "Wrapper script: ${WRAPPER_SCRIPT}"
    echo ""
    echo "Make sure ~/.local/bin is in your PATH."
    ;;
*) echo "Too see all options to install, please run: ./install.sh help" >&2; exit 2 ;;
esac
