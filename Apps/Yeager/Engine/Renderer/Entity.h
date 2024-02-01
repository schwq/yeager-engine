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

namespace Yeager {

class ApplicationCore;
class Shader;
class Texture2D;
class ToolBoxObject;
struct ObjectTexture;

typedef struct {
  YgMatrix4 model = YgMatrix4(1.0);
  YgVector3 position = YgVector3(0.0);
  YgVector3 rotation = YgVector3(0.0);
  YgVector3 scale = YgVector3(1.0);
} Transformation;

extern Transformation GetDefaultTransformation();
extern void ProcessTransformation(Transformation* trans);

struct EntityObjectType {
  enum Enum {
    eOBJECT,
    eOBJECT_ANIMATED,
    eOBJECT_INSTANCED,
    eOBJECT_INSTANCED_ANIMATED,
    eAUDIO_HANDLE,
    eAUDIO_3D_HANDLE,
    eTEXTURE,
    eLIGHT_HANDLE,
    eSKYBOX,
    eCAMERA,
    eNULL
  };
};
/**
 * @brief Entities are the most basic type of class that almost all classes in this engine inherit, it contains variables that all classes share in common
 * like the name and global id, also it can be used to set a object to non serialization status, where the object doesnt get saved in the configuration file.
 * Entity object type are enums that can be used to modify simple entity pointer to a more complex pointer, like a AnimatedObject for example.
 */
class Entity {
 public:
  Entity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name = YEAGER_NULL_LITERAL);
  ~Entity();

  YgString GetName();
  unsigned int GetId();

  void SetEntityType(EntityObjectType::Enum type) { m_Type = type; }
  EntityObjectType::Enum GetEntityType() const { return m_Type; }

  constexpr bool IsValid() const { return m_Valid; }

  YEAGER_FORCE_INLINE bool CanBeSerialize() const { return m_CanBeSerialize; }
  YEAGER_FORCE_INLINE void SetCanBeSerialize(bool serial) { m_CanBeSerialize = serial; }

 protected:
  EntityObjectType::Enum m_Type = EntityObjectType::eNULL;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  YgString m_Name = YEAGER_NULL_LITERAL;
  bool m_Render = true;
  bool m_Valid = false;
  bool m_CanBeSerialize = true;
  const unsigned int m_id;
  static unsigned int m_entityCountId;
};

/**
 * @brief Editor entity is a entity that show proprieties on the editor, like having a toolbox selecteable and having 
 * events that track the status of this entity to been show on the editor screen, like a alert for critical problems or needing to reload or 
 * visualize it, like a texture. This class has made mainly from two other classes (shader and texture) that exists in the engine context, but
 * dont have the need to have a collision or physic tracker, like the game entity
 */
class EditorEntity : public Entity {
 public:
  EditorEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name = YEAGER_NULL_LITERAL);
  ~EditorEntity();

  void SetToolbox(const std::shared_ptr<ToolBoxObject>& toolbox) { m_Toolbox = toolbox; }
  std::shared_ptr<ToolBoxObject> GetToolbox() { return m_Toolbox; }

  /**
   * @brief Sets to true, when the scene calls for search for schedule deletions, it will remove this entity from the 
   * application, basically, delete it 
  */
  constexpr void SetScheduleDeletion(bool deletion) { m_ScheduleDeletion = deletion; }

  /**
   * @brief returns true if the entity must have been requested for deletion
  */
  constexpr bool GetScheduleDeletion() const { return m_ScheduleDeletion; }

 protected:
  std::shared_ptr<ToolBoxObject> m_Toolbox = YEAGER_NULLPTR;
  bool m_ScheduleDeletion = false;
};

/**
 * @brief Game entities have proprieties like editor entities, they have a toolbox, and can interact with the engine itself, also, this kind of entity 
 * makes part of the game itself, so it have some other proprieties that the editor and the basic entities doesnt have, like a transformation. 
 */
class GameEntity : public EditorEntity {
 public:
  GameEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name = YEAGER_NULL_LITERAL);
  ~GameEntity();
  Transformation GetTransformation();
  Transformation* GetTransformationPtr();
  virtual void ProcessTransformation(Shader* Shader);
  void inline SetTransformation(const Transformation& trans) { m_EntityTransformation = trans; }

  constexpr inline std::vector<ObjectTexture*>* GetLoadedTextures() { return &m_EntityLoadedTextures; };

 protected:
  std::vector<ObjectTexture*> m_EntityLoadedTextures;
  Transformation m_EntityTransformation;
};

}  // namespace Yeager