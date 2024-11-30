#include "Window.h"
#include "Main/Core/Application.h"
#include "Main/Settings/Settings.h"
#include "stb_image.h"
using namespace Yeager;

Yeager::WindowInfo Window::sWindowInformation;
const std::vector<uint> Window::sAcceptableSampleValues = {1, 2, 4, 8};

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
  switch (Yeager::StringToInterger(str.c_str())) {
    case Yeager::StringToInterger("4:3"):
      return eASPECT_4_3;
    case Yeager::StringToInterger("21:9"):
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

void Window::InitializeCallbacks(GLFWcursorposfun cursor)
{
  glfwSetWindowSizeCallback(mWindowHandle, ResizeCallback);
  glfwSetWindowPosCallback(mWindowHandle, WindowPosCallback);
  glfwSetWindowMaximizeCallback(mWindowHandle, WindowMaximizeCallback);
  glfwSetFramebufferSizeCallback(mWindowHandle, FramebufferSizeCallback);
  glfwSetCursorPosCallback(mWindowHandle, cursor);
  glfwSetErrorCallback(HandleError);
}

void Window::GenerateWindow(String title, GLFWcursorposfun cursor, DFlags wndType)
{
  Yeager::Log(INFO, "Creating GLFW window!");
  if (!glfwInit()) {
    Yeager::Log(ERROR, "Cannot initialize glfw!");
    PanicCrashReport(std::runtime_error("Cannot initialize GLFW!"));
  }

  sWindowInformation.mEditorTitle = title;
  sWindowInformation.mCursorFunc = cursor;

  const Vector2 size =
      (wndType == YEAGER_GENERATE_EDITOR_WINDOW ? sWindowInformation.mEditorSize : sWindowInformation.mLauncherSize);

  BuildWindowHints();
  GenerateWindow(UVector2(size.x, size.y), title);

  InitializeCallbacks(cursor);

  glfwMakeContextCurrent(mWindowHandle);
  glfwSetInputMode(mWindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetWindowSizeLimits(mWindowHandle, 700, 700, 2000, 2000);
  glfwSetWindowUserPointer(mWindowHandle, mApplication);
  glfwSetWindowPos(mWindowHandle, sWindowInformation.mWindowPosition.x, sWindowInformation.mWindowPosition.y);

  glfwSwapInterval(1);
}

Window::Window(Yeager::ApplicationCore* application) : mApplication(application) {}

bool Window::CheckIfOpenGLContext()
{
  return glfwGetCurrentContext();
}

void Window::BuildWindowHints()
{
  glfwSetErrorCallback(HandleError);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, mWindowHints.mContextVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, mWindowHints.mContextVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, mWindowHints.mOpenGLProfile);
  glfwWindowHint(GLFW_SAMPLES, mWindowHints.mAntiAliasingSamples);
#ifdef YEAGER_SYSTEM_MACOS
  glfwWindowHint(GLFW_OPENGL_FORWAND_COMPAT, m_WindowHints.mOpenGLForwandCompat);
#endif

#ifdef DEBUG_GLFW_GL_CONTEXT
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
}

bool Window::RegenerateMainWindow(const UVector2& size, const String& title) noexcept
{
  if (mWindowHandle) {
    sWindowInformation.mEditorTitle = title;
    sWindowInformation.mEditorSize = Vector2(size.x, size.y);

    glfwSetWindowTitle(mWindowHandle, title.c_str());
    glfwSetWindowSize(mWindowHandle, size.x, size.y);

    glfwSetWindowPos(mWindowHandle, sWindowInformation.mWindowPosition.x, sWindowInformation.mWindowPosition.y);

    return true;
  }

  Yeager::Log(WARNING, "Window pointer does not exist!");
  return false;
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));
  if (application->GetMode() == ApplicationMode::eAPPLICATION_LAUNCHER) {
    sWindowInformation.mLauncherSize = Vector2(width, height);
  } else {
    sWindowInformation.mEditorSize = Vector2(width, height);
  }
}

void Window::WindowMaximizeCallback(GLFWwindow* window, int maximized)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  application->GetMode() == ApplicationMode::eAPPLICATION_LAUNCHER
      ? sWindowInformation.mLauncherSize = Vector2(width, height)
      : sWindowInformation.mEditorSize = Vector2(width, height);

  if (maximized) {
    Yeager::LogDebug(INFO, "GLFW window was maximized");
  } else {
    Yeager::LogDebug(INFO, "GLFW window was restored");
  }
}

bool Window::GenerateWindow(const UVector2& size, const String& title)
{
  mWindowHandle = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);

  if (!mWindowHandle) {
    Yeager::Log(ERROR, "Cannot Generate GLFW Window!");
    PanicCrashReport(std::runtime_error("Cannot Generate GLFW Window!"));
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
    sWindowInformation.mFrameBufferPosition = Vector2(lowerLeftCornerOfViewportX, lowerLeftCornerOfViewportY);
    sWindowInformation.mFrameBufferSize = Vector2(widthOfViewport, heightOfViewport);
  } else {
    glViewport(0, 0, width, height);
    sWindowInformation.mFrameBufferPosition = Vector2(0, 0);
    sWindowInformation.mFrameBufferSize = Vector2(width, height);
  }
}

Window::~Window()
{
  if (!bTerminated)
    Terminate();
}

Vector2 Window::GetWindowSize() const
{
  return (mApplication->GetMode() == ApplicationMode::eAPPLICATION_EDITOR ? sWindowInformation.mEditorSize
                                                                          : sWindowInformation.mLauncherSize);
}

bool Window::ChangeAntiAliasingSamples(int samples)
{
  if (!mWindowHandle)
    return false;

  /* Only x1, x2, x4, and x8 anti alising */
  if (!ValidateAntiAliasingSampleValue(samples))
    return false;

  mWindowHints.mAntiAliasingSamples = samples;
  return true;
}

void Window::SetWindowPos(const IVector2& pos)
{
  if (mWindowHandle) {
    glfwSetWindowPos(mWindowHandle, pos.x, pos.y);
  }
}

IVector2 Window::GetWindowPos() const
{
  if (mWindowHandle) {
    int x, y;
    glfwGetWindowPos(mWindowHandle, &x, &y);
    return IVector2(x, y);
  }
  return IVector2(0, 0);
}

void Window::WindowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
  Yeager::ApplicationCore* application = static_cast<Yeager::ApplicationCore*>(glfwGetWindowUserPointer(window));
  Yeager::WindowInfo* wnd = application->GetWindow()->GetWindowInformationPtr();
  wnd->mWindowPosition = IVector2(xpos, ypos);
}

void Window::Terminate()
{
  if (!bTerminated) {
    glfwTerminate();
    Yeager::LogDebug(INFO, "Destrorying Window!");
    bTerminated = true;
  }
}
YEAGER_FORCE_INLINE bool Window::ValidateAntiAliasingSampleValue(int samples) const
{
  return std::find(sAcceptableSampleValues.begin(), sAcceptableSampleValues.end(), samples) !=
         sAcceptableSampleValues.end();
}
