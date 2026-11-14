@echo off
chcp 65001 > nul
setlocal

REM Automatically get path to Visual Studio 2026 and its Ninja
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\18\Community"
if not exist "%VS_PATH%" set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community"

REM Get MSVC environment (cl.exe)
call "%VS_PATH%\Common7\Tools\VsDevCmd.bat" -arch=amd64 > nul

REM Set path to VS Ninja in current session PATH
set "PATH=%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

echo === Install Conan dependencies & Configure (Debug) ===
conan install . -pr:h=./conan_profiles/debug_MSVC -pr:b=./conan_profiles/debug_MSVC --build=missing
if %errorlevel% neq 0 goto error
cmake --preset conan-debug
if %errorlevel% neq 0 goto error

echo === Install Conan dependencies & Configure (Release) ===
conan install . -pr:h=./conan_profiles/release_MSVC -pr:b=./conan_profiles/release_MSVC --build=missing
if %errorlevel% neq 0 goto error
cmake --preset conan-release
if %errorlevel% neq 0 goto error

echo === Configuring Ready ===
echo To build Debug(Administrator):   cmake --build --preset conan-debug
echo To build Release(Administrator): cmake --build --preset conan-release
goto end

:error
echo.
echo [!] Configuration Error!
pause
exit /b %errorlevel%

:end
echo.
pause