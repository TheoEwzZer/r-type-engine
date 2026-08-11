#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
else
    cd build
fi

cmake --build . -j$(nproc)
