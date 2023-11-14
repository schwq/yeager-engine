#include "Window.h"

Window::Window(unsigned int window_x, unsigned int window_y, YgString title, GLFWcursorposfun cursor)
{
  Yeager::Log(INFO, "Creating GLFW window!");
  if (!glfwInit()) {
    Yeager::Log(ERROR, "Cannot initialize glfw!");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef YEAGER_SYSTEM_MACOS
  glfwWindowHint(GLFW_OPENGL_FORWAND_COMPAT, GL_TRUE);
#endif

  GenerateWindow(window_x, window_y, title);

  glfwSetCursorPosCallback(m_window, cursor);
  glfwMakeContextCurrent(m_window);
  glfwSetErrorCallback(HandleError);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSwapInterval(1);
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
}

void Window::GetWindowSize(int* width, int* height)
{
  if (m_window) {
    glfwGetWindowSize(m_window, width, height);
  }
}