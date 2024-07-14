set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.0)
set(projectName YeagerEngine)
project(${projectName})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PHYSX_BUILD_TYPE "The build type of PhysX, i.e., one of {debug, checked, profile, release}" "checked")

include_directories(Include/Physx/include Include/Physx/pvdruntime/include Include/imgui Include/fmt Include/STB_Image/ Include/implot Include/PFD Include/Loguru Include/UUID)

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building YeagerEngine in debug configuration")
    add_compile_definitions(DDEBUG)
    link_directories("Libraries/Physx/linux.clang/debug")
else()
    message("Building YeagerEngine in release configuration with Physx ${PHYSX_BUILD_TYPE} configuration")
    add_compile_definitions(NDEBUG)
    link_directories("Libraries/Physx/linux.clang/${PHYSX_BUILD_TYPE}")
endif() 

set(LIBRARIES
    Apps/Yeager/ThirdParty/OpenGL/glad.c
    Include/Loguru/loguru.cpp 
    Include/Loguru/loguru.hpp
    Include/STB_Image/stb_image.h
    Include/STB_Image/stb_image_write.h
    Include/imgui/imgui_impl_opengl3.h
    Include/imgui/imgui_impl_opengl3.cpp
    Include/imgui/imgui_impl_opengl3_loader.h
    Include/imgui/imgui_impl_glfw.h
    Include/imgui/imgui_impl_glfw.cpp
    Include/imgui/imgui.h
    Include/imgui/imgui.cpp
    Include/imgui/imgui_draw.cpp
    Include/imgui/imgui_tables.cpp
    Include/imgui/imgui_widgets.cpp
    Include/imgui/imgui_demo.cpp
    Include/imgui/imgui_stdlib.cpp
    Include/implot/implot.h 
    Include/implot/implot.cpp 
    Include/implot/implot_items.cpp
    Include/implot/implot_internal.h
)

set(SOURCEFILES 
    ${LIBRARIES}
    Apps/Yeager/main.cpp  
    Apps/Yeager/Scene.cpp 
    Apps/Yeager/Launcher.cpp
    Apps/Yeager/DefaultValues.cpp
    Apps/Yeager/DefaultValues.h
    Apps/Yeager/Engine/Interface/Interface.cpp
    Apps/Yeager/Engine/Interface/UIManagement.cpp 
    Apps/Yeager/Application.cpp 
    Apps/Yeager/InputHandle.cpp 
    Apps/Yeager/Serialization.cpp
    Apps/Yeager/Settings.cpp 
    Apps/Yeager/Request.cpp 
    Apps/Yeager/Common/Utilities.cpp 
    Apps/Yeager/Engine/Renderer/ShaderHandle.cpp
    Apps/Yeager/Engine/Renderer/OpenGLRender.cpp
    Apps/Yeager/Engine/Renderer/OpenGLRender.h
    Apps/Yeager/Engine/Media/AudioHandle.cpp 
    Apps/Yeager/Engine/Renderer/Entity.cpp
    Apps/Yeager/Engine/Renderer/Object.cpp
    Apps/Yeager/Engine/Renderer/Importer.cpp
    Apps/Yeager/Engine/Renderer/TextureHandle.cpp 
    Apps/Yeager/Engine/Renderer/ImageUtilities.cpp 
    Apps/Yeager/Engine/Renderer/Window.cpp 
    Apps/Yeager/Engine/Renderer/Skybox.cpp
    Apps/Yeager/Engine/Renderer/Player/PlayableObject.cpp
    Apps/Yeager/Engine/Renderer/Player/PlayableObject.h 
    Apps/Yeager/Engine/Renderer/Animation/Animation.cpp 
    Apps/Yeager/Engine/Renderer/Animation/AnimationEngine.cpp 
    Apps/Yeager/Engine/Renderer/Animation/Bone.cpp 
    Apps/Yeager/Engine/Renderer/Text/TextRendering.cpp
    Apps/Yeager/Engine/Renderer/Text/TextRendering.h
    Apps/Yeager/Engine/Editor/Camera.cpp 
    Apps/Yeager/Engine/Editor/Explorer.cpp
    Apps/Yeager/Engine/Editor/ToolboxObj.cpp
    Apps/Yeager/Engine/Editor/NodeHierarchy.cpp
    Apps/Yeager/Engine/Physics/PhysXHandle.cpp
    Apps/Yeager/Engine/Physics/PhysXGeometryHandle.cpp
    Apps/Yeager/Engine/Physics/PhysXGeometryHandle.h
    Apps/Yeager/Engine/Physics/PhysXCharacterController.cpp 
    Apps/Yeager/Engine/Physics/PhysXActor.cpp
    Apps/Yeager/Common/LogEngine.cpp
    Apps/Yeager/Engine/Lighting/LightHandle.h
    Apps/Yeager/Engine/Lighting/LightHandle.cpp
    Apps/Yeager/Engine/Terrain/ProceduralTerrain.cpp
    Apps/Yeager/Engine/Terrain/ProceduralTerrain.h
    Apps/Yeager/Common/Mathematics.cpp
    Apps/Yeager/Common/Mathematics.h
    Apps/Yeager/Engine/Terrain/PerlinNoise.cpp
    Apps/Yeager/Engine/Terrain/PerlinNoise.h
    Apps/Yeager/Kernel/HardwareInfo.h
    Apps/Yeager/Kernel/HardwareInfo.cpp
    Apps/Yeager/Kernel/NvidiaCuda.h 
    Apps/Yeager/Kernel/NvidiaCuda.cpp
    Apps/Yeager/Kernel/Caching/Cache.h
    Apps/Yeager/Kernel/Caching/Cache.cpp
    
)

add_executable(${projectName} ${SOURCEFILES})
target_link_libraries(${projectName} glfw dl assimp IrrKlang yaml-cpp 
PhysXExtensions_static_64
PhysX_static_64
PhysXPvdSDK_static_64
PhysXCommon_static_64
PhysXFoundation_static_64
PhysXCooking_static_64
PhysXCharacterKinematic_static_64
PVDRuntime_64
pthread
freetype
dl)

add_definitions(-w) 
