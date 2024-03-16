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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"

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
enum class ImageExtension { EPng, EJpeg };
extern String ImageExtensionToString(ImageExtension ext);
}  // namespace Yeager

struct ImagePixel {
  std::byte Colors[3];
};

extern bool MakeScreenShot(Cchar output) noexcept;
extern bool MakeScreenShotMiddle(Cchar output) noexcept;
extern bool MakeScreenShotInPosition(Cchar output, unsigned int pos_x, unsigned int pos_y, unsigned int size_x,
                                     unsigned int size_y);