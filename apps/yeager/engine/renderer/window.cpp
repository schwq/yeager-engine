#include "window.h"
#include <GLFW/glfw3.h>

Window::Window(uint window_x, uint window_y) {
    glfwInit();
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

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
}

void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::~Window() { glfwTerminate(); }