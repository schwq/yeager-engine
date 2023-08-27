#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../editor/toolbox_object.h"
#include "shader.h"
#include "texture.h"

class EngineTexture2D;
class Application;
class ToolBoxObject;

class YeagerCube {
 public:
  YeagerCube(String name, Application* app, EngineTexture2D* texture = nullptr,
             const Vector3 position = Vector3(0.0f));
  ~YeagerCube();
  void Draw(Shader* shader);
  constexpr void SetPosition(Vector3 position) { m_position = position; }
  constexpr Vector3 GetPosition() { return m_position; }
  constexpr void SetRotation(Vector3 rotation) { m_rotation = rotation; }
  constexpr Vector3 GetRotation() { return m_rotation; }
  constexpr void SetScale(Vector3 scale) { m_scale = scale; }
  constexpr Vector3 GetScale() { return m_scale; }

 private:
  void Setup();
  void ProcessTransformation(Shader* shader);

  String m_name;
  Vector3 m_position = Vector3(1.0f);
  Vector3 m_rotation = Vector3(0.0f);
  Vector3 m_scale = Vector3(1.0f);
  EngineTexture2D* m_texture = nullptr;
  uint m_vbo, m_vao;
  Application* m_app;
  ToolBoxObject* m_toolbox = nullptr;

  // clang-format off
  float vertices[288] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 
      0.5f,  -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 
      0.5f,  -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 
      0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 
      -0.5f, -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
      -0.5f, 0.5f,  -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 
      -0.5f, 0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

      0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
      0.5f,  0.5f,  -0.5f,1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f,1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
      0.5f,  -0.5f, -0.5f,1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 
      0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 
      0.5f,  -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,   0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
      0.5f,  -0.5f, 0.5f,   0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,   0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f,0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 
      0.5f,  0.5f,  -0.5f,0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
      0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 
      -0.5f, 0.5f,  -0.5f,0.0f,  1.0f,  0.0f, 0.0f, 1.0f
      };
};
// clang-format on
