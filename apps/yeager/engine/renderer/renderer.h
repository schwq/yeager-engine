#pragma once

#include "../../common/common.h"
#include "../../application.h"
#include "../../input.h"

class Application;

enum class RendererEngineName { kOpenGL, kDirectX, kVulkan };

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
