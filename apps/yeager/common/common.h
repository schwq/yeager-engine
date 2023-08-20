#pragma once

#include <climits>
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../../libs/loguru.hpp"
#include "../../../libs/imgui/imgui.h"
#include "../../../libs/imgui/imgui_impl_glfw.h"
#include "../../../libs/imgui/imgui_impl_opengl3.h"
#include "../../../libs/imgui/imgui_stdlib.h"
#include "../../../libs/portable-file-dialogs.h"
#define FMT_HEADER_ONLY
#include "../../../libs/fmt/core.h"
#include "../../../libs/fmt/format.h"

extern unsigned int kWindowX;
extern unsigned int kWindowY;
extern const char* kOperatingSystem;
extern std::string GetPath(std::string path);
#define EngineVersion 1.0

typedef unsigned int uint;
typedef std::string String;
typedef glm::mat4 Matrix4;
typedef glm::mat3 Matrix3;
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;

// Logguru defines
#define INFO 0
#define WARNING -1
#define ERROR -2