#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define YEAGER_SYSTEM_WINDOWS_x64
#include <Windows.h>
#else
#error "This engine does not support Windows x86 build"
#endif

#elif defined(__MINGW32__)
#defne YEAGER_SYSTEM_WINDOWS_MING32
#include <Window.h>

#elif defined(__APPLE__) || defined(__MACH__)
#define YEAGER_SYSTEM_MACOS
#error "This engine does not support MacOS build

#elif defined(__ANDROID__)
#define YEAGER_SYSTEM_ANDROID
#error "This engine does not support Android builds"

#elif defined(__linux__)
#define YEAGER_SYSTEM_LINUX

#else
#error "This engine cannot find the current plataform being used"
#endif
