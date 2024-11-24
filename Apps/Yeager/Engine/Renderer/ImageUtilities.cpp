#include "ImageUtilities.h"
#include "../../Application.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace Yeager;

String ImageExtension::ImageExtensionToString(ImageExtension::Enum ext)
{
  switch (ext) {
    case ImageExtension::eJPEG:
      return ".jpg";
    case ImageExtension::ePNG:
      return ".png";
    case ImageExtension::eUNDEFINED:
    default:
      return "";  // No extension
  }
}

static void ProcessAlignmentAndRead(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                    void* pixels)
{
  GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL_CALL(glReadBuffer(GL_BACK_LEFT));
  GL_CALL(glReadPixels(x, y, width, height, format, type, pixels));
}
template <typename T>
static void ProcessSwapAxisImage(T buffer, size_t width, size_t height, size_t lenght)
{
  for (int line = 0; line != height / 2; ++line) {
    std::swap_ranges(std::begin(buffer, lenght) + static_cast<int>(width) * line,
                     std::begin(buffer, lenght) + static_cast<int>(width) * (line + 1),
                     std::begin(buffer, lenght) + static_cast<int>(width) * (static_cast<int>(height) - line - 1));
  }
}

ProcessedImageInfo Yeager::MakeScreenShot(Yeager::ApplicationCore* application, const String& output) noexcept
{
  Vector2 wndSz = application->GetWindow()->GetWindowSize();
  const size_t lenght = wndSz.x * wndSz.y;

  auto pixels = std::make_unique<ImagePixelRGB[]>(lenght);

  ProcessAlignmentAndRead(0, 0, wndSz.x, wndSz.y, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
  ProcessSwapAxisImage(pixels.get(), wndSz.x, wndSz.y, lenght);

  if (!stbi_write_jpg(output.c_str(), wndSz.x, wndSz.y, 3, pixels.get(), wndSz.x)) {
    Yeager::Log(ERROR, "stbi_write_jpg cannot write to {}", output);
    pixels.release();
    return ProcessedImageInfo(false, 0, 0, 0, YEAGER_NULL_LITERAL);
  }

  pixels.release();
  return ProcessedImageInfo(true, wndSz.x, wndSz.y, lenght, output);
}

ProcessedImageInfo Yeager::MakeScreenShotMiddle(Yeager::ApplicationCore* application, const String& output,
                                                const UVector2 size) noexcept
{
  const Vector2 wndSize = application->GetWindow()->GetWindowSize();

  const size_t lenght = size.x * size.y;

  auto pixels = std::make_unique<ImagePixelRGB[]>(lenght);

  ProcessAlignmentAndRead((wndSize.x / 2) - static_cast<Uint>(size.x / 2),
                          (wndSize.y / 2) - static_cast<Uint>(size.y / 2), size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE,
                          pixels.get());
  ProcessSwapAxisImage(pixels.get(), size.x, size.y, lenght);

  if (!stbi_write_jpg(output.c_str(), size.x, size.y, 3, pixels.get(), size.x)) {
    Yeager::Log(ERROR, "stbi_write_jpg cannot write to {}", output);
    pixels.release();
    return ProcessedImageInfo(false, 0, 0, 0, YEAGER_NULL_LITERAL);
  }

  pixels.release();
  return ProcessedImageInfo(true, size.x, size.y, lenght, output);
}

ProcessedImageInfo Yeager::MakeScreenShotInPosition(Yeager::ApplicationCore* application, const String& output,
                                                    const UVector2 pos, const UVector2 size) noexcept
{
  const Vector2 wndSize = application->GetWindow()->GetWindowSize();

  if (size.x == 0 || size.x > wndSize.x || size.y == 0 || size.y > wndSize.y) {
    Yeager::Log(WARNING, "Trying to make a screenshot with invalid size!");
    return ProcessedImageInfo(false, 0, 0, 0, YEAGER_NULL_LITERAL);
  }

  if (pos.x >= wndSize.x || pos.y >= wndSize.y) {
    Yeager::Log(WARNING, "Trying to make a screenshot with invalid position!");
    return ProcessedImageInfo(false, 0, 0, 0, YEAGER_NULL_LITERAL);
  }

  const size_t lenght = size.x * size.y;

  auto pixels = std::make_unique<ImagePixelRGB[]>(lenght);

  ProcessAlignmentAndRead(pos.x, pos.y, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

  ProcessSwapAxisImage(pixels.get(), size.x, size.y, lenght);

  if (!stbi_write_jpg(output.c_str(), size.x, size.y, 3, pixels.get(), size.y)) {
    Yeager::Log(ERROR, "stbi_write_jpg cannot write to {}", output);
    pixels.release();
    return ProcessedImageInfo(false, 0, 0, 0, YEAGER_NULL_LITERAL);
  }

  pixels.release();
  return ProcessedImageInfo(true, size.x, size.y, lenght, output);
}