#!/bin/bash

mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -S ../ -B .
make && make Shaders && ./vkGameEngine
cd ..
