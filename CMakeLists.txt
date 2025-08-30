cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

# levikno
project(Levikno)

option(LVN_BUILD_EXAMPLES "Build example programs" TRUE)
option(LVN_INCLUDE_GLSLANG "include glslang libraries and shader source compile support" TRUE)

# platform
if (WIN32)
    add_definitions(-DLVN_PLATFORM_WINDOWS)

elseif(APPLE)
    add_definitions(-DLVN_PLATFORM_MACOS)

elseif(UNIX AND NOT APPLE)
    add_definitions(-DLVN_PLATFORM_LINUX)
else()
    add_definitions(-DLVN_PLATFORM_UNKNOWN)
endif()

# output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# build shared
if (BUILD_SHARED_LIBS)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

set(LVN_SOURCES
    include/levikno/levikno.h
    include/levikno/api/lvn_glfw.h

    src/levikno.cpp
    src/lvn_cds.cpp
    src/lvn_internal.h
    src/lvn_window.cpp
    src/lvn_window_internal.h

    # glad
    src/external/glad/include/glad/glad.h
    src/external/glad/src/glad.c

    #glfw
    src/api/window/glfw/lvn_glfw_impl.h
    src/api/window/glfw/lvn_glfw_impl.cpp
)

# Subdirectories

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(src/external/glfw)

list(APPEND LVN_SOURCES $<TARGET_OBJECTS:glfw>)
include_directories(BEFORE SYSTEM src/external/glfw/include)

# library
add_library(levikno STATIC)

target_sources(levikno 
    PRIVATE
        ${LVN_SOURCES}
)

target_include_directories(levikno
    PRIVATE
        include/levikno
        include/levikno/api
        src
        src/external/glfw/include
        src/external/glad/include
        src/api/window/glfw
)

# Build definitions
target_compile_definitions(levikno PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# Build examples
if(LVN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
