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
#include "../Renderer/ShaderHandle.h"

namespace Yeager {
class Shader;
class ApplicationCore;
}  // namespace Yeager

class RendererEngine;

enum class CameraPosition { kForward, kBackward, kRight, kLeft };

class EditorCamera {
 public:
  EditorCamera(Yeager::ApplicationCore* app, YgVector3 cameraPosition = YgVector3(0.0f, 0.0f, 0.0f),
               YgVector3 cameraFront = YgVector3(0.0f, 0.0f, -1.0f), YgVector3 cameraUp = YgVector3(0.0f, 1.0f, 0.0f));
  EditorCamera() {}
  void UpdatePosition(CameraPosition position, float delta);
  void UpdateDirection(float xoffset, float yoffset);
  void UpdateSpeed(float speed);
  void Update(Yeager::Shader* shader);
  void MouseCallback(bool& firstMouse, float& lastX, float& lastY, double xpos, double ypos);
  void SetShouldMove(bool move);
  bool GetShouldMove();
  const YgMatrix4 ReturnViewMatrix();
  const YgVector3 GetPosition();
  constexpr inline void SetPosition(const YgVector3& pos) { m_Position = pos; }
  const YgVector3 GetDirection();
  const float& GetSensitivity();
  constexpr inline void SetDirection(const YgVector3& dir) { m_CameraDirection = dir; }
  const YgVector3 GetFront() { return m_CameraFront; }

 protected:
  YgVector3 m_Position = YgVector3(0.0f);
  YgVector3 m_CameraFront = YgVector3(0.0f);
  YgVector3 m_CameraUp = YgVector3(0.0f);
  YgVector3 m_CameraDirection = YgVector3(0.0f);
  float m_Sensitivity = 0.1f;
  float m_CameraYaw = 0.0f;
  float m_CameraPitch = 0.0f;
  float m_CameraSpeed = 2.0f;
  Yeager::ApplicationCore* m_Application = nullptr;
  bool m_CameraShouldMove = false;
};

class PlayerCamera : public EditorCamera {
  // TODO
};