#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        apt install clangd-12
        apt install qtbase5-dev qt5-qmake
elif [[ "$OSTYPE" == "darwin"* ]]; then
        brew install llvm
        brew install qt@5
else
        echo "Sorry, your platform isn't supported yet :("
fi
