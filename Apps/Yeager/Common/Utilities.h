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

extern void* operator new(size_t s);
extern void operator delete(void* ptr, size_t s);

namespace Yeager {

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

#define YEAGER_DELETE_SMARTPTR(ptr) \
  if (ptr) {                        \
    ptr.reset();                    \
    delete ptr;                     \
    ptr = NULL;                     \
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
    for (YEAGER_UINT x = 0; x < Bytes; x++) {
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

struct MemoryManagement {
  uint32_t m_MemoryAllocatedSize = 0;
  uint32_t m_MemoryFreedSize = 0;
  unsigned long long m_NumberNewCalls = 0;
  unsigned long long m_NumberDeleteCalls = 0;
  uint32_t GetMemortUsage();
};

struct YgDate_t {
  unsigned int Month = 0;
  unsigned int WeekDay = 0;
  unsigned int Day = 0;
  unsigned int Year = 0;
};

struct YgClock_t {
  long long Millis = 0;
  unsigned int Seconds = 0;
  unsigned int Minutes = 0;
  unsigned int Hours = 0;
};

extern String MonthNumberToString(int month);
extern String WeekDayNumberToString(int weekday);

struct YgTime_t {
  YgDate_t Date;
  YgClock_t Time;
};

extern unsigned int ygWindowWidth;
extern unsigned int ygWindowHeight;
extern Cchar kOperatingSystem;
extern String kDefaultTexturePath;
extern String GetPath(String path);
extern Cchar GetShaderPath(String shader);

extern String GetExternalFolderPath();

extern MemoryManagement s_MemoryManagement;

/**
 * @brief Used in switch statements with String
 * @note Thanks to: https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
 * @param str The String (Const char* format)
 * @param h Part of the String to start off, leave it blank!
 * @return constexpr unsigned int 
 */
constexpr unsigned int StringToInteger(const char* str, int h = 0)
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

extern bool CreateDirectoryAndValidate(const std::filesystem::path& p);

extern bool ValidatesPath(const std::filesystem::path& p, bool declare = true,
                          std::filesystem::file_status s = std::filesystem::file_status());

extern String RemoveSuffixUntilCharacter(String expression, char characterToStop);
extern String RemovePreffixUntilCharacter(String expression, char characterToStop);
extern String ReadSuffixUntilCharacter(String expression, char characterToStop);
extern size_t NumberOfFilesInDir(String path);

extern String RemoveExtensionFromFilename(String filename);
extern String ExtractExtensionFromFilename(String filename);

extern String CurrentTimeFormatToString();
extern YgTime_t CurrentTimeToTimeType();
}  // namespace Yeager
