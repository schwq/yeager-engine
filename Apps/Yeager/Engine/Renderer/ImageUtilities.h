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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"

namespace Yeager {

class ApplicationCore;

/**
 * @brief Holds all types of file extensions relate to image, ex: jpeg, png, ect
 */
struct ImageExtension {
  enum Enum { ePNG, eJPEG, eUNDEFINED };
  static String ImageExtensionToString(ImageExtension::Enum ext);
};

/**
 * @brief The image pixel is made of 3 std::byte for rgb stored in a array 
 */
struct ImagePixelRGB {
  std::byte mColors[3];
};

/**
 * @brief ProcessedImageInfo holds information about a recent processed image, it have variables that indicate if the process was successful, and other
 * information, like width, height, path, and the pixel count 
 */
struct ProcessedImageInfo {
  ProcessedImageInfo() = default;
  ProcessedImageInfo(bool succeded, size_t width, size_t height, size_t pixels, String path)
      : bSucceeded(succeded), mWidth(width), mHeight(height), mPixelCount(pixels), mPathWrittenTo(path)
  {}
  bool bSucceeded = false;
  size_t mWidth = 0, mHeight = 0, mPixelCount = 0;
  String mPathWrittenTo = YEAGER_NULL_LITERAL;
};

/**
 * @brief Reads all the pixels on the current framebuffer and stores them in an image .jpg on the given path
 * @return ProcessImageInfo about the situation of the process
 */
extern ProcessedImageInfo MakeScreenShot(Yeager::ApplicationCore* application, const String& output) noexcept;

/**
 * @brief Reads the pixels of the middle of the screen, with the given size, and stores them in an image .jpg on the given path
 * @return ProcessImageInfo about the situation of the process
 */
extern ProcessedImageInfo MakeScreenShotMiddle(Yeager::ApplicationCore* application, const String& output,
                                               const UVector2 size) noexcept;

/**
 * @brief Reads the pixels in the given position of the screen and size, and stores them in an image .jpg on the given path
 * @return ProcessImageInfo about the situation of the process
 */
extern ProcessedImageInfo MakeScreenShotInPosition(Yeager::ApplicationCore* application, const String& output,
                                                   const UVector2 pos, const UVector2 size) noexcept;

}  // namespace Yeager