set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.0)
set(projectName yaeger_engine)
project(${projectName})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARIES
    ../../libs/glad.c
    ../../libs/loguru.cpp 
    ../../libs/loguru.hpp
    ../../libs/stb_image.h
    ../../libs/imgui/imgui_impl_opengl3.h
    ../../libs/imgui/imgui_impl_opengl3.cpp
    ../../libs/imgui/imgui_impl_opengl3_loader.h
    ../../libs/imgui/imgui_impl_glfw.h
    ../../libs/imgui/imgui_impl_glfw.cpp
    ../../libs/imgui/imgui.h
    ../../libs/imgui/imgui.cpp
    ../../libs/imgui/imgui_draw.cpp
    ../../libs/imgui/imgui_tables.cpp
    ../../libs/imgui/imgui_widgets.cpp
    ../../libs/imgui/imgui_demo.cpp
    ../../libs/imgui/imgui_stdlib.cpp
)

set(SOURCEFILES 
    ${LIBRARIES}
    main.cpp 
    interface.cpp 
    application.cpp 
    input.cpp 
    engine/renderer/renderer.cpp 
    engine/renderer/shader.cpp 
    engine/renderer/shapes.cpp 
    engine/renderer/texture.cpp 
    engine/renderer/window.cpp 
    engine/editor/camera.cpp 
    engine/editor/editor_explorer.cpp
    engine/editor/editor_console.cpp
    common/common.cpp
    engine/editor/toolbox_object.cpp
)

add_executable(${projectName} ${SOURCEFILES})
target_link_libraries(${projectName} glfw GLU dl)
add_definitions(-w)