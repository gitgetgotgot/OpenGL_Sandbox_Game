#!/usr/bin/env bash
# Скрипт установки зависимостей Conan и конфигурации CMake (Debug + Release)

set -e

echo "=== Установка зависимостей через Conan (Debug) ==="
conan install . --output-folder=build/debug --build=missing -s build_type=Debug -c tools.system.package_manager:mode=check

echo "=== Установка зависимостей через Conan (Release) ==="
conan install . --output-folder=build/release --build=missing -s build_type=Release -c tools.system.package_manager:mode=check

echo "=== Настройка конфигурации CMake (Debug) ==="
cmake -B build/debug -S . -DCMAKE_TOOLCHAIN_FILE=build/debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

echo "=== Настройка конфигурации CMake (Release) ==="
cmake -B build/release -S . -DCMAKE_TOOLCHAIN_FILE=build/release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

echo "=== Готово! ==="
echo "Для сборки Debug:   cmake --build build/debug"
echo "Для сборки Release: cmake --build build/release"
