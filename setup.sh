#!/usr/bin/env bash
# Скрипт установки зависимостей Conan и конфигурации CMake (Debug + Release)

set -e

echo "=== Установка зависимостей через Conan (Debug и Release) ==="
conan install . --output-folder=build --build=missing -s build_type=Debug
conan install . --output-folder=build --build=missing -s build_type=Release

echo "=== Настройка конфигурации CMake (Debug) ==="
cmake -B build/debug -S . -DCMAKE_TOOLCHAIN_FILE=build/build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

echo "=== Настройка конфигурации CMake (Release) ==="
cmake -B build/release -S . -DCMAKE_TOOLCHAIN_FILE=build/build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

echo "=== Готово! ==="
echo "Для сборки Debug:   cmake --build build/debug"
echo "Для сборки Release: cmake --build build/release"