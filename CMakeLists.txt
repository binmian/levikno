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
	set(PLATFORM_TYPE windows)

elseif(APPLE)
	add_definitions(-DLVN_PLATFORM_MACOS)
	set(PLATFORM_TYPE macos)

elseif(UNIX AND NOT APPLE)
	add_definitions(-DLVN_PLATFORM_LINUX)
	set(PLATFORM_TYPE linux)

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
	find_package(glslang CONFIG REQUIRED)

	if (TARGET glslang::glslang)
		message("glslang found")
	else()
		message("cannot find glslang, please make sure that glslang is installed before building")
	endif()

	if (TARGET glslang::SPIRV)
		message("glslang-SPIRV found")
	else()
		message("cannot find SPIRV, please make sure that SPIRV and glslang are installed before building")
	endif()

	set(LVN_VULKAN_LIBS
		Vulkan::Vulkan
		glslang::glslang
		glslang::SPIRV
	)

else()
	message("cannot find Vulkan")
endif()


# Source build files

set(LVN_API_SRC
	# glfw
	src/api/window/glfw/lvn_glfw.cpp
	src/api/window/glfw/lvn_glfw.h

	# opengl
	src/api/graphics/opengl/lvn_opengl.cpp
	src/api/graphics/opengl/lvn_opengl.h

	# vulkan
	${LVN_VULKAN_SRC}
)

set(LVN_EXTERNAL_SRC
	# glad
	external/glad/include/glad/glad.h
	external/glad/src/glad.c

	# vma
	external/vk_mem_alloc.h

	# stb
	external/stb_image.c
	external/stb_image.h
	external/stb_image_write.c
	external/stb_image_write.h

	# json
	external/json.h

	# miniaudio
	external/miniaudio.c
	external/miniaudio.h

	# mikktspace
	external/mikktspace.c
	external/mikktspace.h
)

set(LVN_SRC
	include/levikno/levikno.h
	src/levikno.cpp
	src/levikno_internal.h
	src/lvn_loadModel.cpp
	src/lvn_loadModel.h
)

# Subdirectories ---------------------------------------- /

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(external/glfw)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:glfw>)
include_directories(BEFORE SYSTEM external/glfw/include)


# freetype
set(FT_DISABLE_ZLIB TRUE CACHE BOOL "")
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "")
set(FT_DISABLE_PNG TRUE CACHE BOOL "")
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "")
set(FT_DISABLE_BROTLI TRUE CACHE BOOL "")
set(FT_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(external/freetype)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:freetype>)
include_directories(BEFORE SYSTEM external/freetype/include)


# enet
set(ENET_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)

add_subdirectory(external/enet)

list(APPEND LVN_EXTERNAL_SRC $<TARGET_OBJECTS:enet>)
include_directories(BEFORE SYSTEM external/enet/include)


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
		src/api/window/glfw
		external
		external/glad/include
		external/vulkan/include
)


# Build definitions
target_compile_definitions(levikno PRIVATE
	$<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
	$<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

target_link_libraries(levikno
	PRIVATE
		${LVN_VULKAN_LIBS}
)

# Build examples
if(LVN_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
