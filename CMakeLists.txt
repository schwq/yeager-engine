set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.0)
set(projectName YeagerEngine)
project(${projectName})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PHYSX_BUILD_TYPE "The build type of PhysX, i.e., one of {debug, checked, profile, release}" "checked")

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building YeagerEngine in debug configuration")
    add_compile_definitions(DDEBUG)
    link_directories("Engine/Lib/Physx/linux.clang/debug")
else()
    message("Building YeagerEngine in release configuration with Physx ${PHYSX_BUILD_TYPE} configuration")
    add_compile_definitions(NDEBUG)
    link_directories("Engine/Lib/Physx/linux.clang/${PHYSX_BUILD_TYPE}")
endif() 


include_directories(
    Engine/Include/imgui 
    Engine/Include/Physx/include 
    Engine/Include/Physx/include/pvdruntime 
    Engine/Include/UUID 
    Engine/Include/STB_Image 
    Engine/Include/PFD 
    Engine/Include/Loguru 
    Engine/Include/implot 
    Engine/Include/fmt
    Engine/Source
)

add_subdirectory(Engine/Include)

add_subdirectory(Engine/Source/Main)
add_subdirectory(Engine/Source/Common)
add_subdirectory(Engine/Source/Components)
add_subdirectory(Engine/Source/Editor)

add_subdirectory(Engine/ThirdParty)

add_executable(${projectName} ${SOURCE_FILES}  ${LIBRARIES_FILES})
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

add_definitions(-w -DDEBUG_ENABLED_ALL) 
