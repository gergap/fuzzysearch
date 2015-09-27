#!/bin/bash

OPTIONS="-DCMAKE_INSTALL_PREFIX=$PWD -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=on"
BLD_DIR="bld"

rm -rf "$BLD_DIR"
mkdir "$BLD_DIR"
cd "$BLD_DIR"
cmake $OPTIONS ..
make
make install
make test

