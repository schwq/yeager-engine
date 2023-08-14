#pragma once

#include "../../common/common.h"
#include <GLFW/glfw3.h>

class Window {
 public:
  Window(uint window_x, uint window_y, GLFWerrorfun error,
         GLFWcursorposfun cursor);
  ~Window();
  GLFWwindow *getWindow() { return m_window; }
  static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);

 private:
  GLFWwindow *m_window = nullptr;
};
