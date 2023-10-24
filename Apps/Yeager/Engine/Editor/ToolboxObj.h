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

class Yeager::Texture2D;
class Yeager::GameEntity;
class Yeager::Entity;
class Yeager::AudioHandle;

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
  kSkybox,
  kImportedObject,
  kPointLight,
  kNone,
  kNoChange
};

extern yg_string ExplorerTypeToString(ExplorerObjectType type);
namespace Yeager {
class ToolBoxObject {
 public:
  ToolBoxObject(){};
  Yeager::Transformation* GetTransformation() { return m_entity->GetTransformationPtr(); }

  Yeager::GameEntity* GetEntity() { return m_entity; }
  constexpr void SetTransformation(Yeager::GameEntity* entity) { m_entity = entity; }

  void DrawObject();
  constexpr void SetType(ExplorerObjectType type) { m_type = type; }
  ExplorerObjectType GetType() { return m_type; }
  void SetPhysics(Yeager::EntityPhysics* physics) { m_physics = physics; }
  Yeager::EntityPhysics* GetPhysics() { return m_physics; }
  void SetAudio(Yeager::AudioHandle* audio);
  bool m_selected = false;

 private:
  ExplorerObjectType m_type;
  Yeager::GameEntity* m_entity = nullptr;
  Yeager::EntityPhysics* m_physics = nullptr;
  Yeager::AudioHandle* m_audio = nullptr;

  float obj_weight = 1.0f;
  float obj_gravity_const = 1.0f;
  bool m_random_rotation_checkbox = false;
  bool m_gravity_checkbox = true;
  bool m_reverse_gravity_checkbox = false;
  irrklang::ik_f32 m_sound_volume = 0.5f;
};
}  // namespace Yeager
extern Yeager::ToolBoxObject GetDefaultToolBoxObject();
extern std::vector<std::shared_ptr<Yeager::ToolBoxObject>> m_toolboxs;
