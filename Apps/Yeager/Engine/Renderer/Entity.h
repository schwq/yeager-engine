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
#include "ShaderHandle.h"
#include "TextureHandle.h"

namespace Yeager {

typedef struct {
  yg_mat4 model;
  yg_vec3 position;
  yg_vec3 rotation;
  yg_vec3 scale;
} Transformation;

extern constexpr Transformation GetDefaultTransformation();

class Entity {
 public:
  Entity(yg_string name = YEAGER_NULL_LITERAL);
  ~Entity();

  yg_string GetName();
  yg_uint GetId();

 protected:
  yg_string m_name;
  const yg_uint m_id;
  static yg_uint m_entityCountId;
};

class GameEntity : public Entity {
 public:
  GameEntity(yg_string name = YEAGER_NULL_LITERAL, Yeager::Texture2D* texture = nullptr,
             Yeager::Shader* shader = nullptr);
  ~GameEntity();
  constexpr Yeager::Texture2D* GetTexture();
  constexpr Yeager::Shader* GetShader();
  Transformation GetTransformation();
  Transformation* GetTransformationPtr();
  void SetPosition(yg_vec3 pos);
  void ProcessTransformation(Shader* Shader);

 protected:
  Transformation m_transformation;
  Yeager::Texture2D* m_texture;
  Yeager::Shader* m_shader;
};
/* 
class EditorEntity : public Entity {
 public:
 protected:
};
*/
}  // namespace Yeager