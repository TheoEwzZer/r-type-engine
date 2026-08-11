#!/bin/bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build -j$(nproc)
