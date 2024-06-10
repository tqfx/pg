#!/usr/bin/env bash
lib=$(dirname $(realpath $0))
if [ -z $1 ]; then
    exit 0
fi
cd $1 && rsync -av --delete \
    -f'+ cmake/**.cmake' \
    -f'+ include/*make*' \
    -f'+ include/a/*.*' \
    -f'+ src/**.[ch]' \
    -f'+ *.txt' \
    -f'+ *.lua' \
    -f'+ *.md' \
    -f'- *.*' \
    cmake \
    include \
    src \
    CMakeLists.txt \
    LICENSE.txt \
    README.md \
    xmake.lua \
    $lib/liba
