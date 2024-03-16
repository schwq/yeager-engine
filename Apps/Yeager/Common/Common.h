//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

// Enable all kind of important messages, useful for debugging
#define YEAGER_DEBUG_VERBOSE true

// Version is written as (major version * 100 + minor version)
#define YEAGER_VERSION 100
#define YEAGER_BETA_VERSION true
#define YEAGER_ALPHA_VERSION false
#define YEAGER_SNAPSHOT_VERSION false

#define YEAGER_CPP_PRE_98_VERSION 1
#define YEAGER_CPP_98_VERSION 199711L
#define YEAGER_CPP_11_VERSION 201103L
#define YEAGER_CPP_14_VERSION 201402L
#define YEAGER_CPP_17_VERSION 201703L
#define YEAGER_CPP_20_VERSION 202002L

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

#ifdef YEAGER_SYSTEM_WINDOWS_x64&& defined(_MSC_VER)
#ifdef _DEBUG
#define YEAGER_DEBUG true
#endif
#elif defined(YEAGER_SYSTEM_LINUX)
#define YEAGER_DEBUG true
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
#define YEAGER_FORCE_INLINE inline
#define YEAGER_UINT unsigned int

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
#include <future>
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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "loguru.hpp"
#include "portable-file-dialogs.h"

#define FMT_HEADER_ONLY
#include "core.h"
#include "format.h"
#include <irrKlang/irrKlang.h>

#define YEAGER_STRING_ERROR(error) {fmt::format("String error []", error)} 

typedef const char* Cchar;
typedef std::string String;
typedef glm::mat4 Matrix4;
typedef glm::mat3 Matrix3;
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;

#define YEAGER_IDENTITY_MATRIX4x4 Matrix4(1.0f)
#define YEAGER_ZERO_VECTOR3 Vector3(0.0f)

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
