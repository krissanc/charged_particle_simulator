@echo off
REM Build script for creating a distributable release package
REM This script builds the project in Release mode and packages it for distribution

echo ========================================
echo Charged Particle Simulator - Release Build
echo ========================================
echo.

REM Navigate to project directory
cd /d "%~dp0"
set PROJECT_DIR=%CD%
set BUILD_DIR=%PROJECT_DIR%\build
set RELEASE_DIR=%PROJECT_DIR%\release
set DIST_DIR=%RELEASE_DIR%\ChargedParticleSim_v1.0

echo [1/6] Cleaning previous release...
if exist "%RELEASE_DIR%" (
    rmdir /s /q "%RELEASE_DIR%"
)
mkdir "%RELEASE_DIR%"
mkdir "%DIST_DIR%"

echo [2/6] Building project in Release mode...
cd "%BUILD_DIR%"

REM Configure for Release build if not already configured
if not exist "CMakeCache.txt" (
    echo Configuring CMake...
    where cl >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
        if %ERRORLEVEL% NEQ 0 (
            cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
        )
    ) else (
        cmake .. -DCMAKE_BUILD_TYPE=Release
    )
)

REM Build in Release configuration
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo [3/6] Locating executable...
set EXE_PATH=
if exist "%BUILD_DIR%\Release\ChargedParticleSim.exe" (
    set EXE_PATH=%BUILD_DIR%\Release\ChargedParticleSim.exe
) else if exist "%BUILD_DIR%\ChargedParticleSim.exe" (
    set EXE_PATH=%BUILD_DIR%\ChargedParticleSim.exe
) else (
    echo ERROR: Executable not found!
    echo Please check the build directory: %BUILD_DIR%
    pause
    exit /b 1
)

echo Found executable: %EXE_PATH%

echo.
echo [4/6] Copying executable and required files...
copy "%EXE_PATH%" "%DIST_DIR%\ChargedParticleSim.exe" >nul

REM Copy shader files if they exist
if exist "%PROJECT_DIR%\shaders" (
    echo Copying shader files...
    xcopy /E /I /Y "%PROJECT_DIR%\shaders" "%DIST_DIR%\shaders" >nul
)

REM Copy any config files
if exist "%PROJECT_DIR%\*.ini" (
    copy "%PROJECT_DIR%\*.ini" "%DIST_DIR%\" >nul
)
if exist "%PROJECT_DIR%\*.config" (
    copy "%PROJECT_DIR%\*.config" "%DIST_DIR%\" >nul
)

echo.
echo [5/6] Locating and copying required DLLs...

REM Find GLFW DLL
set GLFW_DLL_FOUND=0

REM Check common GLFW locations
set GLFW_PATHS[0]=%BUILD_DIR%\Release\glfw3.dll
set GLFW_PATHS[1]=%BUILD_DIR%\glfw3.dll
set GLFW_PATHS[2]=C:\vcpkg\installed\x64-windows\bin\glfw3.dll
set GLFW_PATHS[3]=%PROJECT_DIR%\..\dependencies\lib\glfw3.dll
set GLFW_PATHS[4]=%PROJECT_DIR%\..\dependencies\GLFW\lib-vc2022\glfw3.dll
set GLFW_PATHS[5]=%PROJECT_DIR%\..\dependencies\GLFW\lib-vc2019\glfw3.dll

for /L %%i in (0,1,5) do (
    call set "GLFW_PATH=%%GLFW_PATHS[%%i]%%"
    if exist "!GLFW_PATH!" (
        echo Found GLFW DLL: !GLFW_PATH!
        copy "!GLFW_PATH!" "%DIST_DIR%\glfw3.dll" >nul
        set GLFW_DLL_FOUND=1
        goto :glfw_found
    )
)

:glfw_found
if %GLFW_DLL_FOUND% EQU 0 (
    echo.
    echo WARNING: GLFW DLL (glfw3.dll) not found automatically!
    echo The application may not run without it.
    echo.
    echo Please manually copy glfw3.dll to: %DIST_DIR%
    echo.
    echo Common locations:
    echo   - C:\vcpkg\installed\x64-windows\bin\glfw3.dll
    echo   - Your GLFW installation bin directory
    echo.
    pause
)

REM Check for Visual C++ Redistributable dependencies
echo.
echo [6/6] Creating README and package info...

REM Create README for distribution
(
echo Charged Particle Simulator v1.0
echo =================================
echo.
echo SYSTEM REQUIREMENTS:
echo   - Windows 10 or later
echo   - OpenGL 4.3+ compatible graphics card
echo   - Visual C++ Redistributable 2015-2022 ^(usually pre-installed^)
echo.
echo INSTALLATION:
echo   1. Extract all files to a folder
echo   2. Run ChargedParticleSim.exe
echo.
echo CONTROLS:
echo   - Left Mouse Drag: Orbit camera around scene
echo   - Left Click + Drag on Particle: Drag particle
echo   - Scroll Wheel: Zoom in/out
echo   - R Key: Reset camera to default position
echo   - SPACE: Pause/resume simulation
echo   - ESC: Exit application
echo.
echo TROUBLESHOOTING:
echo   If the application doesn't start:
echo   1. Make sure glfw3.dll is in the same folder as the .exe
echo   2. Install Visual C++ Redistributable from Microsoft
echo   3. Update your graphics drivers
echo   4. Check that your graphics card supports OpenGL 4.3+
echo.
echo For more information, visit: [Your Website URL]
) > "%DIST_DIR%\README.txt"

echo.
echo ========================================
echo Release package created successfully!
echo ========================================
echo.
echo Package location: %DIST_DIR%
echo.
echo Next steps:
echo   1. Test the executable in the release folder
echo   2. Create a ZIP file: %RELEASE_DIR%\ChargedParticleSim_v1.0.zip
echo   3. Upload to your website for download
echo.
echo To create ZIP file, you can use:
echo   7z a "%RELEASE_DIR%\ChargedParticleSim_v1.0.zip" "%DIST_DIR%\*"
echo   OR
echo   Right-click the folder and select "Send to ^> Compressed ^(zipped^) folder"
echo.
pause

