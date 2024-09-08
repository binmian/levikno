cmake_minimum_required(VERSION 3.24)

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



# Find VulkanSDK
set(MIN_VULKAN_VERSION 1.2)
find_package(Vulkan ${MIN_VULKAN_VERSION} REQUIRED COMPONENTS glslang)

if(Vulkan_FOUND)
	message ("Vulkan found")
	add_definitions(-DLVN_GRAPHICS_API_INCLUDE_VULKAN)

	set (LVN_VULKAN_SRC
		src/api/graphics/vulkan/lvn_vulkan.cpp
		src/api/graphics/vulkan/lvn_vulkan.h
		src/api/graphics/vulkan/lvn_vulkanBackends.h
	)

	if(Vulkan_glslang_FOUND)
		message ("glslang found")
	else()
		message ("FATAL_ERROR cannot find glslang")
	endif()

	set(VULKAN_LIB_DIR
		Vulkan::Vulkan
		Vulkan::glslang
	)

else()
	message ("cannot find Vulkan")
endif()

# Source build files

# Graphics
set(LVN_GRAPHICS_SRC
	# glad
	src/external/glad/include/glad/glad.h
	src/external/glad/src/glad.c

	# opengl
	src/api/graphics/opengl/lvn_opengl.cpp
	src/api/graphics/opengl/lvn_opengl.h

	# vulkan
	${LVN_VULKAN_SRC}
)

set(LVN_SOURCES
	include/levikno/levikno.h
	src/levikno.cpp
	src/levikno_internal.h
	src/lvn_loadModel.cpp
	src/lvn_loadModel.h

	# glfw
	src/api/window/glfw/lvn_glfw.cpp
	src/api/window/glfw/lvn_glfw.h

	# vma
	src/external/vk_mem_alloc.h

	# stb
	src/external/stb_image.c
	src/external/stb_image.h

	# json
	src/external/json.h

	# miniaudio
	src/external/miniaudio.c
	src/external/miniaudio.h

	${LVN_GRAPHICS_SRC}
)

# library
add_library(levikno STATIC ${LVN_SOURCES})

if (MSVC)
	target_compile_options(levikno PRIVATE "/W2")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
	target_compile_options(levikno PRIVATE "-w")
endif()

# Subdirectories ---------------------------------------- /
# GLFW
add_subdirectory(${CMAKE_SOURCE_DIR}/src/external/glfw)

# freetype
add_subdirectory(${CMAKE_SOURCE_DIR}/src/external/freetype)


target_include_directories(levikno
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}/include/levikno
		${PROJECT_SOURCE_DIR}/src/api/graphics/vulkan
		${PROJECT_SOURCE_DIR}/src/api/graphics/opengl
		${PROJECT_SOURCE_DIR}/src/api/window/glfw
		${PROJECT_SOURCE_DIR}/src/external
		${PROJECT_SOURCE_DIR}/src/external/glfw/include
		${PROJECT_SOURCE_DIR}/src/external/glad/include
		${PROJECT_SOURCE_DIR}/src/external/vulkan/include
		${PROJECT_SOURCE_DIR}/src/external/freetype/include
)


# Build definitions
target_compile_definitions(levikno PRIVATE
	$<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
	$<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

target_link_libraries(levikno
	PRIVATE
		glfw
		freetype
		${VULKAN_LIB_DIR}
)

# Build examples
if(LVN_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
