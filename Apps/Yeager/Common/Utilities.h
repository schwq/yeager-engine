//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Common.h"
#include "DirectorySystem.h"
#include "Random.h"
#include "Time.h"

namespace std {
template <typename T>
T* begin(T* arr, size_t N)
{
  return arr;
}

template <typename T>
const T* cbegin(const T* arr, size_t N)
{
  return arr;
}

template <typename T>
T* end(T* arr, size_t N)
{
  return arr + N;
}

template <typename T>
const T* cend(const T* arr, size_t N)
{
  return arr + N;
}
}  // namespace std
namespace Yeager {

template <typename... T>
void LogDebug(int verbosity, fmt::format_string<T...> fmt, T&&... args);

// clang-format off
#define YEAGER_ENUM_TO_STRING(struct) static String ToString(struct::Enum type);
#define YEAGER_STRING_TO_ENUM(struct) static struct::Enum ToEnum(const String& str);
#define YEAGER_USING_SHARED_PTR template <class U>  using SharedPtr = std::shared_ptr<U>;

// clang-format on

#define BYTE(n) n * sizeof(uint8_t)
#define KB(n) BYTE(1024 * n)
#define MB(n) KB(1024 * n)
#define GB(n) MB(1024 * n)
#define TB(n) GB(1024 * n)

#define YEAGER_DELETE(ptr) \
  if (ptr) {               \
    delete ptr;            \
    ptr = NULL;            \
  }

#define YEAGER_DELETE_ARRAY(arr) \
  if (arr) {                     \
    delete[] ptr;                \
    ptr = NULL;                  \
  }

#define YEAGER_FREE(ptr) \
  if (ptr) {             \
    free(ptr);           \
    ptr = NULL;          \
  }

namespace CHECK {

struct No {};
template <typename T, typename Arg>
No operator==(const T&, const Arg&);

template <typename T, typename Arg = T>
struct EqualExists {
  enum { value = !std::is_same<decltype(*(T*)(0) == *(Arg*)(0)), No>::value };
};
}  // namespace CHECK

template <uint8_t Bytes>
struct YgPadding {
  uint8_t Padding[Bytes];
  YgPadding()
  {
    for (Uint x = 0; x < Bytes; x++) {
      Padding[x] = 0;
    }
  }
};

extern bool EvaluateIntToBool(const int i);

extern Cchar g_OperatingSystemString;
extern bool g_FromSourceCode;

/**
 * @brief Used in switch statements with String
 * @note Thanks to: https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
 * @param str The String (Const char* format)
 * @param h Part of the String to start off, leave it blank!
 * @return constexpr Uint 
 */
YEAGER_CONSTEXPR Uint StringToInterger(const char* str, int h = 0)
{
  return !str[h] ? 5381 : (StringToInterger(str, h + 1) * 33) ^ str[h];
}

extern glm::mat4 ConvertAssimpMatrixToGLMFormat(const aiMatrix4x4& from);
YEAGER_CONSTEXPR YEAGER_FORCE_INLINE glm::vec3 GetGLMVec(const aiVector3D& vec)
{
  return glm::vec3(vec.x, vec.y, vec.z);
}

YEAGER_CONSTEXPR YEAGER_FORCE_INLINE glm::quat GetGLMQuat(const aiQuaternion& qa)
{
  return glm::quat(qa.w, qa.x, qa.y, qa.z);
}

extern uint64_t Convert32BitsTo64Bits(uint32_t high, uint32_t low);

extern String RemoveSuffixUntilCharacter(String expression, char characterToStop);
extern String RemovePreffixUntilCharacter(String expression, char characterToStop);
extern String ReadSuffixUntilCharacter(String expression, char characterToStop);

/** 
    @brief Returns a vector of string with phrases that are between a specific character for example, the String passes as "usr/share/bin/YeagerEngine"     
    and the character '/', it returns a vector with { "usr", "share", "bin", "YeagerEngine" }
*/
YEAGER_NODISCARD extern std::vector<String> RetrievePhrasesBetweenChar(String str, const char ch);

/**
  @brief Displays a message box warning about the engine panic! It shows the cause of the failure and the log that is the path to the crash report generated!
  In windows system, it calls the function MessageBox from winuser.h
  TODO: Make the message box for linux systems 
*/
extern int DisplayWarningPanicMessageBox(const String& cause, const std::filesystem::path& log);

#ifdef YEAGER_SYSTEM_WINDOWS_x64
extern int DisplayWarningPanicMessageBoxWindows(const String& cause, const std::filesystem::path& log);
#endif

extern String ToLower(const String& str);
extern String ToUpper(const String& str);

}  // namespace Yeager
