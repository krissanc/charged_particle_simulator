# Building Charged Particle Simulator on Windows

This guide will help you build and run the Charged Particle Simulator on Windows.

## Prerequisites

1. **CMake 3.21 or higher**
   - Download from: https://cmake.org/download/
   - During installation, select "Add CMake to system PATH"

2. **C++ Compiler**
   - **Visual Studio 2019 or 2022** (recommended)
     - Download Community Edition: https://visualstudio.microsoft.com/
     - Install "Desktop development with C++" workload
   - OR **MinGW-w64** (alternative)
     - Download from: https://www.mingw-w64.org/

3. **OpenGL 4.3+ compatible graphics card**
   - Most modern graphics cards support this
   - Update your graphics drivers

## Dependencies Setup

The project requires GLFW for window management. You have two options:

### Option 1: Using vcpkg (Recommended)

vcpkg is Microsoft's C++ package manager and makes dependency management easy.

1. **Install vcpkg:**
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Install GLFW:**
   ```powershell
   .\vcpkg install glfw3:x64-windows
   ```

3. **Note the vcpkg path** - you'll need it for CMake configuration

### Option 2: Manual GLFW Installation

1. **Download GLFW:**
   - Go to: https://www.glfw.org/download.html
   - Download "Windows pre-compiled binaries" (64-bit)

2. **Extract GLFW:**
   - Extract to: `charged_particle_simulator/dependencies/GLFW/`
   - Your structure should look like:
     ```
     dependencies/
       GLFW/
         include/
           GLFW/
             glfw3.h
         lib-vc2019/  (or lib-vc2022)
           glfw3.lib
         glfw3.dll
     ```

## Building the Project

### Method 1: Using the Build Script (Easiest)

1. Open Command Prompt or PowerShell in the project directory
2. Run:
   ```batch
   build_windows.bat
   ```

The script will:
- Check for CMake
- Verify dependencies
- Configure and build the project

### Method 2: Manual CMake Build

1. **Open Command Prompt** in the project directory:
   ```batch
   cd charged_particle_simulator\charged_particle_sim
   ```

2. **Create build directory:**
   ```batch
   mkdir build
   cd build
   ```

3. **Configure CMake:**

   **If using vcpkg:**
   ```batch
   cmake .. -G "Visual Studio 17 2022" -A x64 ^
            -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]\scripts\buildsystems\vcpkg.cmake
   ```

   **If using manual GLFW:**
   ```batch
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

   **For MinGW:**
   ```batch
   cmake .. -G "MinGW Makefiles"
   ```

4. **Build the project:**
   ```batch
   cmake --build . --config Release
   ```

   Or open the generated `.sln` file in Visual Studio and build from there.

## Running the Simulation

After building, the executable will be in:
- `build/Release/ChargedParticleSim.exe` (Visual Studio)
- `build/ChargedParticleSim.exe` (MinGW)

**Important:** If you manually installed GLFW, make sure `glfw3.dll` is:
- In the same directory as the executable, OR
- In your system PATH

### Controls

- **Left Mouse Drag**: Orbit camera around scene
- **Left Click + Drag on Particle**: Drag particle and observe field line updates
- **Scroll Wheel**: Zoom in/out
- **R Key**: Reset camera to default position
- **SPACE**: Pause/resume simulation
- **ESC**: Exit application

## Troubleshooting

### CMake can't find GLFW

**Solution 1:** Use vcpkg with toolchain file:
```batch
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-path]\scripts\buildsystems\vcpkg.cmake
```

**Solution 2:** Manually specify GLFW paths:
```batch
cmake .. -DGLFW_LIBRARY="path\to\glfw3.lib" ^
         -DGLFW_INCLUDE_DIR="path\to\GLFW\include"
```

### OpenGL not found

- Update your graphics drivers
- Ensure you have a compatible graphics card
- On some systems, you may need to install OpenGL libraries

### Linker errors

- Ensure GLFW library files (.lib) are in the correct location
- Check that you're using the correct architecture (x64 vs x86)
- Verify Visual Studio version matches the GLFW library version

### Runtime errors (DLL not found)

- Copy `glfw3.dll` to the same directory as the executable
- Or add the GLFW bin directory to your PATH

### Build errors about missing headers

- Verify all dependencies are in `dependencies/` folder:
  - `dependencies/glad/glad.h`
  - `dependencies/GLFW/glfw3.h`
  - `dependencies/glm/glm.hpp`
  - `dependencies/KHR/khrplatform.h`

## Development Setup

For development, you can use:

- **Visual Studio Code** with C++ extension
- **Visual Studio** (full IDE)
- **CLion** (JetBrains)

Make sure to configure your IDE to use the CMake build system.

## Next Steps

Once the simulation is running:
- Check `logs/simulation.log` for detailed runtime information
- Experiment with different particle configurations in `main.cpp`
- Modify physics parameters in `engine/core/Constants.hpp`

