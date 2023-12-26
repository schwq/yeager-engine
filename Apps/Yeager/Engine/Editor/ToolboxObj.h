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
#include "../Media/AudioHandle.h"
#include "../Physics/PhysicsHandle.h"
#include "../Renderer/Entity.h"
#include "../Renderer/TextureHandle.h"

namespace Yeager {
class Texture2D;
class GameEntity;
class Entity;
class AudioHandle;
}  // namespace Yeager
enum class ExplorerObjectType {
  kShapes = 0,
  kScene,
  kShader,
  kTexture,
  kPlayer,
  kNetwork,
  kImage,
  kVideo,
  kAudio,
  k3DAudio,
  kSkybox,
  kImportedObject,
  kPointLight,
  kNone,
  kNoChange
};

extern YgString ExplorerTypeToString(ExplorerObjectType type);
namespace Yeager {
class ToolBoxObject {
 public:
  ToolBoxObject(){};
  ToolBoxObject(Yeager::GameEntity* entity, ExplorerObjectType type, Yeager::EntityPhysics* physics)
      : m_entity(entity), m_type(type), m_physics(physics){};
  Yeager::Transformation* GetTransformation() { return m_entity->GetTransformationPtr(); }

  Yeager::GameEntity* GetEntity() { return m_entity; }
  constexpr void SetTransformation(Yeager::GameEntity* entity) { m_entity = entity; }

  void DrawObject();
  constexpr void SetType(ExplorerObjectType type) { m_type = type; }
  ExplorerObjectType GetType() { return m_type; }
  void SetPhysics(Yeager::EntityPhysics* physics) { m_physics = physics; }
  Yeager::EntityPhysics* GetPhysics() { return m_physics; }
  void SetAudio(Yeager::AudioHandle* audio);
  void Set3DAudio(Yeager::Audio3DHandle* audio);

  constexpr void SetEntity(Yeager::Entity* entity) { m_entity = (GameEntity*)entity; }

  bool m_selected = false;

 private:
  ExplorerObjectType m_type;
  Yeager::GameEntity* m_entity = YEAGER_NULLPTR;
  Yeager::EntityPhysics* m_physics = YEAGER_NULLPTR;
  Yeager::Audio3DHandle* m_audio = YEAGER_NULLPTR;

  float obj_weight = 1.0f;
  float obj_gravity_const = 1.0f;
  float m_random_rotation_pow = 1.0f;
  bool m_random_rotation_checkbox = false;
  bool m_gravity_checkbox = true;
  bool m_reverse_gravity_checkbox = false;
  float m_3d_audio_position[3];
  irrklang::ik_f32 m_sound_volume = 0.5f;
};
}  // namespace Yeager
extern Yeager::ToolBoxObject GetDefaultToolBoxObject();
extern std::vector<std::shared_ptr<Yeager::ToolBoxObject>> m_toolboxs;
