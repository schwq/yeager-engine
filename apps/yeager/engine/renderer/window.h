#pragma once

#include "../../common/common.h"
#include <GLFW/glfw3.h>

class Window {
  public:
    Window(uint window_x, uint window_y);
    ~Window();
    GLFWwindow *getWindow() { return m_window; }
    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

  private:
    GLFWwindow *m_window;
    uint m_window_x;
    uint m_window_y;
};
