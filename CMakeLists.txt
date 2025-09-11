cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

include(FindPkgConfig)


# levikno
project(Levikno)

option(LVN_BUILD_EXAMPLES "Build example programs" TRUE)
option(LVN_INCLUDE_GLSLANG "include glslang libraries and shader source compile support" TRUE)
option(LVN_INCLUDE_GLFW "include glfw support" TRUE)

# platform
if (WIN32)
    add_definitions(-DLVN_PLATFORM_WINDOWS)
    add_definitions(-DLVN_INCLUDE_WIN32)
elseif(APPLE)
    add_definitions(-DLVN_PLATFORM_OSX)
    add_definitions(-DLVN_INCLUDE_COCOA)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DLVN_PLATFORM_LINUX)
else()
    add_definitions(-DLVN_PLATFORM_UNKNOWN)
endif()

# x11
find_package(X11)

if (X11_FOUND)
    add_definitions(-DLVN_INCLUDE_X11)
endif()

# wayland
pkg_check_modules(Wayland QUIET
    wayland-client>=0.2.7
    wayland-cursor>=0.2.7
    wayland-egl>=0.2.7
    xkbcommon>=0.5.0)

if (Wayland_FOUND)
    add_definitions(-DLVN_INCLUDE_WAYLAND)
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


# glfw
if (LVN_INCLUDE_GLFW)
    add_definitions(-DLVN_INCLUDE_GLFW)
endif()

# find vulkanSDK
set(MIN_VULKAN_VERSION 1.2)
find_package(Vulkan ${MIN_VULKAN_VERSION})

if(Vulkan_FOUND)
    message("Vulkan found")
    add_definitions(-DLVN_INCLUDE_VULKAN)

    set (LVN_VULKAN_SRC
        src/api/lvn_impl_vulkan.cpp
        src/api/lvn_impl_vulkan.h
    )

    # find glslang
    find_package(glslang CONFIG)

    # include libs
    set(LVN_VULKAN_LIBS Vulkan::Vulkan)

    if (LVN_INCLUDE_GLSLANG)
        if (TARGET glslang::glslang)
            message("glslang found")
            set(LVN_USE_GLSLANG)
        else()
            message("cannot find glslang, skipping glslang source compile features")
        endif()

        if (TARGET glslang::SPIRV)
            message("glslang-SPIRV found")
        else()
            message("cannot find SPIRV, skipping glslang source compile features")
        endif()

        if (TARGET glslang::glslang AND TARGET glslang::SPIRV)
            add_definitions(-DLVN_INCLUDE_GLSLANG_SUPPORTED)
            list(APPEND LVN_VULKAN_LIBS glslang::glslang glslang::SPIRV)
        endif()
    endif()

else()
    message("cannot find Vulkan, skipping vulkan")
endif()


set(LVN_SRC_CORE
    include/levikno/levikno.h
    include/levikno/lvn_ecs.h

    src/levikno.cpp
    src/levikno_internal.h
    src/lvn_cds.cpp
    src/lvn_ecs.cpp
    src/lvn_platform.cpp
    src/lvn_platform.h
)

set(LVN_SRC_GRAPHICS
    include/levikno/lvn_graphics.h
    src/lvn_graphics.cpp
    src/lvn_graphics_internal.h

    # glad
    src/external/glad/src/glad.c
    src/external/glad/include/glad/glad.h

    #glfw
    src/api/lvn_impl_glfw.h
    src/api/lvn_impl_glfw.cpp

    # vulkan
    src/external/vk_mem_alloc.h
    ${LVN_VULKAN_SRC}

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

list(APPEND LVN_SRC_GRAPHICS $<TARGET_OBJECTS:glfw>)
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
        src
        src/api
        src/external/glfw/include
        src/external/glad/include
)

# Build definitions
target_compile_definitions(levikno PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# graphics
add_library(lvngraphics STATIC)

target_sources(lvngraphics
    PRIVATE
    ${LVN_SRC_GRAPHICS}
)

target_link_libraries(lvngraphics PRIVATE
    levikno
    ${LVN_VULKAN_LIBS}
)

target_include_directories(lvngraphics
    PRIVATE
        include/levikno
        src
        src/external
        src/external/glfw/include
        src/external/glad/include
        src/external/vulkan/include
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
