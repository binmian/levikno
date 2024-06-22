# basic scene
set(LVN_BASIC_SCENE_SRC
	basicScene/src/Main.cpp
)

add_executable(basicScene ${LVN_BASIC_SCENE_SRC})

target_include_directories(basicScene 
		PRIVATE
		${PROJECT_SOURCE_DIR}/basicScene
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(basicScene
	PRIVATE
		levikno
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT basicScene)
endif()

# pong
add_executable(pong pong.cpp)

target_include_directories(pong 
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(pong
	PRIVATE
		levikno
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT pong)
endif()


# simple texture
add_executable(simpleTexture simpleTexture.cpp)

target_include_directories(simpleTexture 
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(simpleTexture
	PRIVATE
		levikno
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simpleTexture)
endif()


# simple triangle
add_executable(simpleTriangle simpleTriangle.cpp)

target_include_directories(simpleTriangle 
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(simpleTriangle
	PRIVATE
		levikno
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simpleTriangle)
endif()
