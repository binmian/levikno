file(COPY res DESTINATION .)

set(LVN_SOURCES
	antiAliasing.cpp
	basicScene/src/Main.cpp
	cubemap.cpp
	framebuffer.cpp
	loadingModel.cpp
	logging.cpp
	pong.cpp
	shaderLoading.cpp
	simpleMatrix.cpp
	simpleSound.cpp
	simpleText.cpp
	simpleTexture.cpp
	simpleTriangle.cpp
	simpleWindow.cpp
)

foreach(LVN_SRC ${LVN_SOURCES})
	get_filename_component(LVN_SRC_NAME ${LVN_SRC} NAME)
	string(REPLACE ".cpp" "" LVN_SRC_NAME ${LVN_SRC_NAME})

	add_executable(${LVN_SRC_NAME} ${LVN_SRC})
	target_include_directories(${LVN_SRC_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(${LVN_SRC_NAME} PRIVATE levikno)
endforeach()
