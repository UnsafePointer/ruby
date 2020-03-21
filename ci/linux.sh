#!/usr/bin/env bash
set -x
set -e

rm -rf build
mkdir build
cd build
cmake ..
make -j4
