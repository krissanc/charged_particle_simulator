cmake_minimum_required(VERSION 3.21)
project(ChargedParticleSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags for strict warnings (disable -Werror for now to allow warnings)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Dependencies - try to find packages, but make some optional
find_package(OpenGL REQUIRED)

# Try to find GLEW, but if not found, we'll use GLAD (header-only OpenGL loader)
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    message(STATUS "GLEW not found, using GLAD instead")
    set(USE_GLAD TRUE)
endif()

# Try to find GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    # Try alternative names
    find_package(GLFW3 QUIET)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW3 QUIET glfw3)
    endif()
endif()

# GLM is header-only, so we just need the include path
# Try to find it via find_package first
find_package(glm QUIET)

# Include directories - adjust paths based on your project structure
# Assuming dependencies are in the parent directory
# Note: For <glad/gl.h>, we need the glad_new/include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/glad_new/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/GLFW)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/KHR)

# Set up dependencies list
set(DEPS OpenGL::GL)

# Add GLFW if found
if(glfw3_FOUND)
    list(APPEND DEPS glfw)
elseif(GLFW3_FOUND)
    list(APPEND DEPS ${GLFW3_LIBRARIES})
    include_directories(${GLFW3_INCLUDE_DIRS})
else()
    message(STATUS "GLFW not found via find_package, searching for local installation...")
    # Try to find GLFW in common locations
    set(GLFW_SEARCH_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/lib
        $ENV{TEMP}/glfw
        C:/vcpkg/installed/x64-windows/lib
        C:/vcpkg/installed/x86-windows/lib
    )
    
    # Try to find GLFW in temp directory (downloaded)
    if(EXISTS "$ENV{TEMP}/glfw")
        file(GLOB GLFW_TEMP_DIRS "$ENV{TEMP}/glfw/*")
        foreach(GLFW_DIR ${GLFW_TEMP_DIRS})
            if(IS_DIRECTORY ${GLFW_DIR})
                # Look for library in various subdirectories
                file(GLOB GLFW_LIBS "${GLFW_DIR}/lib*/*.lib" "${GLFW_DIR}/*/lib*/*.lib")
                if(GLFW_LIBS)
                    list(GET GLFW_LIBS 0 GLFW_LIBRARY)
                    message(STATUS "Found GLFW library in temp: ${GLFW_LIBRARY}")
                endif()
                
                # Look for include directory
                if(EXISTS "${GLFW_DIR}/include/GLFW/glfw3.h")
                    set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
                    message(STATUS "Found GLFW include in temp: ${GLFW_INCLUDE_DIR}")
                endif()
            endif()
        endforeach()
    endif()
    
    # If not found in temp, try standard find_library
    if(NOT GLFW_LIBRARY)
        find_library(GLFW_LIBRARY 
            NAMES glfw3 glfw3dll glfw
            PATHS ${GLFW_SEARCH_PATHS}
            PATH_SUFFIXES lib lib-vc2019 lib-vc2022 lib-vc2015 lib-vc2017 lib-vc2019 lib-vc2022 lib/glfw3
        )
    endif()
    
    # If not found in temp, try standard find_path
    if(NOT GLFW_INCLUDE_DIR)
        find_path(GLFW_INCLUDE_DIR
            NAMES GLFW/glfw3.h glfw3.h
            PATHS 
                ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/GLFW
                $ENV{TEMP}/glfw
                C:/vcpkg/installed/x64-windows/include
                C:/vcpkg/installed/x86-windows/include
            PATH_SUFFIXES include
        )
    endif()
    
    if(GLFW_LIBRARY AND GLFW_INCLUDE_DIR)
        message(STATUS "Found GLFW library: ${GLFW_LIBRARY}")
        message(STATUS "Found GLFW include: ${GLFW_INCLUDE_DIR}")
        list(APPEND DEPS ${GLFW_LIBRARY})
        include_directories(${GLFW_INCLUDE_DIR})
    else()
        message(WARNING "GLFW library not found. Attempting to use header-only approach...")
        # For now, we'll try to compile without linking GLFW (will fail at link time if GLFW functions are used)
        # User will need to install GLFW properly
        message(FATAL_ERROR "GLFW is required. Please install GLFW:\n  - Download from https://www.glfw.org/download.html\n  - Or use vcpkg: vcpkg install glfw3\n  - Or use package manager: winget install glfw")
    endif()
endif()

# Add GLEW if found, otherwise we'll use GLAD (which is header-only)
if(GLEW_FOUND)
    list(APPEND DEPS GLEW::GLEW)
elseif(USE_GLAD)
    message(STATUS "Using GLAD for OpenGL loading (header-only)")
    # GLAD is header-only, no library to link
endif()

# Add GLM if found via find_package
if(glm_FOUND)
    list(APPEND DEPS glm::glm)
endif()

# Source files organized by module
set(CORE_SOURCES
    engine/core/Logger.cpp
    engine/core/InputManager.cpp
)

set(PHYSICS_SOURCES
    engine/physics/Particle.cpp
    engine/physics/ElectricField.cpp
    engine/physics/FieldLineGenerator.cpp
    engine/physics/FieldLineManager.cpp
)

set(RENDER_SOURCES
    engine/render/ParticleRenderer.cpp
    engine/render/FieldLineRenderer.cpp
)

set(INTERACTION_SOURCES
    engine/interaction/Camera.cpp
    engine/interaction/RayCaster.cpp
    engine/interaction/ParticlePicker.cpp
    engine/interaction/DragController.cpp
)

set(SCENE_SOURCES
    engine/scene/ParticleSystem.cpp
)

set(MATH_SOURCES
    engine/math/Integrators.cpp
)

# GLAD implementation file (GLAD2 format from glad_new folder)
# Note: Using GLAD2 with <glad/gl.h> include format
set(GLAD_IMPL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/glad_new/src/gl.c")
if(EXISTS "${GLAD_IMPL_FILE}")
    message(STATUS "Found GLAD implementation file: ${GLAD_IMPL_FILE}")
    list(APPEND CORE_SOURCES "${GLAD_IMPL_FILE}")
else()
    message(WARNING "GLAD implementation file (gl.c) not found!")
    message(WARNING "Please generate it from https://glad.dav1d.de/ and place it in dependencies/glad_new/src/")
    message(WARNING "See GLAD_SETUP.md for detailed instructions")
endif()

set(CORE_HEADERS
    engine/core/Logger.hpp
    engine/core/Constants.hpp
    engine/core/Timer.hpp
    engine/core/InputManager.hpp
)

# Main executable
add_executable(ChargedParticleSim
    src/main.cpp
    ${CORE_SOURCES}
    ${PHYSICS_SOURCES}
    ${RENDER_SOURCES}
    ${INTERACTION_SOURCES}
    ${SCENE_SOURCES}
    ${MATH_SOURCES}
)

target_link_libraries(ChargedParticleSim PRIVATE ${DEPS})

# Get absolute paths for dependencies
get_filename_component(DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies" ABSOLUTE)
file(TO_CMAKE_PATH "${DEPS_DIR}" DEPS_DIR)

# Set include directories - use file(TO_CMAKE_PATH) to ensure proper path format
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# For <glad/gl.h> includes, we need the glad_new/include directory
file(TO_CMAKE_PATH "${DEPS_DIR}/glad_new/include" GLAD_INCLUDE_DIR)
file(TO_CMAKE_PATH "${DEPS_DIR}" DEPS_INCLUDE_DIR)
file(TO_CMAKE_PATH "${DEPS_DIR}/GLFW" GLFW_LOCAL_DIR)
file(TO_CMAKE_PATH "${DEPS_DIR}/glm" GLM_DIR)
file(TO_CMAKE_PATH "${DEPS_DIR}/KHR" KHR_DIR)

list(APPEND INCLUDE_DIRS "${GLAD_INCLUDE_DIR}" "${DEPS_INCLUDE_DIR}" "${GLFW_LOCAL_DIR}" "${GLM_DIR}" "${KHR_DIR}")

if(GLFW_INCLUDE_DIR)
    file(TO_CMAKE_PATH "${GLFW_INCLUDE_DIR}" GLFW_INCLUDE_DIR)
    list(APPEND INCLUDE_DIRS "${GLFW_INCLUDE_DIR}")
endif()

message(STATUS "Include directories: ${INCLUDE_DIRS}")

target_include_directories(ChargedParticleSim PRIVATE ${INCLUDE_DIRS})

# Shader files (copy to output dir)
file(GLOB SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
)

foreach(shader ${SHADERS})
    add_custom_command(TARGET ChargedParticleSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${shader}
        $<TARGET_FILE_DIR:ChargedParticleSim>
    )
endforeach()

# Test executable (if Catch2 is available)
# find_package(Catch2 QUIET)
# if(Catch2_FOUND)
#     add_executable(ChargedParticleSimTests
#         tests/test_coulomb.cpp
#         tests/test_field_line.cpp
#         tests/test_integrators.cpp
#     )
#     target_link_libraries(ChargedParticleSimTests PRIVATE ${DEPS} Catch2::Catch2)
#     target_include_directories(ChargedParticleSimTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# endif()

