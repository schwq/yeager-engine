#include "Window.h"
Window::Window(yg_uint window_x, yg_uint window_y, GLFWerrorfun error,
               GLFWcursorposfun cursor)
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

  m_window = glfwCreateWindow(window_x, window_y, "Yeager Engine", NULL, NULL);
  if (m_window == NULL) {
    VLOG_F(ERROR, "Cannot created GLFW window!");
    glfwTerminate();
  }
  glfwSetErrorCallback(error);
  glfwSetCursorPosCallback(m_window, cursor);
  glfwMakeContextCurrent(m_window);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSwapInterval(1);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  kWindowX = width;
  kWindowY = height;

  glViewport(0, 0, width, height);
}

void Window::LaunchEditor()
{
  glfwDestroyWindow(m_window);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  m_window =
      glfwCreateWindow(mode->width, mode->height, "Yeager Engine", NULL, NULL);

  if (m_window == NULL) {
    VLOG_F(ERROR, "Cannot created GLFW window!");
    glfwTerminate();
  }
}

Window::~Window()
{
  glfwTerminate();
}