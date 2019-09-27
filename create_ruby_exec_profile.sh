#!/usr/bin/env bash

rm -rf build
mkdir build
pushd build
cmake -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg ..
make -j8
popd
./build/ruby
gprof build/ruby > ruby_exec_profile.txt
