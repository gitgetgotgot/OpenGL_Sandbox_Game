#!/usr/bin/env bash
# Script for CMake configuration and Conan dependencies for Linux (Debug + Release)

set -e

echo "=== Install Conan dependencies (Debug) ==="
conan install . --build=missing -s build_type=Debug

echo "=== Install Conan dependencies (Release) ==="
conan install . --build=missing -s build_type=Release

echo "=== CMake configuration (conan-debug) ==="
cmake --preset conan-debug
echo "=== CMake configuration (conan-release) ==="
cmake --preset conan-release

echo "=== Configuring Ready ==="
echo "To build Debug (should run as administrator):   cmake --build --preset conan-debug"
echo "To build Release (should run as administrator): cmake --build --preset conan-release"
