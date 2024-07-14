#include "Window.h"
#include "../../Application.h"
#include "../../Settings.h"
#include "stb_image.h"
using namespace Yeager;

Yeager::WindowInfo Window::m_WindowInformation;

String AspectRatio::ToString(AspectRatio::Enum type)
{
  switch (type) {
    case eASPECT_21_9:
      return "21:9";
    case eASPECT_4_3:
      return "4:3";
    default:
      return "NO_ASPECT";
  }
}

AspectRatio::Enum AspectRatio::ToEnum(const String& str)
{
  switch (Yeager::StringToInteger(str.c_str())) {
    case Yeager::StringToInteger("4:3"):
      return eASPECT_4_3;
    case Yeager::StringToInteger("21:9"):
      return eASPECT_21_9;
    default:
      return eASPECT_4_3;
  }
}

float AspectRatio::ToValue(AspectRatio::Enum type)
{
  switch (type) {
    case eASPECT_21_9:
      return 21 / 9;
    case eASPECT_4_3:
      return 4 / 3;
    default:
      return 4 / 3;
  }
}

void Window::GenerateWindow(String title, GLFWcursorposfun cursor, DFlags wndType)
{
  Yeager::Log(INFO, "Creating GLFW window!");
  if (!glfwInit()) {
    Yeager::Log(ERROR, "Cannot initialize glfw!");
  }

  m_WindowInformation.EditorTitle = title;
  m_WindowInformation.CursorFunc = cursor;

  const Vector2 size =
      (wndType == YEAGER_GENERATE_EDITOR_WINDOW ? m_WindowInformation.EditorSize : m_WindowInformation.LauncherSize);

  BuildWindowHints();
  GenerateWindow(size.x, size.y, title);
  glfwSetCursorPosCallback(m_WindowHandle, cursor);
  glfwMakeContextCurrent(m_WindowHandle);
  glfwSetFramebufferSizeCallback(m_WindowHandle, FramebufferSizeCallback);
  glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetWindowSizeCallback(m_WindowHandle, ResizeCallback);
  glfwSetWindowMaximizeCallback(m_WindowHandle, WindowMaximizeCallback);
  glfwSetWindowSizeLimits(m_WindowHandle, 700, 700, 2000, 2000);
  glfwSetWindowUserPointer(m_WindowHandle, m_Application);
  glfwSwapInterval(1);
}

Window::Window(Yeager::ApplicationCore* application) : m_Application(application) {}

bool Window::CheckIfOpenGLContext()
{
  return glfwGetCurrentContext();
}

void Window::BuildWindowHints()
{
  glfwSetErrorCallback(HandleError);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_WindowHints.ContextVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_WindowHints.ContextVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, m_WindowHints.OpenGLProfile);
  glfwWindowHint(GLFW_SAMPLES, m_WindowHints.AntiAliasingSamples);
#ifdef YEAGER_SYSTEM_MACOS
  glfwWindowHint(GLFW_OPENGL_FORWAND_COMPAT, m_WindowHints.OpenGLForwandCompat);
#endif
}

bool Window::RegenerateMainWindow(Uint window_x, Uint window_y, String title, GLFWcursorposfun cursor) noexcept
{
  if (m_WindowHandle) {
    glfwDestroyWindow(m_WindowHandle);
    m_WindowInformation.EditorTitle = title;
    m_WindowInformation.EditorSize = Vector2(window_x, window_y);
    BuildWindowHints();
    m_WindowHandle = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);
    glfwSetCursorPosCallback(m_WindowHandle, cursor);
    glfwMakeContextCurrent(m_WindowHandle);
    glfwSetErrorCallback(HandleError);
    glfwSetWindowMaximizeCallback(m_WindowHandle, WindowMaximizeCallback);
    glfwSetFramebufferSizeCallback(m_WindowHandle, FramebufferSizeCallback);
    glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_WindowHandle, m_Application);
    return true;
  } else {
    Yeager::Log(WARNING, "Window pointer does not exist!");
    return false;
  }
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));
  if (application->GetMode() == ApplicationMode::eAPPLICATION_LAUNCHER) {
    m_WindowInformation.LauncherSize = Vector2(width, height);
  } else {
    m_WindowInformation.EditorSize = Vector2(width, height);
  }
}

void Window::WindowMaximizeCallback(GLFWwindow* window, int maximized)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));

  int width, height;
  glfwGetWindowSize(window, &width, &height);
  if (application->GetMode() == ApplicationMode::eAPPLICATION_LAUNCHER) {
    m_WindowInformation.LauncherSize = Vector2(width, height);
  } else {
    m_WindowInformation.EditorSize = Vector2(width, height);
  }

  if (maximized) {
    Yeager::LogDebug(INFO, "GLFW window was maximized");
  } else {
    Yeager::LogDebug(INFO, "GLFW window was restored");
  }
}

bool Window::GenerateWindow(unsigned window_x, Uint window_y, String title)
{
  void* previous = m_WindowHandle;
  m_WindowHandle = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);
  Yeager::LogDebug(INFO, "GLFW window pointer changes from ({}) to ({})", fmt::ptr(previous), fmt::ptr(m_WindowHandle));

  if (!m_WindowHandle) {
    Yeager::Log(ERROR, "Cannot Generate GLFW Window!");
    glfwTerminate();
    return false;
  }
  return true;
}

void Window::HandleError(int code, Cchar description)
{
  Yeager::Log(ERROR, "GLFW window error, message {}, code {}", description, code);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));
  Yeager::Settings* settings = application->GetSettings();

  if (settings->GetVideoSettingsStruct().WindowObeysAspectRatio) {

    float aspectRatio = AspectRatio::ToValue(settings->GetVideoSettingsStruct().WindowDesireAspectRatio);
    int widthOfViewport, heightOfViewport;
    // These are two new values that we will be calculating in this function
    int lowerLeftCornerOfViewportX, lowerLeftCornerOfViewportY;

    float requiredHeightOfViewport = width * (1.0f / aspectRatio);
    if (requiredHeightOfViewport > height) {
      float requiredWidthOfViewport = height * aspectRatio;
      if (requiredWidthOfViewport > width) {
        Yeager::LogDebug(ERROR, "Cannot find dimensions that preserve the aspect ratio");
      } else {
        // Remember that if we reach this point you will observe vertical bars
        // on the left and right
        widthOfViewport = static_cast<int>(requiredWidthOfViewport);
        heightOfViewport = height;

        // The widths of the two vertical bars added together are equal to the
        // difference between the width of the framebuffer and the width of the viewport
        float widthOfTheTwoVerticalBars = width - widthOfViewport;

        // Set the X position of the lower left corner of the viewport equal to the
        // width of one of the vertical bars. By doing this, we center the viewport
        // horizontally and we make vertical bars appear on the left and right
        lowerLeftCornerOfViewportX = static_cast<int>(widthOfTheTwoVerticalBars / 2.0f);
        // We don't need to center the viewport vertically because we are using the
        // height of the framebuffer as the height of the viewport
        lowerLeftCornerOfViewportY = 0;
      }
    } else {
      // Remember that if we reach this point you will observe horizontal bars
      // on the top and bottom
      widthOfViewport = width;
      heightOfViewport = static_cast<int>(requiredHeightOfViewport);

      // The heights of the two horizontal bars added together are equal to the difference
      // between the height of the framebuffer and the height of the viewport
      float heightOfTheTwoHorizontalBars = height - heightOfViewport;

      // We don't need to center the viewport horizontally because we are using the
      // width of the framebuffer as the width of the viewport
      lowerLeftCornerOfViewportX = 0;
      // Set the Y position of the lower left corner of the viewport equal to the
      // height of one of the vertical bars. By doing this, we center the viewport
      // vertically and we make horizontal bars appear on the top and bottom
      lowerLeftCornerOfViewportY = static_cast<int>(heightOfTheTwoHorizontalBars / 2.0f);
    }

    // Call glViewport to specify the new drawing area
    // By specifying its lower left corner, we center it
    glViewport(lowerLeftCornerOfViewportX, lowerLeftCornerOfViewportY, widthOfViewport, heightOfViewport);
    m_WindowInformation.FrameBufferPosition = Vector2(lowerLeftCornerOfViewportX, lowerLeftCornerOfViewportY);
    m_WindowInformation.FrameBufferSize = Vector2(widthOfViewport, heightOfViewport);
  } else {
    glViewport(0, 0, width, height);
    m_WindowInformation.FrameBufferPosition = Vector2(0, 0);
    m_WindowInformation.FrameBufferSize = Vector2(width, height);
  }
}

Window::~Window()
{
  Yeager::Log(INFO, "Destrorying Window!");
  delete m_IconImageData;
}

Vector2 Window::GetWindowSize() const
{
  return (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR ? m_WindowInformation.EditorSize
                                                                           : m_WindowInformation.LauncherSize);
}

void Window::GetWindowSize(int* width, int* height)
{
  if (m_WindowHandle) {
    glfwGetWindowSize(m_WindowHandle, width, height);
  }
}

bool Window::ChangeAntiAliasingSamples(int samples)
{
  if (!m_WindowHandle)
    return false;

  /* Only x1, x2, x4, and x8 anti alising */
  if (!ValidateAntiAliasingSampleValue(samples))
    return false;

  m_WindowHints.AntiAliasingSamples = samples;
  return true;
}
