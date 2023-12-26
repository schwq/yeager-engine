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
#include "../../Common/Utilities.h"
#include "ShaderHandle.h"

struct AABBPositions {
  YgVector3 Max;
  YgVector3 Min;
  YgVector3 Center;
  YgVector3 Extents;
};

namespace Yeager {

class Object;

class AABBCollision {
 public:
  AABBCollision();
  void BuildCollision(Object* obj, std::vector<YgVector3>& vertices);
  YEAGER_NODISCARD std::pair<YgVector3, YgVector3> GetExtents(const std::vector<YgVector3>& vertices);
  YEAGER_NODISCARD bool CheckCollision(Object* obj1);
  constexpr void SetEnabled(bool enable) noexcept { m_Enabled = enable; }
  constexpr bool GetEnabled() const noexcept { return m_Enabled; }
  AABBPositions GetPositions() const noexcept { return m_AABB; }
  void Setup();
  void Draw(Shader* shader, YgMatrix4& model);
  constexpr bool GetIsColliding() const noexcept { return m_IsColliding; }
  constexpr void SetIsColliding(bool collision) noexcept { m_IsColliding = collision; }

 private:
  bool m_IsColliding = false;
  bool m_Enabled = true;
  GLsizei m_VerticeSize = 0;
  AABBPositions m_AABB;
  Object* m_ObjectCollidable = YEAGER_NULLPTR;
  GLuint m_Vao = 0, m_Vbo = 0;
};
}  // namespace Yeager