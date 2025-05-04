cmake_minimum_required(VERSION 3.24)

include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 17)

# Levikno
project(Levikno)

option(LVN_BUILD_EXAMPLES "Build example programs" ON)

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


# Find VulkanSDK
set(MIN_VULKAN_VERSION 1.2)
find_package(Vulkan ${MIN_VULKAN_VERSION})

if(Vulkan_FOUND)
	message("Vulkan found")
	add_definitions(-DLVN_GRAPHICS_API_INCLUDE_VULKAN)

	set (LVN_VULKAN_SRC
		src/api/graphics/vulkan/lvn_vulkan.cpp
		src/api/graphics/vulkan/lvn_vulkan.h
		src/api/graphics/vulkan/lvn_vulkanBackends.h
	)

	# Find glslang
	find_package(glslang CONFIG)

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

	# include libs
	set(LVN_VULKAN_LIBS Vulkan::Vulkan)

	if (TARGET glslang::glslang AND TARGET glslang::SPIRV)
		add_definitions(-DLVN_INCLUDE_GLSLANG_SUPPORTED)
		list(APPEND LVN_VULKAN_LIBS glslang::glslang glslang::SPIRV)
	endif()

else()
	message("cannot find Vulkan, skipping vulkan")
endif()


# Source build files

set(LVN_API_SRC
	# glfw
	src/api/window/glfw/lvn_glfw.cpp
	src/api/window/glfw/lvn_glfw.h

	# loaders
	src/api/loaders/lvn_loader_gltf.cpp
	src/api/loaders/lvn_loader_obj.cpp
	src/api/loaders/lvn_loaders.h

	# opengl
	src/api/graphics/opengl/lvn_opengl.cpp
	src/api/graphics/opengl/lvn_opengl.h
	src/api/graphics/opengl/lvn_openglBackends.h

	# vulkan
	${LVN_VULKAN_SRC}
)

set(LVN_EXTERNAL_SRC
	# glad
	src/external/glad/include/glad/glad.h
	src/external/glad/src/glad.c

	# vma
	src/external/vk_mem_alloc.h

	# stb
	src/external/stb_image.h
	src/external/stb_image_write.h
	src/external/stb_impl.cpp

	# json
	src/external/json.h

	# miniaudio
	src/external/miniaudio.cpp
	src/external/miniaudio.h

	# mikktspace
	src/external/mikktspace.cpp
	src/external/mikktspace.h
)

set(LVN_SRC
	include/levikno/levikno.h
	src/levikno.cpp
	src/levikno_internal.h
	src/lvn_renderer.cpp
)


# Subdirectories

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(src/external/glfw)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:glfw>)
include_directories(BEFORE SYSTEM src/external/glfw/include)


# freetype
set(FT_DISABLE_ZLIB TRUE CACHE BOOL "")
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "")
set(FT_DISABLE_PNG TRUE CACHE BOOL "")
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "")
set(FT_DISABLE_BROTLI TRUE CACHE BOOL "")
set(FT_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(src/external/freetype)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:freetype>)
include_directories(BEFORE SYSTEM src/external/freetype/include)


# enet
set(ENET_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(src/external/enet)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:enet>)
include_directories(BEFORE SYSTEM src/external/enet/include)


# library
add_library(levikno STATIC
	${LVN_API_SRC}
	${LVN_EXTERNAL_SRC}
	${LVN_SRC}
)

if (MSVC)
	target_compile_options(levikno PRIVATE "/W2")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
	target_compile_options(levikno PRIVATE -w)
endif()


target_include_directories(levikno
	PRIVATE
		src
		include/levikno
		src/api/graphics/vulkan
		src/api/graphics/opengl
		src/api/loaders
		src/api/window/glfw
		src/external
		src/external/glad/include
		src/external/vulkan/include
)


# Build definitions
target_compile_definitions(levikno PRIVATE
	$<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
	$<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

if (WIN32)
	set(LVN_PLATFORM_LIBS ws2_32.lib winmm.lib)
endif()

target_link_libraries(levikno
	PRIVATE
		${LVN_VULKAN_LIBS}
		${LVN_PLATFORM_LIBS}
)

# Build examples
if(LVN_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
