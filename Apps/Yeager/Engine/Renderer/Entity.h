
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
#include "../../Kernel/Allocator.h"

namespace Yeager {

class ApplicationCore;
class Shader;
class Texture2D;
class ToolboxHandle;
class NodeComponent;
class MaterialBase;

struct Transformation {
  Matrix4 model = YEAGER_IDENTITY_MATRIX4x4;
  Vector3 position = YEAGER_ZERO_VECTOR3;
  Vector3 rotation = YEAGER_ZERO_VECTOR3;
  Vector3 scale = Vector3(1.0);
};

extern Transformation GetDefaultTransformation();
extern void ApplyTransformation(Transformation* trans);

struct EntityObjectType {
  enum Enum {
    OBJECT,
    OBJECT_ANIMATED,
    OBJECT_INSTANCED,
    OBJECT_INSTANCED_ANIMATED,
    AUDIO_HANDLE,
    AUDIO_3D_HANDLE,
    TEXTURE,
    LIGHT_HANDLE,
    SKYBOX,
    CAMERA,
    UNDEFINED
  };
  static String ToString(Enum type);
};

/**
 * @brief Entities are the most basic type of class that almost all classes in this engine inherit, it contains variables that all classes share in common
 * like the name and global id, also it can be used to set a object to non serialization status, where the object doesnt get saved in the configuration file.
 * Entity object type are enums that can be used to modify simple entity pointer to a more complex pointer, like a AnimatedObject for example.
 */
class Entity {
 public:
  Entity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name = YEAGER_NULL_LITERAL);
  ~Entity();

  String GetName();
  void SetName(const String& name) { m_Name = name; }
  unsigned int GetId();

  void SetEntityType(EntityObjectType::Enum type) { m_Type = type; }
  EntityObjectType::Enum GetEntityType() const { return m_Type; }

  YEAGER_FORCE_INLINE bool CanBeSerialize() const { return m_CanBeSerialize; }
  YEAGER_FORCE_INLINE void SetCanBeSerialize(bool serial) { m_CanBeSerialize = serial; }

  /* User data can be any type of data that a object or class that ihherit the Entity class can link to it, and extract in some point during the process */
  UserDataHandle* GetUserData() { return &m_UserData; }

 protected:
  EntityObjectType::Enum m_Type = EntityObjectType::UNDEFINED;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  UserDataHandle m_UserData;
  String m_Name = YEAGER_NULL_LITERAL;
  bool m_Render = true;
  bool m_CanBeSerialize = true;
  const unsigned int m_EntityID;
  static unsigned int m_EntityCountID;
};

/**
 * @brief Editor entity is a entity that show proprieties on the editor, like having a toolbox selecteable and having 
 * events that track the status of this entity to been show on the editor screen, like a alert for critical problems or needing to reload or 
 * visualize it, like a texture. This class has made mainly from two other classes (shader and texture) that exists in the engine context, but
 * dont have the need to have a collision or physic tracker, like the game entity
 */
class EditorEntity : public Entity {
 public:
  EditorEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name = YEAGER_NULL_LITERAL);
  ~EditorEntity();

  std::shared_ptr<ToolboxHandle> GetToolbox() { return m_Toolbox; }

  /**
   * @brief Sets to true, when the scene calls for search for schedule deletions, it will remove this entity from the 
   * application, basically, delete it 
  */
  constexpr void SetScheduleDeletion(bool deletion) { m_ScheduleDeletion = deletion; }

  /**
   * @brief returns true if the entity must have been requested for deletion
  */
  constexpr bool GetScheduleDeletion() const { return m_ScheduleDeletion; }

  void BuildNode(Yeager::NodeComponent* parent);
  Yeager::NodeComponent* GetNodeComponent() { return m_Node; }

 protected:
  /* Toolbox object handles the information about the entity, name, id, and custom propieties*/
  std::shared_ptr<ToolboxHandle> m_Toolbox = YEAGER_NULLPTR;
  /* Node component handles the relation of the entity with other entities, like if the are parent, children. 
    This build a hierarchy of information about parent nodes and their children */
  Yeager::NodeComponent* m_Node = YEAGER_NULLPTR;

  bool m_ScheduleDeletion = false;
};

/**
 * @brief Game entities have proprieties like editor entities, they have a toolbox, and can interact with the engine itself, also, this kind of entity 
 * makes part of the game itself, so it have some other proprieties that the editor and the basic entities doesnt have, like a transformation. 
 */
class GameEntity : public EditorEntity {
 public:
  GameEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name = YEAGER_NULL_LITERAL);
  ~GameEntity();
  Transformation GetTransformation();
  Transformation* GetTransformationPtr();

  virtual void ApplyTransformation(Shader* Shader);
  void inline SetTransformation(const Transformation& trans) { m_EntityTransformation = trans; }

  constexpr inline std::vector<MaterialBase*>* GetLoadedTextures() { return &m_EntityLoadedTextures; };

 protected:
  std::vector<MaterialBase*> m_EntityLoadedTextures;
  Transformation m_EntityTransformation;
};

}  // namespace Yeager