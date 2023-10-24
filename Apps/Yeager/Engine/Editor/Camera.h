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

#include "../../Application.h"
#include "../../Common/Common.h"
#include "../Renderer/ShaderHandle.h"

class Application;
class Yeager::Shader;
class RendererEngine;

enum class CameraPosition { kForward, kBackward, kRight, kLeft };

class EditorCamera {
 public:
  EditorCamera(Application* app, yg_vec3 cameraPosition = yg_vec3(0.0f, 0.0f, 0.0f),
               yg_vec3 cameraFront = yg_vec3(0.0f, 0.0f, -1.0f), yg_vec3 cameraUp = yg_vec3(0.0f, 1.0f, 0.0f));

  void UpdatePosition(CameraPosition position, float delta);
  void UpdateDirection(float xoffset, float yoffset);
  void UpdateSpeed(float speed);
  void Update(Yeager::Shader* shader);
  void MouseCallback(bool& firstMouse, float& lastX, float& lastY, double xpos, double ypos);
  void SetShouldMove(bool move);
  bool GetShouldMove();
  const yg_mat4 ReturnViewMatrix();
  const yg_vec3 GetPosition();
  const yg_vec3 GetDirection();
  const float& GetSensitivity();

 protected:
  yg_vec3 m_position;
  yg_vec3 m_cameraFront;
  yg_vec3 m_cameraUp;
  yg_vec3 m_cameraDirection;
  float m_sensitivity = 0.1f;
  float m_cameraYaw = 0.0f;
  float m_cameraPitch = 0.0f;
  float m_cameraSpeed = 0.2f;
  Application* m_app;
  bool m_cameraShouldMove = false;
};

class PlayerCamera : public EditorCamera {
  // TODO
};