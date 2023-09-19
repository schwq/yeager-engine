#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../editor/toolbox_object.h"
#include "../renderer/shader.h"
#include "../renderer/shapes.h"

class Application;
enum class EngineShapeType;
class ToolBoxObject;
class Transformation;
class YeagerShape;
class YeagerTexture2D;
class Shader;

struct PointLight {
  Vector3 m_position;
  float m_constant;
  float m_linear;
  float m_quadratic;
  Vector3 m_ambient;
  Vector3 m_diffuse;
  Vector3 m_specular;
  bool m_active;
};

#define DEFAULT_PT_LIGHT                                              \
  PointLight {                                                        \
    Vector3(0.0f), 1.0f, 0.09f, 0.032f, Vector3(0.05f, 0.05f, 0.05f), \
        Vector3(0.8f, 0.8f, 0.8f), Vector3(1.0f, 1.0f, 1.0f), true    \
  }

class PointLightSource {
 public:
  PointLightSource(Application* app, String name, PointLight prop,
                   EngineShapeType shape, Shader* prop_shader,
                   Shader* shape_shader);
  ~PointLightSource();
  Transformation* Transform();
  PointLight* GetProp() { return &m_prop; }
  void Draw();
  void SetPropToShader();
  void SetArrayPos(uint pos) { m_lights_array_pos = pos; }

 private:
  void Setup();

  Shader* m_prop_shader;
  Shader* m_shape_shader;

  uint m_lights_array_pos;
  YeagerShape* yeager_shape = nullptr;
  Application* m_app;
  String m_name;
  EngineShapeType m_shape;
  ToolBoxObject* m_toolbox_object = nullptr;
  PointLight m_prop = DEFAULT_PT_LIGHT;
};
