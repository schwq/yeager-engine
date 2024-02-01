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

enum ExplorerObjectType {
  EExplorerTypeObject,
  EExplorerTypeAnimatedObject,
  EExplorerTypeShader,
  EExplorerTypeSkybox,
  EExplorerTypeInstancedObject,
  EExplorerTypeAnimatedInstancedObject,
  EExplorerTypeAudio,
  EExplorerTypeAudio3D,
  EExplorerTypeNull
};

extern YgString ExplorerTypeToString(ExplorerObjectType type);

/**
 * @brief ToolBoxObject represent the selectable object from the toolbox window, listing objects, audios, skyboxes of the current scene to be
 * selected and modify it
 */
class ToolBoxObject {
 public:
  ToolBoxObject() { m_Valid = true; };
  ToolBoxObject(Yeager::GameEntity* entity, ExplorerObjectType type) : m_GameEntityPtr(entity), m_ExplorerType(type)
  {
    m_Valid = true;
  };
  ~ToolBoxObject() { m_Valid = false; }

  Yeager::Transformation* GetTransformation() { return m_GameEntityPtr->GetTransformationPtr(); }
  Yeager::GameEntity* GetEntity() { return m_GameEntityPtr; }

  void DrawObject();

  constexpr void SetType(ExplorerObjectType type) { m_ExplorerType = type; }
  ExplorerObjectType GetType() const { return m_ExplorerType; }
  constexpr void SetEntity(Yeager::Entity* entity) { m_GameEntityPtr = (GameEntity*)entity; }
  constexpr void SetScheduleDeletion(bool deletion) { m_ScheduleDeletion = deletion; }
  constexpr bool GetScheduleDeletion() const { return m_ScheduleDeletion; }
  constexpr bool IsValid() const { return m_Valid; }
  constexpr bool* IsSelected() { return &m_SelectedToolbox; }
  constexpr void SetSelected(bool selected) { m_SelectedToolbox = selected; }

 private:
  ExplorerObjectType m_ExplorerType = EExplorerTypeNull;
  Yeager::GameEntity* m_GameEntityPtr = YEAGER_NULLPTR;

  bool m_Valid = false;
  bool m_SelectedToolbox = false;

  void DrawToolboxObjectType();
  void DrawToolboxObjectAnimated();
  void DrawToolboxAudio();
  void DrawToolboxAudio3D();

  float m_ObjectPhysicWeight = 1.0f;
  float m_ObjectPhysicGravityConst = 1.0f;
  float m_RandomRotationPower = 1.0f;
  bool m_RandomRotationCheckbox = false;
  bool m_PhysicGravityCheckbox = true;
  bool m_PhysicReverseGravityCheckbox = false;
  float m_Audio3DPosition[3] = {0.0f, 0.0f, 0.0f};
  bool m_ScheduleDeletion = false;
  irrklang::ik_f32 m_AudioVolume = 0.5f;
};
}  // namespace Yeager
