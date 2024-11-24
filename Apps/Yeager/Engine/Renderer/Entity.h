

//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

namespace Yeager {

class ApplicationCore;
class Shader;
class Texture2D;
class ToolboxHandle;
class NodeComponent;
class MaterialBase;

struct Transformation {
  Transformation() {}
  Matrix4 model = YEAGER_IDENTITY_MATRIX4x4;
};

struct Transformation2D : Transformation {
  Transformation2D(const Vector2& Position, const Vector2& Rotation, const Vector2& Scale)
      : Transformation(), position(Position), rotation(Rotation), scale(Scale)
  {}
  Transformation2D() : Transformation() {}
  Vector2 position = YEAGER_ZERO_VECTOR2;
  Vector2 rotation = YEAGER_ZERO_VECTOR2;
  Vector2 scale = Vector2(1.0);

  static void Apply(Transformation2D* transformation, Yeager::Shader* shader);
  static void Apply(Transformation2D* transformation);
  static Matrix4 Apply(const Transformation2D& transformation);
  static Transformation2D GetDefault();
};

struct Transformation3D : Transformation {
  Transformation3D(const Vector3& Position, const Vector3& Rotation, const Vector3& Scale)
      : Transformation(), position(Position), rotation(Rotation), scale(Scale)
  {}
  Transformation3D() : Transformation() {}
  Vector3 position = YEAGER_ZERO_VECTOR3;
  Vector3 rotation = YEAGER_ZERO_VECTOR3;
  Vector3 scale = Vector3(1.0);

  static void Apply(Transformation3D* transformation, Yeager::Shader* shader);
  static void Apply(Transformation3D* transformation);
  static Matrix4 Apply(const Transformation3D& transformation);
  static Transformation3D GetDefault();
};

struct EntityObjectType {
  enum Enum {
    OBJECT,
    OBJECT_ANIMATED,
    OBJECT_INSTANCED,
    OBJECT_INSTANCED_ANIMATED,
    OBJECT_PLAYABLE,
    OBJECT_ANIMATED_PLAYABLE,
    AUDIO_HANDLE,
    AUDIO_3D_HANDLE,
    TEXTURE,
    LIGHT_HANDLE,
    SKYBOX,
    CAMERA,
    UNDEFINED
  };
  YEAGER_ENUM_TO_STRING(EntityObjectType)
};

struct EntityBuilder {

  Yeager::ApplicationCore* Application = YEAGER_NULLPTR;
  String Name = YEAGER_NULL_LITERAL;
  uuid_t UUID;
  EntityObjectType::Enum Type = EntityObjectType::UNDEFINED;

  EntityBuilder(Yeager::ApplicationCore* application, String name = YEAGER_NULL_LITERAL,
                EntityObjectType::Enum type = EntityObjectType::UNDEFINED, uuid_t uuid = uuid_t())
      : Application(application), Name(name), UUID(uuid), Type(type)
  {
    if (UUID.is_nil())
      UUID = GetRandomUUID();
  }
};

/**
 * @brief Entities are the most basic type of class that almost all classes in this engine inherit, it contains variables that all classes share in common
 * like the name and global id, also it can be used to set a object to non serialization status, where the object doesnt get saved in the configuration file.
 * Entity object type are enums that can be used to modify simple entity pointer to a more complex pointer, like a AnimatedObject for example.
 */
class Entity {
 public:
  Entity(const EntityBuilder& builder);
  ~Entity();

  String GetName();
  void SetName(const String& name) { mName = name; }
  uuid_t GetEntityUUID();

  void SetEntityType(EntityObjectType::Enum type) { mType = type; }
  EntityObjectType::Enum GetEntityType() const { return mType; }

  YEAGER_FORCE_INLINE bool CanBeSerialize() const { return bCanBeSerialize; }
  YEAGER_FORCE_INLINE void SetCanBeSerialize(bool serial) { bCanBeSerialize = serial; }

  /* This can be useful when working with toolboxes or other areas without access to the application core pointer, why tho? i dont know, lazyness*/
  Yeager::ApplicationCore* GetApplication() { return mApplication; }

 protected:
  EntityObjectType::Enum mType = EntityObjectType::UNDEFINED;
  Yeager::ApplicationCore* mApplication = YEAGER_NULLPTR;
  String mName = YEAGER_NULL_LITERAL;
  uuid_t mEntityUUID;

  bool bRender = true;
  bool bCanBeSerialize = true;

  static Uint mEntityCount;
};

/**
 * @brief Editor entity is a entity that show proprieties on the editor, like having a toolbox selecteable and having 
 * events that track the status of this entity to been show on the editor screen, like a alert for critical problems or needing to reload or 
 * visualize it, like a texture. This class has made mainly from two other classes (shader and texture) that exists in the engine context, but
 * dont have the need to have a collision or physic tracker, like the game entity
 */
class EditorEntity : public Entity {
 public:
  EditorEntity(const EntityBuilder& builder);
  ~EditorEntity();

  std::shared_ptr<ToolboxHandle> GetToolbox() { return mToolbox; }

  /**
   * @brief Sets to true, when the scene calls for search for schedule deletions, it will remove this entity from the 
   * application, basically, delete it 
  */
  YEAGER_CONSTEXPR void SetScheduleDeletion(bool deletion) { bScheduleDeletion = deletion; }

  /**
   * @brief returns true if the entity must have been requested for deletion
  */
  YEAGER_CONSTEXPR bool GetScheduleDeletion() const { return bScheduleDeletion; }

  void BuildNode(std::shared_ptr<NodeComponent> parent);
  std::shared_ptr<NodeComponent> GetNodeComponent() { return mNode; }

 protected:
  /* Toolbox object handles the information about the entity, name, id, and custom propieties*/
  std::shared_ptr<ToolboxHandle> mToolbox = YEAGER_NULLPTR;
  /* Node component handles the relation of the entity with other entities, like if the are parent, children. 
    This build a hierarchy of information about parent nodes and their children */
  std::shared_ptr<NodeComponent> mNode = YEAGER_NULLPTR;

  bool bScheduleDeletion = false;
};

/**
 * @brief Game entities have proprieties like editor entities, they have a toolbox, and can interact with the engine itself, also, this kind of entity 
 * makes part of the game itself, so it have some other proprieties that the editor and the basic entities doesnt have, like a transformation. 
 */
class GameEntity : public EditorEntity {
 public:
  GameEntity(const EntityBuilder& builder);
  ~GameEntity();
  Transformation3D GetTransformation();
  Transformation3D* GetTransformationPtr();

  virtual void ApplyTransformation(Shader* Shader);
  YEAGER_FORCE_INLINE void SetTransformation(const Transformation3D& trans) { mEntityTransformation = trans; }
  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE std::vector<MaterialBase*>* GetLoadedTextures()
  {
    return &mEntityLoadedTextures;
  };

 protected:
  std::vector<MaterialBase*> mEntityLoadedTextures;
  Transformation3D mEntityTransformation;
};

}  // namespace Yeager