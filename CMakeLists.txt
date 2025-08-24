cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

# Levikno
project(Levikno)

option(LVN_BUILD_EXAMPLES "Build example programs" TRUE)
option(LVN_INCLUDE_GLSLANG "include glslang libraries and shader source compile support" TRUE)


# output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Architecture
set(LVN_ARCHITEXTURE_TYPE x64)

# Platform
if (WIN32)
    add_definitions(-DLVN_PLATFORM_WINDOWS)

elseif(APPLE)
    add_definitions(-DLVN_PLATFORM_MACOS)

elseif(UNIX AND NOT APPLE)
    add_definitions(-DLVN_PLATFORM_LINUX)
else()
    add_definitions(-DLVN_PLATFORM_UNKNOWN)
endif()

# build shared
if (BUILD_SHARED_LIBS)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()


# library
add_library(levikno STATIC)

target_sources(levikno 
    PRIVATE
        include/levikno/levikno.h
        src/levikno.cpp
        src/lvn_cds.cpp
)

target_include_directories(levikno
    PRIVATE
        src
        include/levikno
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
