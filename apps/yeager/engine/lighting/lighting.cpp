#include "lighting.h"
using namespace ImGui;

uint LightingManager::m_lights_count = 0;

void LightingManager::AddLightSource(String name, Shader* render_shader,
                                     Shader* prop_shader, Application* app) {
  m_lights_count++;
  PointLightSource point(app, name, DEFAULT_PT_LIGHT, EngineShapeType::kCube,
                         prop_shader, render_shader);
  point.SetArrayPos(m_lights_count);
  m_point_lights.push_back(point);
}

void LightingManager::DrawLights() {
  for (uint x = 0; x < m_point_lights.size(); x++) {
    m_point_lights[x].SetPropToShader();
    m_point_lights[x].Draw();
  }
}
