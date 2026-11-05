@echo off
REM Скрипт установки зависимостей Conan и конфигурации CMake (Debug + Release)

echo === Установка зависимостей через Conan (Debug и Release) ===
conan install . --output-folder=build --build=missing -s build_type=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

conan install . --output-folder=build --build=missing -s build_type=Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Настройка конфигурации CMake (Debug) ===
cmake -B build/debug -S . -DCMAKE_TOOLCHAIN_FILE=build/build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Настройка конфигурации CMake (Release) ===
cmake -B build/release -S . -DCMAKE_TOOLCHAIN_FILE=build/build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Готово! ===
echo Для сборки Debug:   cmake --build build/debug
echo Для сборки Release: cmake --build build/release