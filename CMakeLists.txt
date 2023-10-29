set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.0)
set(projectName yeager)
project(${projectName})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARIES
    Libraries/glad.c
    Libraries/loguru.cpp 
    Libraries/loguru.hpp
    Libraries/stb_image.h
    Libraries/imgui/imgui_impl_opengl3.h
    Libraries/imgui/imgui_impl_opengl3.cpp
    Libraries/imgui/imgui_impl_opengl3_loader.h
    Libraries/imgui/imgui_impl_glfw.h
    Libraries/imgui/imgui_impl_glfw.cpp
    Libraries/imgui/imgui.h
    Libraries/imgui/imgui.cpp
    Libraries/imgui/imgui_draw.cpp
    Libraries/imgui/imgui_tables.cpp
    Libraries/imgui/imgui_widgets.cpp
    Libraries/imgui/imgui_demo.cpp
    Libraries/imgui/imgui_stdlib.cpp
)

set(SOURCEFILES 
    ${LIBRARIES}
    Apps/Yeager/main.cpp  
    Apps/Yeager/Scene.cpp 
    Apps/Yeager/Interface.cpp 
    Apps/Yeager/Application.cpp 
    Apps/Yeager/InputHandle.cpp 
    Apps/Yeager/Serialization.cpp
    Apps/Yeager/Common/Utilities.cpp 
    Apps/Yeager/Engine/Renderer/Render.cpp 
    Apps/Yeager/Engine/Renderer/ShaderHandle.cpp
    Apps/Yeager/Engine/Media/AudioHandle.cpp 
    Apps/Yeager/Engine/Renderer/Entity.cpp
    Apps/Yeager/Engine/Renderer/TextureHandle.cpp 
    Apps/Yeager/Engine/Renderer/Window.cpp 
    Apps/Yeager/Engine/Renderer/ImportedObj.cpp
    Apps/Yeager/Engine/Renderer/Geometry.cpp
    Apps/Yeager/Engine/Editor/Camera.cpp 
    Apps/Yeager/Engine/Editor/Explorer.cpp
    Apps/Yeager/Engine/Editor/ToolboxObj.cpp
    Apps/Yeager/Engine/Physics/PhysicsHandle.cpp
    Apps/Yeager/Common/LogEngine.cpp
)

add_executable(${projectName} ${SOURCEFILES})
target_link_libraries(${projectName} glfw GLU dl assimp IrrKlang yaml-cpp)
add_definitions(-w)