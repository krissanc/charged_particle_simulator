# GLAD Setup Instructions

GLAD requires both a header file (`glad.h`) and an implementation file (`glad.c`). The `glad.h` file exists but `glad.c` is missing.

## Current Situation

**Files Present:**
- ✅ `dependencies/glad/glad.h` - OpenGL 3.3, Compatibility profile (generated Jan 7, 2023)
- ✅ `dependencies/KHR/khrplatform.h` - Present and valid
- ❌ `dependencies/glad/glad.c` - **MISSING** (this is what we need)

**Code Requirements:**
- `main.cpp` requests: OpenGL 4.3, Core profile (lines 71-73)

**Version Mismatch:**
- Existing `glad.h` is for OpenGL 3.3 Compatibility
- Code requests OpenGL 4.3 Core
- **Important**: `glad.h` and `glad.c` must be generated together and match exactly

## Recommendation

**Option 2 (Full Update)** is recommended because:
1. Your code already requests OpenGL 4.3 Core
2. Core profile is more modern and removes deprecated features
3. Ensures consistency between GLAD files and your code

You have two options:

## Option 1: Quick Fix - Match Existing glad.h (Not Recommended)

Generate `glad.c` to match your existing `glad.h`:

1. **Go to the GLAD Generator**: https://glad.dav1d.de/

2. **Configure GLAD** (to match existing glad.h):
   - **Language**: C/C++
   - **Specification**: OpenGL
   - **API gl**: Version **3.3** (matches existing glad.h)
   - **Profile**: **Compatibility** (matches existing glad.h)
   - **Extensions**: Leave default
   - **Generator**: C/C++

3. **Generate and Download**:
   - Click "Generate"
   - Download the generated ZIP file
   - Extract the ZIP

4. **Copy Files**:
   - Copy `glad.c` to: `charged_particle_simulator/dependencies/glad/glad.c`
   - **Note**: Keep your existing `glad.h` (don't replace it)

5. **Update main.cpp** (temporary):
   - Change OpenGL version request from 4.3 to 3.3 in `main.cpp` line 71-72:
     ```cpp
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     ```

## Option 2: Full Update - Regenerate Both Files for OpenGL 4.3 (Recommended)

This matches what your code already requests:

1. **Go to the GLAD Generator**: https://glad.dav1d.de/

2. **Configure GLAD**:
   - **Language**: C/C++
   - **Specification**: OpenGL
   - **API gl**: Version **4.3** (matches your code)
   - **Profile**: **Core** (matches your code)
   - **Extensions**: Leave default or add any you need
   - **Generator**: C/C++

3. **Generate and Download**:
   - Click "Generate"
   - Download the generated ZIP file
   - Extract the ZIP

4. **Copy Files**:
   - Copy `glad.c` to: `charged_particle_simulator/dependencies/glad/glad.c`
   - **Replace** `glad.h` in: `charged_particle_simulator/dependencies/glad/glad.h`
   - **Note**: `khrplatform.h` already exists in `dependencies/KHR/`, so you can skip copying it unless the generated one is newer

## Verification

After adding `glad.c`, rebuild the project:
```powershell
cd charged_particle_simulator\charged_particle_sim\build_new
cmake --build . --config Release
```

The linker errors for `gladLoadGLLoader` and other GLAD functions should be resolved.

## Note

The CMakeLists.txt is already configured to automatically include `glad.c` when it's found in the dependencies folder.

