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

struct ObjectTexture;

typedef struct {
  YgMatrix4 model;
  YgVector3 position;
  YgVector3 rotation;
  YgVector3 scale;
} Transformation;

extern Transformation GetDefaultTransformation();

class Shader;

class Entity {
 public:
  Entity(YgString name = YEAGER_NULL_LITERAL);
  ~Entity();

  YgString GetName();

  unsigned int GetId();
  constexpr inline void SetRender(bool render) { m_Render = render; }
  constexpr inline bool* GetRender() { return &m_Render; }

 protected:
  YgString m_Name;
  bool m_Render = true;
  const unsigned int m_id;
  static unsigned int m_entityCountId;
};

class GameEntity : public Entity {
 public:
  GameEntity(YgString name = YEAGER_NULL_LITERAL);
  ~GameEntity();

  Transformation GetTransformation();
  Transformation* GetTransformationPtr();
  virtual void ProcessTransformation(Shader* Shader);

  constexpr inline std::vector<ObjectTexture*>* GetLoadedTextures() { return &m_EntityLoadedTextures; };

 protected:
  std::vector<ObjectTexture*> m_EntityLoadedTextures;
  Transformation m_EntityTransformation;
};

class DrawableEntity : public GameEntity {
 public:
  DrawableEntity(YgString name = YEAGER_NULL_LITERAL);
  ~DrawableEntity();

  virtual void Draw(Shader* shader);

  void Terminate();

 protected:
  GLuint m_Ebo = 0, m_Vao = 0, m_Vbo = 0;
};

}  // namespace Yeager