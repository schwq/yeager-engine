#include "ImageUtilities.h"
#include "../../Application.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace Yeager;

bool Yeager::MakeScreenShot(Yeager::ApplicationCore* application, Cchar output) noexcept
{
  Vector2 wndSize = application->GetWindow()->GetWindowSize();
  size_t lenght = wndSize.x * wndSize.y;
  ImagePixel* pixels = new ImagePixel[lenght];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  glReadPixels(0, 0, wndSize.x, wndSize.y, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // Flip the Image in the X and Y axis
  for (int line = 0; line != wndSize.y / 2; ++line) {
    std::swap_ranges(
        std::begin(pixels, lenght) + static_cast<int>(wndSize.x) * line,
        std::begin(pixels, lenght) + static_cast<int>(wndSize.x) * (line + 1),
        std::begin(pixels, lenght) + static_cast<int>(wndSize.x) * (static_cast<int>(wndSize.y) - line - 1));
  }

  stbi_write_jpg(output, wndSize.x, wndSize.y, 3, pixels, wndSize.x);
  delete pixels;
  return true;
}

String Yeager::ImageExtensionToString(ImageExtension ext)
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

bool Yeager::MakeScreenShotMiddle(Yeager::ApplicationCore* application, Cchar output) noexcept
{
  size_t lenght = 800 * 800;
  const Vector2 wndSize = application->GetWindow()->GetWindowSize();
  ImagePixel* pixels = new ImagePixel[lenght];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  int pos_x = (wndSize.x / 2) - 400;
  int pos_y = (wndSize.y / 2) - 400;
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

bool Yeager::MakeScreenShotInPosition(Yeager::ApplicationCore* application, Cchar output, Uint pos_x, Uint pos_y,
                                      Uint size_x, Uint size_y)
{
  const Vector2 wndSize = application->GetWindow()->GetWindowSize();
  if (size_x == 0 || size_x > wndSize.x || size_y == 0 || size_y > wndSize.y) {
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