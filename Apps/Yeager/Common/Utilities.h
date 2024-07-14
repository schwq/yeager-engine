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

#include "Common.h"

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

enum FileExtensionType {
  EExtensitonType3DModel,
  EExtensionTypeAudio,
  EExtensionTypeImage,
  EExtensionTypeVideo,
  EExtensionTypeSource,
  EExtensionTypeConfiguration
};

struct FileType {
  FileType() {}
  FileType(String type, FileExtensionType ext, bool supported = false)
  {
    Typename = type;
    ExtType = ext;
    Supported = supported;
  }
  String Typename = YEAGER_NULL_LITERAL;
  FileExtensionType ExtType = FileExtensionType::EExtensionTypeSource;
  bool Supported = false;
};

std::pair<bool, FileType> CheckFileExtensionType(String path, FileExtensionType type);

extern std::map<String, FileType> ExtensionTypesToRawExtensions;
extern String ExtractExtensionTypenameFromPath(String filename);

extern bool EvaluateIntToBool(const int i);

struct YgDate_t {
  Uint Month = 0;
  Uint WeekDay = 0;
  Uint Day = 0;
  Uint Year = 0;
};

struct YgClock_t {
  long long Millis = 0;
  Uint Seconds = 0;
  Uint Minutes = 0;
  Uint Hours = 0;
};

extern String MonthNumberToString(int month);
extern String WeekDayNumberToString(int weekday);

struct YgTime_t {
  YgDate_t Date;
  YgClock_t Time;
};

extern Cchar g_OperatingSystemString;
extern String GetPath(String path);
extern String GetPathFromShared(String path);
extern Cchar GetShaderPath(String shader);

extern String GetExternalFolderPath();

/**
 * @brief Used in switch statements with String
 * @note Thanks to: https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
 * @param str The String (Const char* format)
 * @param h Part of the String to start off, leave it blank!
 * @return constexpr Uint 
 */
constexpr Uint StringToInteger(const char* str, int h = 0)
{
  return !str[h] ? 5381 : (StringToInteger(str, h + 1) * 33) ^ str[h];
}

extern glm::mat4 ConvertAssimpMatrixToGLMFormat(const aiMatrix4x4& from);
constexpr inline glm::vec3 GetGLMVec(const aiVector3D& vec)
{
  return glm::vec3(vec.x, vec.y, vec.z);
}

constexpr inline glm::quat GetGLMQuat(const aiQuaternion& qa)
{
  return glm::quat(qa.w, qa.x, qa.y, qa.z);
}

/** Checks if the directory already exists, if not, it creates it */
extern bool ValidatesAndCreateDirectory(const std::filesystem::path& p);
/** Creates a directory and thrown an error if cannot creates it */
extern bool CreateDirectoryAndValidate(const std::filesystem::path& p);

extern bool ValidatesPath(const std::filesystem::path& p, bool declare = true,
                          std::filesystem::file_status s = std::filesystem::file_status());

/** Given a path, it convert all the text (content) inside the file to a string */
extern String FileContentToString(const std::filesystem::path& p);

extern String RemoveSuffixUntilCharacter(String expression, char characterToStop);
extern String RemovePreffixUntilCharacter(String expression, char characterToStop);
extern String ReadSuffixUntilCharacter(String expression, char characterToStop);
extern size_t NumberOfFilesInDir(String path);

extern String RemoveExtensionFromFilename(String filename);
extern String ExtractExtensionFromFilename(String filename);

/** Used in the quick serialization of files, creates a file, writes the data and closes it. Returns a boolean represeting the success of the operation */
template <typename Type>
bool CreateFileAndWrites(const String& path, const Type& data)
{
  std::ofstream file(path);

  if (!file.is_open()) {
    Yeager::LogDebug(ERROR, "Cannot opens file for writing: {}", path);
    return false;
  }

  file << data;
  file.close();
  return true;
}

extern String CurrentTimeFormatToString();
extern YgTime_t CurrentTimeToTimeType();
}  // namespace Yeager
