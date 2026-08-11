#!/bin/bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
cmake --build build -j$(nproc)
