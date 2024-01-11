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


namespace Yeager {

<<<<<<< Updated upstream
typedef struct {
  yg_mat4 model;
  yg_vec3 position;
  yg_vec3 rotation;
  yg_vec3 scale;
} Transformation;

extern constexpr Transformation GetDefaultTransformation();
=======
class ApplicationCore;
struct ObjectTexture;

typedef struct {
  YgMatrix4 model = YgMatrix4(1.0);
  YgVector3 position = YgVector3(0.0);
  YgVector3 rotation = YgVector3(0.0);
  YgVector3 scale = YgVector3(1.0);
} Transformation;

extern Transformation GetDefaultTransformation();
extern void ProcessTransformation(Transformation* trans);

class Shader;
class Texture2D;

enum EntityObjectType {
    EObject,
    EObjectAnimated,
    EObjectInstanced,
    EObjectInstancedAnimated,
    EShader,
    EAudioHandle,
    EAudio3DHandle,
    ENull
};
>>>>>>> Stashed changes

class Entity {
 public:
  Entity(yg_string name = YEAGER_NULL_LITERAL);
  ~Entity();

  yg_string GetName();
  yg_uint GetId();

  void SetEntityType(EntityObjectType type) { m_Type = type; }
  EntityObjectType GetEntityType() { return m_Type; }

 protected:
<<<<<<< Updated upstream
  yg_string m_name;
  const yg_uint m_id;
  static yg_uint m_entityCountId;
=======
  EntityObjectType m_Type = ENull;
  YgString m_Name = YEAGER_NULL_LITERAL;
  bool m_Render = true;
  const unsigned int m_id;
  static unsigned int m_entityCountId;
>>>>>>> Stashed changes
};

class GameEntity : public Entity {
 public:
<<<<<<< Updated upstream
  GameEntity(yg_string name = YEAGER_NULL_LITERAL, Yeager::Texture2D* texture = nullptr,
             Yeager::Shader* shader = nullptr);
=======
  GameEntity(YgString name = YEAGER_NULL_LITERAL, ApplicationCore* app = YEAGER_NULLPTR);
>>>>>>> Stashed changes
  ~GameEntity();
  constexpr Yeager::Texture2D* GetTexture();
  constexpr Yeager::Shader* GetShader();
  Transformation GetTransformation();
  Transformation* GetTransformationPtr();
<<<<<<< Updated upstream
  void SetPosition(yg_vec3 pos);
  void ProcessTransformation(Shader* Shader);

 protected:
  Transformation m_transformation;
  Yeager::Texture2D* m_texture;
  Yeager::Shader* m_shader;
=======
  virtual void ProcessTransformation(Shader* Shader);
  virtual void ProcessTransformationCollision(Shader* shader, AABBCollision* col);
  void inline SetTransformation(const Transformation& trans) { m_EntityTransformation = trans; }

  constexpr inline std::vector<ObjectTexture*>* GetLoadedTextures() { return &m_EntityLoadedTextures; };
  constexpr AABBCollision* GetCollision() noexcept { return &m_Collision; }

  /**
   * @brief This is used by the verify collision loop, is the collision have been already set to true during the frame, it wont be set to false when comparing to other object that isnt make collision
   * @param collision Boolean if the collision is happening
   */
  void MakeCollision(bool collision)
  {
    if (m_Collision.GetIsColliding()) {
      return;
    }
    m_Collision.SetIsColliding(collision);
  }

  /**
   * @brief This force the collision to be set to another state, used in the start of the frame
   * @param collision Boolean if the collision is happening
   */
  void ForceCollision(bool collision) { m_Collision.SetIsColliding(collision); }

 protected:
  AABBCollision m_Collision;
  std::vector<ObjectTexture*> m_EntityLoadedTextures;
  Transformation m_EntityTransformation;
  ApplicationCore* m_Application = YEAGER_NULLPTR;
>>>>>>> Stashed changes
};
/* 
class EditorEntity : public Entity {
 public:
 protected:
};
*/
}  // namespace Yeager