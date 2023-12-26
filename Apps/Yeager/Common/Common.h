//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstddef>
#include <cstdint>
#if !defined(__cplusplus)
#error C++ needed to compile project!.
#endif

#include "PlataformDetect.h"

// Enable debug functions and messages
#define YEAGER_DEBUG true

// Enable all kind of important messages, useful for debugging
#define YEAGER_DEBUG_VERBOSE true

// Version is written as (major version * 100 + minor version)
#define YEAGER_VERSION 100
#define YEAGER_BETA_VERSION true
#define YEAGER_ALPHA_VERSION false
#define YEAGER_SNAPSHOT_VERSION false

#if defined(__clang__) && !defined(__ibmxl__)
#define YEAGER_CLANG_VERSION (__clang_major__ * 100 + __clang_minor)
#define YEAGER_COMPILER "CLANG"
#else
#define YEAGER_CLANG_VERSION 0
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__NVCOMPILER)
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

#define YEAGER_NOT_IMPLEMENTED(func)                                     \
  {                                                                      \
    printf("(??) Function not implemented as been called [%s]\n", func); \
  }
#define YEAGER_NULL_LITERAL "NULL"

#define YEAGER_NULLPTR nullptr
#define YEAGER_NULLPTR_T nullptr_t

#include <math.h>
#include <string.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

// clang-format off
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/component_wise.hpp>
//clang-format on

#include "../../../Libraries/imgui/imgui.h"
#include "../../../Libraries/imgui/imgui_impl_glfw.h"
#include "../../../Libraries/imgui/imgui_impl_opengl3.h"
#include "../../../Libraries/imgui/imgui_stdlib.h"
#include "../../../Libraries/loguru.hpp"
#include "../../../Libraries/portable-file-dialogs.h"

#define FMT_HEADER_ONLY
#include "../../../Libraries/fmt/core.h"
#include "../../../Libraries/fmt/format.h"

#include <irrKlang/irrKlang.h>

typedef const char* YgCchar;
typedef std::string YgString;
typedef glm::mat4 YgMatrix4;
typedef glm::mat3 YgMatrix3;
typedef glm::vec2 YgVector2;
typedef glm::vec3 YgVector3;

// PS means Path separator, used in getting path from files and folders
#if defined(YEAGER_SYSTEM_WINDOWS_x64)
#define YG_PS '\\'
#elif defined(YEAGER_SYSTEM_LINUX)
#define YG_PS '/'
#endif

// Logguru defines
#define INFO 0
#define WARNING -1
#define ERROR -2

// Yeager defines
#define YG_SYSTEM 0
#define YG_USER 1
#define IMGUI_RED_ERROR_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define IMGUI_YELLOW_WARNING_COLOR ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define IMGUI_WHITE_COMMON_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

// clang-format on
