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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"

namespace Yeager {

struct FileHandle {
  std::fstream mFile;
  String mPath = YEAGER_NULL_LITERAL;
  std::size_t mSize = 0;
  bool bValid = false;
};

extern FileHandle OpenFileR(const String& path, std::ios_base::openmode mode);
extern FileHandle OpenFileW(const String& path, std::ios_base::openmode mode);
extern std::size_t GetFileSize(FileHandle& handle);
extern bool CloseFile(FileHandle& handle);

}  // namespace Yeager