#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${DIR}

function corecount {
    getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu
}

source build.options

export NK_C_FLAGS="-shared -fPIC -ldl -lm -lpthread -lSDL2 -lOpenGL"
export LIB_LINK_FLAGS="-shared -fPIC -ldl -lm -lpthread -lSDL2 -lOpenGL -L. -lnk_yodaw -lnolibc_syscall"
export INTERFACE_C_FLAGS="-fPIC"
export LIB_BJOU_FLAGS="-I src"
export DRIVER_C_FLAGS="-ldl -lm -lpthread -L. -lnk_yodaw"

# Add this framework to the Mac debug build so
# that we can use Instruments.app to profile yed
if [ "$(uname)" == "Darwin" ]; then
    debug+=" -framework CoreFoundation"
fi

if [ "$#" -ge 1 ]; then
    cfg=${!1}
    bjou_cfg=""
else
    cfg=${release}
    bjou_cfg="-O"
fi

# I hate getting spammed with error messages
ERR_LIM=" -fmax-errors=3 -Wno-unused-command-line-argument"

${CC} --version ${ERR_LIM} 2>&1 > /dev/null

if [ $? -eq 0 ]; then
    cfg+=${ERR_LIM}
fi

export cfg=${cfg}
export bjou_cfg=${bjou_cfg}

make all -j$(corecount)
