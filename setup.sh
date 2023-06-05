#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        apt install clangd-12
        apt install qtbase5-dev qt5-qmake
        cat config/linux_init_config.json > /home/$SUDO_USER/.diff_config.json
elif [[ "$OSTYPE" == "darwin"* ]]; then
        brew install llvm
        brew install qt@5
        cat config/mac_init_config.json  > /home/$SUDO_USER/.diff_config.json
else
        echo "Sorry, your platform isn't supported yet :("
fi
