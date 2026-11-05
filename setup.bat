@echo off
chcp 65001 > nul

echo === Install Conan dependencies (Debug) ===
conan install . --build=missing -s build_type=Debug
if %errorlevel% neq 0 goto error

echo === Install Conan dependencies (Release) ===
conan install . --build=missing -s build_type=Release
if %errorlevel% neq 0 goto error

echo === CMake configuration (conan-default) ===
cmake --preset conan-default
if %errorlevel% neq 0 goto error

echo === Ready! ===
echo To build Debug:   cmake --build --preset conan-debug
echo To build Release: cmake --build --preset conan-release
goto end

:error
echo.
echo [!] Ошибка настройки!
pause
exit /b %errorlevel%

:end
echo.
pause