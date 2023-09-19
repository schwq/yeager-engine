#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../editor/toolbox_object.h"
#include "shader.h"
#include "texture.h"
#include "transformation.h"

class YeagerTexture2D;
class Application;
class ToolBoxObject;

enum class EngineShapeType { kCube, kSphere, kCone };

class YeagerShape {
 public:
  YeagerShape(String name, Application* app, YeagerTexture2D texture,
              EngineShapeType type);
  YeagerShape(String name, Application* app, String texturePath,
              EngineShapeType type);
  YeagerShape() {}
  ~YeagerShape();

  void Draw(Shader* shader);
  constexpr Transformation* Transform() { return &m_trans; }

 private:
  void Setup();
  void ProcessTransformation(Shader* shader);
  void ProcessShapeType();
  String m_name = YEAGER_NULL_LITERAL;
  EngineShapeType m_type;
  uint m_vbo = 0, m_vao = 0, m_ebo = 0;
  Application* m_app = nullptr;
  YeagerTexture2D m_texture;
  Transformation m_trans;
  ToolBoxObject* m_toolbox = nullptr;
  std::vector<GLfloat> m_vertices;
  std::vector<uint> m_indices;
};

// clang-format off
extern std::vector<GLfloat> GetCubeVertices();
extern std::vector<uint> GetCubeIndices();
// clang-format on
