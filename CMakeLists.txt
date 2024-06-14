cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)

# Levikno
project(Levikno)

# Architecture
set(ARCHITECTURE_TYPE x64)

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

# Output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})


# Subdirectories ---------------------------------------- /

# GLFW
# set(GLFW_BUILD_WAYLAND 1)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/external/glfw)


# Source build files

# Graphics
set(LVN_GRAPHICS_SRC
	# glad
	src/external/glad/include/glad/glad.h
	src/external/glad/src/glad.c

	# vulkan
	src/api/graphics/vulkan/lvn_vulkan.cpp
	src/api/graphics/vulkan/lvn_vulkan.h
	src/api/graphics/vulkan/lvn_vulkanBackends.h
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
	src/external/vma/include/vk_mem_alloc.cpp
	src/external/vma/include/vk_mem_alloc.h

	# stb
	src/external/stb/include/stb_image.cpp
	src/external/stb/include/stb_image.h

	# json
	src/external/json/include/json.h

  ${LVN_GRAPHICS_SRC}
)

add_library(levikno STATIC ${LVN_SOURCES})

target_include_directories(levikno
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}/include/levikno
		${PROJECT_SOURCE_DIR}/src/api/graphics/vulkan
		${PROJECT_SOURCE_DIR}/src/api/window/glfw
		${PROJECT_SOURCE_DIR}/src/external/glfw/include
		${PROJECT_SOURCE_DIR}/src/external/glad/include
		${PROJECT_SOURCE_DIR}/src/external/vulkan/include
		${PROJECT_SOURCE_DIR}/src/external/vma/include
		${PROJECT_SOURCE_DIR}/src/external/stb/include
		${PROJECT_SOURCE_DIR}/src/external/json/include
)


# Build definitions
target_compile_definitions(levikno PRIVATE
	$<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
	$<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# Find VulkanSDK
set(MIN_VULKAN_VERSION 1.3)
find_package(Vulkan ${MIN_VULKAN_VERSION} REQUIRED COMPONENTS glslang)

if(Vulkan_FOUND)
    message ("Vulkan found")
else()
    message (FATAL_ERROR "Cannot find Vulkan")
endif()

if(Vulkan_glslang_FOUND)
    message ("glslang found")
else()
    message (FATAL_ERROR "Cannot find glslang")
endif()

set(VULKAN_LIB_DIR
	Vulkan::Vulkan
	Vulkan::glslang
)

target_link_libraries(levikno
	PRIVATE 
		glfw
		${VULKAN_LIB_DIR}
)

# LeviknoEditor
set(LVN_EDITOR_SOURCES
	examples/basicScene/src/Main.cpp
)

add_executable(basicScene ${LVN_EDITOR_SOURCES})

target_include_directories(basicScene 
		PRIVATE
		${PROJECT_SOURCE_DIR}/examples/basicScene
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(basicScene
	PRIVATE
		levikno
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT basicScene)
endif()
