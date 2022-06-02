#!/bin/zsh

g++ -Wall -Wextra --std=c++17 -Iinclude -o raytracing src/main.cpp &&
./raytracing > output/output.ppm &&
imgcat output.ppm
