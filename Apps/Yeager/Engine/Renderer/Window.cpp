#include "Window.h"

Window::Window(yg_uint window_x, yg_uint window_y, GLFWcursorposfun cursor)
{
  if (!glfwInit()) {
    VLOG_F(ERROR, "Cannot initialize glfw!");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWAND_COMPAT, GL_TRUE);
#endif
  /*
  m_primary_monitor = glfwGetPrimaryMonitor();
  m_vid_mode = glfwGetVideoMode(m_primary_monitor);
  m_window_monitor_x = m_vid_mode->width;
  m_window_monitor_y = m_vid_mode->height;
  */
  m_window = glfwCreateWindow(window_x, window_y, "Yeager Engine", NULL, NULL);
  if (m_window == NULL) {
    VLOG_F(ERROR, "Cannot created GLFW window!");
    glfwTerminate();
  }
  glfwSetErrorCallback(glfwErrorCallback);
  glfwSetCursorPosCallback(m_window, cursor);
  glfwMakeContextCurrent(m_window);
  glfwSetErrorCallback(HandleError);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSwapInterval(1);
}

void Window::HandleError(int code, const char* description)
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