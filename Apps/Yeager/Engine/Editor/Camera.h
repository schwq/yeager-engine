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
#include "../Physics/PhysXCharacterController.h"
#include "../Renderer/Entity.h"

namespace Yeager {
class Shader;
class ApplicationCore;

struct YgCameraPosition {
  enum Enum { eCAMERA_FORWARD, eCAMERA_BACKWARD, eCAMERA_RIGHT, eCAMERA_LEFT, eCAMERA_UP, eCAMERA_DOWN };
};

struct YgCameraType {
  enum Enum { eCAMERA_BASE, eCAMERA_EDITOR, eCAMERA_PLAYER };
};

/**
 * @brief BaseCamera holds all values in common of the types of camera that are created inside the program, like the player`s camera in the user`s game, and the built-in camera 
 * for the editing. Most of the code in this class is self explanatory 
 * TODO the entity class must have a child class for classes like the camera 
 */
class BaseCamera : public EditorEntity {
 public:
  BaseCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition = Vector3(0.0f, 0.0f, 0.0f),
             Vector3 cameraFront = Vector3(0.0f, 0.0f, -1.0f), Vector3 cameraUp = Vector3(0.0f, 1.0f, 0.0f));
  ~BaseCamera() {}

  virtual void UpdatePosition(YgCameraPosition::Enum position, float delta);
  void UpdateDirection(float xoffset, float yoffset);
  void UpdateSpeed(float speed);
  void Update(Yeager::Shader* shader);
  void MouseCallback(bool& firstMouse, float& lastX, float& lastY, double xpos, double ypos);

  const Matrix4 ReturnViewMatrix();

  YEAGER_FORCE_INLINE float GetSensitivity() const { return m_Sensitivity; }
  YEAGER_FORCE_INLINE bool GetShouldMove() const { return m_CameraShouldMove; }
  YEAGER_FORCE_INLINE const Vector3 GetPosition() const { return m_Position; }
  YEAGER_FORCE_INLINE const Vector3 GetDirection() const { return m_CameraDirection; }
  YEAGER_FORCE_INLINE void SetShouldMove(bool move) { m_CameraShouldMove = move; }
  YEAGER_FORCE_INLINE void SetPosition(const Vector3& pos) { m_Position = pos; }
  YEAGER_FORCE_INLINE void SetDirection(const Vector3& dir) { m_CameraDirection = dir; }
  YEAGER_FORCE_INLINE Vector3 GetFront() const { return m_CameraFront; }

  YEAGER_FORCE_INLINE YgCameraType::Enum GetCameraType() const { return m_CameraType; }

 protected:
  Vector3 m_Position = YEAGER_ZERO_VECTOR3;
  Vector3 m_CameraFront = YEAGER_ZERO_VECTOR3;
  Vector3 m_CameraUp = YEAGER_ZERO_VECTOR3;
  Vector3 m_CameraDirection = YEAGER_ZERO_VECTOR3;

  YgCameraType::Enum m_CameraType = YgCameraType::eCAMERA_BASE;

  float m_Sensitivity = 0.1f;
  float m_CameraYaw = 0.0f;
  float m_CameraPitch = 0.0f;
  float m_CameraSpeed = 2.0f;

  bool m_CameraShouldMove = false;
};

/**
 * @brief EditorCamera is the camera that the yeager engine editor uses to show the game development, like flying around.
 * The player camera in the user`s game and the camera programmed inside the engine have a lot of similarities, like position, sensitivity, and direction for example
 * Even so, it`s a good pratice to separate both cameras types, isolating problems that might occur during the runtime of the user`s game
 */
class EditorCamera : public BaseCamera {
 public:
  EditorCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition = Vector3(0.0f, 0.0f, 0.0f),
               Vector3 cameraFront = Vector3(0.0f, 0.0f, -1.0f), Vector3 cameraUp = Vector3(0.0f, 1.0f, 0.0f));
  /** TODO implement this function properlly */
  void RayCasting(int mouse_x, int mouse_y, Matrix4 projection, Matrix4 view);

 protected:
  bool m_CameraCanFly = false;
  bool m_CameraShouldCollide = true;
};

class PlayerCamera : public BaseCamera {
 public:
  PlayerCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition = Vector3(0.0f, 0.0f, 0.0f),
               Vector3 cameraFront = Vector3(0.0f, 0.0f, -1.0f), Vector3 cameraUp = Vector3(0.0f, 1.0f, 0.0f));
  ~PlayerCamera();

  void Teleport(const Vector3& globalPose);
  void UpdatePosition(YgCameraPosition::Enum position, float delta);
  void ApplyGravity(float delta);
  void AttachCameraPosToCharacterPos();
  void Update(float delta);

 protected:
  physx::PxController* m_Controller = YEAGER_NULLPTR;

  PhysXCollisionDetection CharacterControllerMove(const physx::PxVec3& disp, physx::PxF32 minDist,
                                                  physx::PxF32 elapsedTime, const physx::PxControllerFilters& filters,
                                                  const physx::PxObstacleContext* obstacles = YEAGER_NULLPTR);
  bool m_CollisionFloor = false;
};

}  // namespace Yeager