//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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
#include "../../Common/Utilities.h"
#include "Entity.h"

#define YEAGER_MAX_DRAWN_LINES 100000

namespace Yeager {
class ApplicationCore;
class Shader;

struct LineData {
  Vector3 Vertex1 = YEAGER_ZERO_VECTOR3;
  Vector3 Vertex2 = YEAGER_ZERO_VECTOR3;
};

class Line {
 public:
  Line(const Vector3& p1, const Vector3& p2, const Vector3& position, const Vector3& rotation, const Vector3& color);
  ~Line();

  void Setup();
  void Draw(Shader* shader);

  void Delete();

  YEAGER_FORCE_INLINE bool IsCreated() const { return m_Created; }
  YEAGER_FORCE_INLINE bool CanRender() const { return m_Render; }
  YEAGER_FORCE_INLINE void SetCanRender(bool render) { m_Render = render; }

 protected:
  GLuint m_Vao = -1, m_Vbo = -1;
  LineData m_Data;
  Transformation m_Transformation;
  Vector3 m_P1 = YEAGER_ZERO_VECTOR3;
  Vector3 m_P2 = YEAGER_ZERO_VECTOR3;
  Vector3 m_Color = YEAGER_ZERO_VECTOR3;
  bool m_Created = false;
  bool m_Render = true;
};

class RendererLines {
 public:
  RendererLines(Yeager::ApplicationCore* application);
  ~RendererLines();

  void AddLine(const Yeager::Line& line);
  void AddLine(const Vector3& p1, const Vector3& p2, const Vector3& pos, const Vector3& rotation, const Vector3& color);
  void Draw(Shader* shader);

 protected:
  std::vector<Yeager::Line> m_Lines;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
};
}  // namespace Yeager
