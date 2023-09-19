#pragma once

#define MAX_LIGHTS 40

#include "../../application.h"
#include "../../common/common.h"
#include "lighting_source.h"

class Application;
class PointLightSource;
class Shader;

class LightingManager {
 public:
  LightingManager(){};
  ~LightingManager(){};
  void AddLightSource(String name, Shader* render_shader, Shader* prop_shader,
                      Application* app);
  void DrawLights();

 private:
  std::vector<PointLightSource> m_point_lights;
  static uint m_lights_count;
};
