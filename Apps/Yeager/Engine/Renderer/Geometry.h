//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../Editor/ToolboxObj.h"
#include "../Physics/PhysicsHandle.h"
#include "Entity.h"

namespace Yeager {
class ApplicationCore;
}

namespace Yeager {

enum GeometryShape { kCube, kSphere, kTriangle };
extern YgString ShapeToString(GeometryShape shape);
class Geometry : public GameEntity {
 public:
  Geometry(YgString name, YgVector3 color, GeometryShape shape, Yeager::ApplicationCore* app, bool is_color = true,
           Yeager::Texture2D* texture = nullptr);
  ~Geometry();

  void Draw(Shader* shader);
  void Setup();
  void ChangeColor(YgVector3 color) { m_color = color; }
  GeometryShape GetShape() { return m_shape; }
  YgVector3 GetColor() { return m_color; }
  bool isColor() { return m_is_color; }
  Yeager::Texture2D* GetTexture() { return m_texture; }

 private:
  GeometryShape m_shape;
  Yeager::ApplicationCore* m_app;
  Yeager::Texture2D* m_texture;
  YgVector3 m_color;
  bool m_is_color = true;
  std::vector<GLfloat> m_vertices;
  std::vector<GLuint> m_indices;
  GLuint m_vao, m_vbo, m_ebo;
  Yeager::EntityPhysics m_physics;

  std::vector<GLfloat> GenerateCubeVertices();
  std::vector<GLuint> GenerateCubeIndices();
  void GenerateVerticesIndices();
};
}  // namespace Yeager
