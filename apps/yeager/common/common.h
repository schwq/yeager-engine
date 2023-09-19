// Yaeger Engine 2023 - Present
// Game and Rendering Engine written in C++
// By: schwq
// Git repo: https://github.com/schwq/yeager-engine
// Feel free to mail a email to me about any issues encounter: schwz@protonmail.com

#pragma once

#if !defined(__cplusplus)
#error C++ needed to compile project!.
#endif

// Version is written as (major version * 100 + minor version)
#define YEAGER_VERSION 1000

#if defined(__clang__) && !defined(__ibmxl__)
#define YEAGER_CLANG_VERSION (__clang_major__ * 100 + __clang_minor)
#define YEAGER_COMPILER "CLANG"
#else
#define YEAGER_CLANG_VERSION 0
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && \
    !defined(__NVCOMPILER)
#define YEAGER_GNU_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#define YEAGER_COMPILER "GNUC"
#else
#define YEAGER_GNU_VERSION 0
#endif

#ifdef _MSC_VER
#define YEAGER_MSC_VERSION _MSC_VER
#define YEAGER_MSC_WARNING(...) __pragma(warning(__VA_ARGS__))
#define YEAGER_COMPILER "MSC"
#else
#define YEAGER_MSC_VERSION 0
#define YEAGER_MSC_WARNING(...)
#endif

#ifdef _MSVC_LANG
#define YEAGER_CPLUSPLUS _MSVC_LANG
#else
#define YEAGER_CPLUSPLUS __cplusplus
#endif

#ifdef __has_feature
#define YEAGER_HAS_FEATURE(x) __has_feature(x)
#else
#define YEAGER_HAS_FEATURE(x) 0
#endif

#ifdef __has_cpp_attribute
#define YEAGER_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define YEAGER_HAS_CPP_ATTRIBUTE(x) 0
#endif

#if YEAGER_HAS_CPP_ATTRIBUTE(nodiscard)
#define YEAGER_NODISCARD [[nodiscard]]
#else
#define YEAGER_NODISCARD
#endif

#define YEAGER_NULL_LITERAL "NULL"

// clang-format off

#include <math.h>
#include <string.h>
#include <algorithm>
#include <climits>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cassert>
#include <thread>
#include <filesystem>
#include <fstream>
#include <ios>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../../libs/imgui/imgui.h"
#include "../../../libs/imgui/imgui_impl_glfw.h"
#include "../../../libs/imgui/imgui_impl_opengl3.h"
#include "../../../libs/imgui/imgui_stdlib.h"
#include "../../../libs/loguru.hpp"
#include "../../../libs/portable-file-dialogs.h"

#define FMT_HEADER_ONLY
#include "../../../libs/fmt/core.h"
#include "../../../libs/fmt/format.h"

#include <irrKlang/irrKlang.h>

extern unsigned int kWindowX;
extern unsigned int kWindowY;
extern const char* kOperatingSystem;
extern std::string kDefaultTexturePath;
extern std::string GetPath(std::string path);

typedef unsigned int uint;
typedef std::string String;
typedef glm::mat4 Matrix4;
typedef glm::mat3 Matrix3;
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;

// kPS means Path separator
#if defined(_WIN32)
#define kPS '\\'
#elif defined(__linux__)
#define kPS '/'
#endif

// Logguru defines
#define INFO 0
#define WARNING -1
#define ERROR -2

// Yeager defines
#define SYSTEM_SD 0
#define USER_SD 1
#define IMGUI_RED_ERROR_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define IMGUI_YELLOW_WARNING_COLOR ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define IMGUI_WHITE_COMMON_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

// clang-format on