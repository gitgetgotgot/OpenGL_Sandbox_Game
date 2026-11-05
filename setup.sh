#!/usr/bin/env bash
# Скрипт установки зависимостей Conan и конфигурации CMake (Debug + Release) для Linux

set -e # Остановить выполнение при любой ошибке

echo "=== Install Conan dependencies (Debug) ==="
conan install . --build=missing -s build_type=Debug

echo "=== Install Conan dependencies (Release) ==="
conan install . --build=missing -s build_type=Release

echo "=== CMake configuration (conan-default) ==="
cmake --preset conan-default

echo "=== Ready! ==="
echo "To build Debug (should run as administrator):   cmake --build --preset conan-debug"
echo "To build Release (should run as administrator): cmake --build --preset conan-release"
