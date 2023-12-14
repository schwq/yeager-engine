#include "ImageUtilities.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../../../Libraries/stb_image_write.h"
using namespace Yeager;

bool MakeScreenShot(YgCchar output) noexcept
{
  size_t lenght = ygWindowWidth * ygWindowHeight;
  ImagePixel* pixels = new ImagePixel[lenght];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  glReadPixels(0, 0, ygWindowWidth, ygWindowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // Flip the Image in the X and Y axis
  for (int line = 0; line != ygWindowHeight / 2; ++line) {
    std::swap_ranges(std::begin(pixels, lenght) + ygWindowWidth * line,
                     std::begin(pixels, lenght) + ygWindowWidth * (line + 1),
                     std::begin(pixels, lenght) + ygWindowWidth * (ygWindowHeight - line - 1));
  }

  stbi_write_jpg(output, ygWindowWidth, ygWindowHeight, 3, pixels, ygWindowWidth);
  delete pixels;
  return true;
}

YgString Yeager::ImageExtensionToString(ImageExtension ext)
{
  switch (ext) {
    case ImageExtension::EJpeg:
      return ".jpg";
    case ImageExtension::EPng:
      return ".png";
    default:
      return ".png";
  }
}

extern bool MakeScreenShotMiddle(YgCchar output) noexcept
{
  size_t lenght = 800 * 800;
  ImagePixel* pixels = new ImagePixel[lenght];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  int pos_x = (ygWindowWidth / 2) - 400;
  int pos_y = (ygWindowHeight / 2) - 400;
  glReadPixels(pos_x, pos_y, 800, 800, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // Flip the Image in the X and Y axis
  for (int line = 0; line != 800 / 2; ++line) {
    std::swap_ranges(std::begin(pixels, lenght) + 800 * line, std::begin(pixels, lenght) + 800 * (line + 1),
                     std::begin(pixels, lenght) + 800 * (800 - line - 1));
  }

  stbi_write_jpg(output, 800, 800, 3, pixels, 800);
  delete pixels;
  return true;
}
extern bool MakeScreenShotInPosition(YgCchar output, unsigned int pos_x, unsigned int pos_y, unsigned int size_x,
                                     unsigned int size_y)
{
  if (size_x == 0 || size_x > ygWindowWidth || size_y == 0 || size_y > ygWindowHeight) {
    Yeager::Log(WARNING, "Trying to make a screenshot with invalid size!");
    return false;
  }

  size_t lenght = size_x * size_y;
  ImagePixel* pixels = new ImagePixel[lenght];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  glReadPixels(pos_x, pos_y, size_x, size_y, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // Flip the Image in the X and Y axis
  for (int line = 0; line != size_y / 2; ++line) {
    std::swap_ranges(std::begin(pixels, lenght) + size_x * line, std::begin(pixels, lenght) + size_x * (line + 1),
                     std::begin(pixels, lenght) + size_x * (size_y - line - 1));
  }
  stbi_write_jpg(output, size_x, size_y, 3, pixels, size_y);
  delete pixels;
  return true;
}