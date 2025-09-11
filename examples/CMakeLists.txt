set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

set(LVN_SOURCES
    entityComponentSystem.cpp
    events.cpp
    logging.cpp
    multithreading.cpp
    simpleTriangle.cpp
    simpleWindow.cpp
)

foreach(LVN_SRC ${LVN_SOURCES})
    get_filename_component(LVN_SRC_NAME ${LVN_SRC} NAME)
    string(REPLACE ".cpp" "" LVN_SRC_NAME ${LVN_SRC_NAME})

    add_executable(${LVN_SRC_NAME} ${LVN_SRC})
    target_include_directories(${LVN_SRC_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(${LVN_SRC_NAME} PRIVATE levikno lvngraphics)
endforeach()
