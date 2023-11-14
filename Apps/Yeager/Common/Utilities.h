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

extern unsigned int ygWindowWidth;
extern unsigned int ygWindowHeight;
extern YgCchar kOperatingSystem;
extern YgString kDefaultTexturePath;
extern YgString GetPath(YgString path);
extern YgCchar GetShaderPath(YgString shader);

namespace Yeager {
struct MemoryManagement {
  uint32_t m_MemoryAllocatedSize = 0;
  uint32_t m_MemoryFreedSize = 0;
  uint32_t GetMemortUsage();
};
extern MemoryManagement s_MemoryManagement;
}  // namespace Yeager

extern void* operator new(size_t s);
extern void operator delete(void* ptr, size_t s);

extern YgString RemoveSuffixUntilCharacter(YgString expression, char characterToStop);
extern YgString RemovePreffixUntilCharacter(YgString expression, char characterToStop);