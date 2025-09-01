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


set(LVN_SRC_CORE
    include/levikno/levikno.h
    include/levikno/lvn_ecs.h

    src/levikno.cpp
    src/levikno_internal.h
    src/lvn_cds.cpp
    src/lvn_ecs.cpp
)

set(LVN_SRC_WINDOW
    include/levikno/api/lvn_glfw.h
    include/levikno/lvn_window.h
    src/lvn_window.cpp
    src/lvn_window_internal.h

    #glfw
    src/api/window/glfw/lvn_glfw_impl.h
    src/api/window/glfw/lvn_glfw_impl.cpp
)

set(LVN_SRC_GRAPHICS
    include/levikno/lvn_graphics.h
    src/lvn_graphics.cpp
    src/lvn_graphics_internal.h

    # glad
    src/external/glad/src/glad.c
    src/external/glad/include/glad/glad.h

    # vulkan
    src/external/vk_mem_alloc.h
    src/api/graphics/vulkan/lvn_vulkan.cpp
    src/api/graphics/vulkan/lvn_vulkan.h

    # stb
    src/external/stb_image.h
    src/external/stb_image_write.h
    src/external/stb_impl.cpp
)

# Subdirectories

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(src/external/glfw)

list(APPEND LVN_SRC_WINDOW $<TARGET_OBJECTS:glfw>)
include_directories(BEFORE SYSTEM src/external/glfw/include)

# library
add_library(levikno STATIC)

target_sources(levikno
    PRIVATE
    ${LVN_SRC_CORE}
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

# window
add_library(lvnwindow STATIC)

target_sources(lvnwindow
    PRIVATE
    ${LVN_SRC_WINDOW}
)

target_include_directories(lvnwindow
    PRIVATE
        include/levikno
        include/levikno/api
        src
        src/external/glfw/include
        src/api/window/glfw
)

# Build definitions
target_compile_definitions(lvnwindow PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# graphics
add_library(lvngraphics STATIC)

target_sources(lvngraphics
    PRIVATE
    ${LVN_SRC_GRAPHICS}
)

target_include_directories(lvngraphics
    PRIVATE
        include/levikno
        include/levikno/api
        src
        src/external
        src/external/glfw/include
        src/external/glad/include
)

# Build definitions
target_compile_definitions(lvngraphics PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# Build examples
if(LVN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
