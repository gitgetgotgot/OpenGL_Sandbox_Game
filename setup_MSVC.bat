@echo off
chcp 65001 > nul
REM Script for CMake configuration and Conan dependencies for Windows & Visual Studio (Debug + Release)

echo === Install Conan dependencies (Debug) ===
conan install . --build=missing -s build_type=Debug
if %errorlevel% neq 0 goto error

echo === Install Conan dependencies (Release) ===
conan install . --build=missing -s build_type=Release
if %errorlevel% neq 0 goto error

echo === CMake configuration (conan-default) using Clang-cl ===
cmake --preset conan-default
if %errorlevel% neq 0 goto error

echo === Configuring Ready ===
echo To build Debug (should run as administrator):   cmake --build --preset conan-debug
echo To build Release (should run as administrator): cmake --build --preset conan-release
goto end

:error
echo.
echo [!] Configuration Error!
pause
exit /b %errorlevel%

:end
echo.
pause