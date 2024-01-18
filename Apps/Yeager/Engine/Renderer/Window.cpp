#include "Window.h"

#include "stb_image.h"

Window::Window(unsigned int window_x, unsigned int window_y, YgString title, GLFWcursorposfun cursor)
{
  Yeager::Log(INFO, "Creating GLFW window!");
  if (!glfwInit()) {
    Yeager::Log(ERROR, "Cannot initialize glfw!");
  }
  glfwSetErrorCallback(HandleError);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef YEAGER_SYSTEM_MACOS
  glfwWindowHint(GLFW_OPENGL_FORWAND_COMPAT, GL_TRUE);
#endif

  GenerateWindow(window_x, window_y, title);
  glfwSetCursorPosCallback(m_window, cursor);
  glfwMakeContextCurrent(m_window);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetWindowSizeCallback(m_window, ResizeCallback);
  glfwSetWindowSizeLimits(m_window, 700, 700, 2000, 2000);
  glfwSwapInterval(1);
}

bool Window::CheckIfOpenGLContext()
{
  return glfwGetCurrentContext();
}

bool Window::RegenerateMainWindow(unsigned int window_x, unsigned int window_y, YgString title,
                                  GLFWcursorposfun cursor) noexcept
{
  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);
    glfwSetCursorPosCallback(m_window, cursor);
    glfwMakeContextCurrent(m_window);
    glfwSetErrorCallback(HandleError);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(1);
    return true;
  } else {
    Yeager::Log(WARNING, "Window pointer does not exist!");
    return false;
  }
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
  ygWindowHeight = height;
  ygWindowWidth = width;
  Yeager::Log(INFO, "Window resized to {} {}", width, height);
}

bool Window::GenerateWindow(unsigned window_x, unsigned int window_y, YgString title)
{

  m_window = glfwCreateWindow(window_x, window_y, title.c_str(), NULL, NULL);

  if (!m_window) {
    Yeager::Log(ERROR, "Cannot Generate GLFW Window!");
    glfwTerminate();
    return false;
  }
  return true;
}

void Window::HandleError(int code, YgCchar description)
{
  Yeager::Log(ERROR, "GLFW window error, message {}, code {}", description, code);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  ygWindowWidth = width;
  ygWindowHeight = height;

  glViewport(0, 0, width, height);
}

Window::~Window()
{
  Yeager::Log(INFO, "Destrorying Window!");
  delete m_IconImageData;
}

void Window::GetWindowSize(int* width, int* height)
{
  if (m_window) {
    glfwGetWindowSize(m_window, width, height);
  }
}
