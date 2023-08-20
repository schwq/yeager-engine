#pragma once

#include "../../common/common.h"
#include "../../application.h"
#include "../../input.h"
#include "texture.h"
#include "shader.h"
#include "shapes.h"

class Application;

enum class RendererEngineName { kOpenGL = 0, kDirectX = 1, kVulkan = 2 };

/* TODO: make sure the program args and argv are taken to here for revision, user choices i mean*/
struct RendererEngineHandle {
  RendererEngineName name;
};

class RendererEngine {
 public:
  RendererEngine(RendererEngineName name, Application *app);
  ~RendererEngine(){};

  void Render();
  void checkGLAD();

 private:
  RendererEngineHandle m_handle;
  Application *m_app;

  void RendererOpenGL();
};
