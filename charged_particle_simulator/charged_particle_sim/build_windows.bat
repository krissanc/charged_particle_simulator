@echo off
REM Build script for Charged Particle Simulator on Windows
REM This script sets up the build environment and compiles the project

echo ========================================
echo Charged Particle Simulator Build Script
echo ========================================
echo.

REM Check if CMake is available
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake is not found in PATH
    echo Please install CMake from https://cmake.org/download/
    echo Or add CMake to your PATH
    pause
    exit /b 1
)

echo [1/4] Checking CMake version...
cmake --version
echo.

REM Navigate to project directory
cd /d "%~dp0"
set PROJECT_DIR=%CD%
set BUILD_DIR=%PROJECT_DIR%\build

echo [2/4] Project directory: %PROJECT_DIR%
echo [2/4] Build directory: %BUILD_DIR%
echo.

REM Check if GLFW is available
echo [3/4] Checking for GLFW...
set GLFW_FOUND=0

REM Check for vcpkg GLFW
if exist "C:\vcpkg\installed\x64-windows\include\GLFW\glfw3.h" (
    echo Found GLFW via vcpkg
    set GLFW_FOUND=1
)

REM Check for local GLFW headers (but need library)
if exist "%PROJECT_DIR%\..\dependencies\GLFW\glfw3.h" (
    echo Found GLFW headers in dependencies folder
    echo WARNING: GLFW library files (.lib/.dll) may be missing for Windows
    echo.
    echo If build fails, you need to:
    echo   1. Download GLFW from https://www.glfw.org/download.html
    echo   2. Extract to dependencies\GLFW\ (or use vcpkg: vcpkg install glfw3)
    echo   3. Ensure .lib and .dll files are in the appropriate subdirectories
    echo.
)

if %GLFW_FOUND% EQU 0 (
    echo.
    echo GLFW library files not found. You have two options:
    echo.
    echo Option 1 - Using vcpkg (Recommended):
    echo   1. Install vcpkg: git clone https://github.com/Microsoft/vcpkg.git
    echo   2. Run: .\vcpkg\bootstrap-vcpkg.bat
    echo   3. Run: .\vcpkg\vcpkg install glfw3:x64-windows
    echo   4. Configure CMake with: -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
    echo.
    echo Option 2 - Manual Download:
    echo   1. Download GLFW 3.x from https://www.glfw.org/download.html
    echo   2. Extract to dependencies\GLFW\
    echo   3. Ensure lib-vc2019 or lib-vc2022 folder contains glfw3.lib
    echo   4. Ensure the DLL is available at runtime
    echo.
    pause
)

REM Create build directory
if not exist "%BUILD_DIR%" (
    echo [4/4] Creating build directory...
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

echo.
echo [4/4] Configuring CMake...
echo.

REM Configure CMake
REM Try to detect Visual Studio generator
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using Visual Studio generator...
    cmake .. -G "Visual Studio 17 2022" -A x64
    if %ERRORLEVEL% NEQ 0 (
        cmake .. -G "Visual Studio 16 2019" -A x64
    )
) else (
    echo Using default generator...
    cmake ..
)

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo.
    echo Common issues:
    echo   - GLFW not found: Install via vcpkg or download manually
    echo   - OpenGL not found: Install graphics drivers
    echo   - Compiler not found: Install Visual Studio Build Tools
    echo.
    pause
    exit /b 1
)

echo.
echo [5/5] Building project...
echo.

REM Build the project
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location:
if exist "Release\ChargedParticleSim.exe" (
    echo   Release\ChargedParticleSim.exe
) else if exist "ChargedParticleSim.exe" (
    echo   ChargedParticleSim.exe
) else (
    echo   Check build directory for executable
)
echo.
echo To run the simulation, execute the .exe file from the build directory
echo Make sure GLFW DLL is in the same directory or in your PATH
echo.
pause

