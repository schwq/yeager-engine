#include "Window.h"
#include "stb_image.h"
using namespace Yeager;

Window::Window(unsigned int window_x, unsigned int window_y, String title, GLFWcursorposfun cursor)
{
  Yeager::Log(INFO, "Creating GLFW window!");
  if (!glfwInit()) {
    Yeager::Log(ERROR, "Cannot initialize glfw!");
  }
  m_WindowInformation.Title = title;
  m_WindowInformation.CursorFunc = cursor;
  m_WindowInformation.Size = Vector2(window_x, window_y);
  BuildWindowHints();
  GenerateWindow(window_x, window_y, title);
  glfwSetCursorPosCallback(m_WindowHandle, cursor);
  glfwMakeContextCurrent(m_WindowHandle);
  glfwSetFramebufferSizeCallback(m_WindowHandle, FramebufferSizeCallback);
  glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetWindowSizeCallback(m_WindowHandle, ResizeCallback);
  glfwSetWindowSizeLimits(m_WindowHandle, 700, 700, 2000, 2000);
  glfwSwapInterval(1);
}

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

bool Window::RegenerateMainWindow(unsigned int window_x, unsigned int window_y, String title,
                                  GLFWcursorposfun cursor) noexcept
{
  if (m_WindowHandle) {
    glfwDestroyWindow(m_WindowHandle);
    m_WindowInformation.Title = title;
    m_WindowInformation.Size = Vector2(window_x, window_y);
    BuildWindowHints();
    m_WindowHandle = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);
    glfwSetCursorPosCallback(m_WindowHandle, cursor);
    glfwMakeContextCurrent(m_WindowHandle);
    glfwSetErrorCallback(HandleError);
    glfwSetFramebufferSizeCallback(m_WindowHandle, FramebufferSizeCallback);
    glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(1);
    return true;
  } else {
    Yeager::Log(WARNING, "Window pointer does not exist!");
    return false;
  }
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
  Yeager::ygWindowHeight = height;
  Yeager::ygWindowWidth = width;
  Yeager::Log(INFO, "Window resized to {} {}", width, height);
}

bool Window::GenerateWindow(unsigned window_x, unsigned int window_y, String title)
{

  m_WindowHandle = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);

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
  Yeager::ygWindowWidth = width;
  Yeager::ygWindowHeight = height;

  glViewport(0, 0, width, height);
}

Window::~Window()
{
  Yeager::Log(INFO, "Destrorying Window!");
  delete m_IconImageData;
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
