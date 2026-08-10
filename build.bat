@echo off
if not exist build (
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
) else (
    cd build
)
cmake --build . --config Release -j
