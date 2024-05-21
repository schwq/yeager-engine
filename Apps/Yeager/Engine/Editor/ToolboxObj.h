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
#include "../Media/AudioHandle.h"
#include "../Renderer/Entity.h"
#include "../Renderer/TextureHandle.h"

namespace Yeager {
class Texture2D;
class GameEntity;
class Entity;
class AudioHandle;

struct ToolboxEntityType {
  enum Enum { EditorType, GameType, Undefined };
};

struct ToolboxOptionsHandle {
  float m_ObjectPhysicWeight = 1.0f;
  float m_ObjectPhysicGravityConst = 1.0f;
  float m_RandomRotationPower = 1.0f;
  bool m_RandomRotationCheckbox = false;
  bool m_PhysicGravityCheckbox = true;
  bool m_PhysicReverseGravityCheckbox = false;
  float m_Audio3DPosition[3] = {0.0f, 0.0f, 0.0f};
  irrklang::ik_f32 m_AudioVolume = 0.5f;
  float m_TextureSpaceTaken = 0;
};

static bool ValidateToolbox(ToolboxHandle* handle);

static bool EntityToolboxIsSeen(EntityObjectType::Enum type);

/**
 * @brief ToolboxHandle represent the selectable object from the toolbox window, listing objects, audios, skyboxes of the current scene to be
 * selected and modify it. The ToolboxHandle MUST be built by the editor entity
 */
class ToolboxHandle {
 public:
  ToolboxHandle() = default;

  ToolboxHandle(Yeager::EditorEntity* entity);
  ToolboxHandle(Yeager::GameEntity* entity);

  ~ToolboxHandle() {}

  Yeager::Transformation* GetTransformation()
  {
    return static_cast<Yeager::GameEntity*>(m_EntityPtr)->GetTransformationPtr();
  }
  Yeager::EditorEntity* GetEntity() { return m_EntityPtr; }

  void DrawObject();

  constexpr void SetEntity(Yeager::Entity* entity) { m_EntityPtr = (GameEntity*)entity; }
  constexpr void SetScheduleDeletion(bool deletion) { m_ScheduleDeletion = deletion; }
  constexpr bool GetScheduleDeletion() const { return m_ScheduleDeletion; }
  constexpr bool* IsSelected() { return &m_SelectedToolbox; }
  constexpr void SetSelected(bool selected) { m_SelectedToolbox = selected; }

  ToolboxEntityType::Enum GetEntityType() const { return m_EntityType; }

  constexpr bool IsSeen() const { return m_IsSeen; }

 private:
  ToolboxEntityType::Enum m_EntityType = ToolboxEntityType::Undefined;
  Yeager::EditorEntity* m_EntityPtr = YEAGER_NULLPTR;

  bool m_SelectedToolbox = false;
  bool m_ScheduleDeletion = false;
  bool m_IsSeen = false;
  bool m_RenameWindowOpen = false;
  bool m_FlipEveryTexture = false;
  String m_NewName = YEAGER_NULL_LITERAL;

  void DrawToolboxObjectType();
  void DrawToolboxObjectAnimated();
  void DrawToolboxObjectPlayable();
  void DrawToolboxObjectAnimatedPlayable();
  void DrawToolboxAudio();
  void DrawToolboxAudio3D();
  void DrawToolboxSkybox();

  ToolboxOptionsHandle m_Options;
};
}  // namespace Yeager
