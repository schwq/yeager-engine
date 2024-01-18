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

#include "Common.h"

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

extern unsigned int ygWindowWidth;
extern unsigned int ygWindowHeight;
extern YgCchar kOperatingSystem;
extern YgString kDefaultTexturePath;
extern YgString GetPath(YgString path);
extern YgCchar GetShaderPath(YgString shader);

#ifdef YEAGER_SYSTEM_LINUX
extern YgString GetLinuxHomeDirectory();
#endif

namespace Yeager {
struct MemoryManagement {
  uint32_t m_MemoryAllocatedSize = 0;
  uint32_t m_MemoryFreedSize = 0;
  unsigned long long m_NumberNewCalls = 0;
  unsigned long long m_NumberDeleteCalls = 0;
  uint32_t GetMemortUsage();
};
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

extern bool ValidatesPath(const std::filesystem::path& p, bool declare = true,
                          std::filesystem::file_status s = std::filesystem::file_status());

}  // namespace Yeager

extern void* operator new(size_t s);
extern void operator delete(void* ptr, size_t s);

extern YgString RemoveSuffixUntilCharacter(YgString expression, char characterToStop);
extern YgString RemovePreffixUntilCharacter(YgString expression, char characterToStop);
extern YgString ReadSuffixUntilCharacter(YgString expression, char characterToStop);
extern size_t NumberOfFilesInDir(YgString path);

struct YgDate_t {
  YgString Month;
  YgString WeekDay;
  unsigned int Day;
  unsigned int Year;
};

struct YgClock_t {
  long long Millis;
  unsigned int Seconds;
  unsigned int Minutes;
  unsigned int Hours;
};

struct YgTime_t {
  YgDate_t Date;
  YgClock_t Time;
};

extern YgString CurrentTimeFormatToString();
extern YgTime_t CurrentTimeToTimeType();
