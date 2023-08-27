#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../../input.h"
#include "imported_object.h"
#include "shader.h"
#include "shapes.h"
#include "texture.h"

class Application;

enum class RendererEngineName { kOpenGL = 0, kDirectX = 1, kVulkan = 2 };

/* TODO: make sure the program args and argv are taken to here for revision, user choices i mean*/
struct RendererEngineHandle {
  RendererEngineName name;
};

class RendererEngine {
 public:
  RendererEngine(RendererEngineName name, Application* app);
  ~RendererEngine(){};

  void Render();
  void checkGLAD();

 private:
  RendererEngineHandle m_handle;
  Application* m_app;

  void RendererOpenGL();
};
