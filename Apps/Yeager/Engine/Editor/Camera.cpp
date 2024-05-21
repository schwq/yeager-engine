#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include "../../Application.h"
using namespace Yeager;

String Yeager::CameraOrientationToString(const Vector3& orientation)
{
  if (orientation == YEAGER_CAMERA_ORIENTATION_ABOVE)
    return "Above";
  if (orientation == YEAGER_CAMERA_ORIENTATION_DOWN)
    return "Down";
  if (orientation == YEAGER_CAMERA_ORIENTATION_LEFT)
    return "Left";
  if (orientation == YEAGER_CAMERA_ORIENTATION_RIGHT)
    return "Right";
  if (orientation == YEAGER_CAMERA_ORIENTATION_FRONT)
    return "Front";
  if (orientation == YEAGER_CAMERA_ORIENTATION_BACK)
    return "Back";
  return YEAGER_NULL_LITERAL;
}

void EditorCamera::RayCasting(int mouse_x, int mouse_y, Matrix4 projection, Matrix4 view)
{
  YEAGER_NOT_IMPLEMENTED("RayCasting");
}

void BaseCamera::TransferInformation(Yeager::BaseCamera* other)
{
  this->m_Position = other->m_Position;
  this->m_CameraDirection = other->m_CameraDirection;
  this->m_CameraPitch = other->m_CameraPitch;
  this->m_CameraYaw = other->m_CameraYaw;
  this->m_CameraFront = other->m_CameraFront;
}

BaseCamera::BaseCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition, Vector3 cameraFront, Vector3 cameraUp)
    : EditorEntity(EntityObjectType::CAMERA, app, "Camera")
{
  m_Position = cameraPosition;
  m_CameraFront = cameraFront;
  m_CameraUp = cameraUp;
  m_CameraDirection = Vector3(0.0f, 0.0f, 0.0f);
  m_CameraPitch = 0.0f;
  m_CameraYaw = 0.0f;
  Yeager::Log(INFO, "Base Camera was created!");
}

EditorCamera::EditorCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition, Vector3 cameraFront, Vector3 cameraUp)
    : BaseCamera(app, cameraPosition, cameraFront, cameraUp)
{
  m_CameraType = YgCameraType::eCAMERA_EDITOR;
  Yeager::Log(INFO, "Editor Camera was created!");
}

void BaseCamera::UpdateSpeed(float speed)
{
  m_CameraSpeed = speed;
}

const Matrix4 BaseCamera::ReturnViewMatrix()
{
  return glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
}

void BaseCamera::Update(Yeager::Shader* shader)
{
  Matrix4 view = Matrix4(1.0f);
  view = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
  shader->SetMat4("view", view);
}

void BaseCamera::UpdatePosition(YgCameraPosition::Enum position, float delta)
{
  UpdateSpeed(30.0f * delta);
  switch (position) {
    case YgCameraPosition::eCAMERA_FORWARD: {
      m_Position += m_CameraSpeed * m_CameraFront;
    } break;
    case YgCameraPosition::eCAMERA_BACKWARD: {
      m_Position -= m_CameraSpeed * m_CameraFront;
    } break;
    case YgCameraPosition::eCAMERA_RIGHT: {
      m_Position += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
    } break;
    case YgCameraPosition::eCAMERA_LEFT: {
      m_Position -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
    } break;
  }
}

void BaseCamera::UpdateDirection(float xoffset, float yoffset)
{
  xoffset *= m_Sensitivity;
  yoffset *= m_Sensitivity;

  m_CameraYaw += xoffset;
  m_CameraPitch += yoffset;

  if (m_CameraPitch > 89.0f) {
    m_CameraPitch = 89.0f;
  }

  if (m_CameraPitch < -89.0f) {
    m_CameraPitch = -89.0f;
  }

  m_CameraDirection.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
  m_CameraDirection.y = sin(glm::radians(m_CameraPitch));
  m_CameraDirection.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
  m_CameraFront = glm::normalize(m_CameraDirection);
}

void BaseCamera::MouseCallback(bool& firstMouse, float& lastX, float& lastY, double xpos, double ypos)
{
  if (m_CameraShouldMove && m_Application->GetMode() == YgApplicationMode::eAPPLICATION_EDITOR) {
    if (firstMouse) {
      lastX = static_cast<float>(xpos);
      lastY = static_cast<float>(ypos);
      firstMouse = false;
    }

    float yoffset = lastY - static_cast<float>(ypos);
    float xoffset = static_cast<float>(xpos) - lastX;
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    UpdateDirection(xoffset, yoffset);
  }
}

PlayerCamera::PlayerCamera(Yeager::ApplicationCore* app, Vector3 cameraPosition, Vector3 cameraFront, Vector3 cameraUp)
    : BaseCamera(app, cameraPosition, cameraFront, cameraUp)
{
  m_CameraType = YgCameraType::eCAMERA_PLAYER;
  m_Controller = m_Application->GetPhysXHandle()->GetCharacterController()->CreateController();
  Yeager::Log(INFO, "Player Camera was created!");
}

PlayerCamera::~PlayerCamera()
{
  //if (m_Controller)
  // TODO Check here the controller is been deleting, maybe in the deletion of CharacterController, all controllers are destroyed
  //PX_RELEASE(m_Controller);
}

void PlayerCamera::Teleport(const Vector3& globalPose)
{
  m_Application->GetPhysXHandle()->GetCharacterController()->SetPosition(
      m_Controller, physx::PxExtendedVec3(globalPose.x, globalPose.y, globalPose.z));
}

void PlayerCamera::UpdatePosition(YgCameraPosition::Enum position, float delta)
{
  UpdateSpeed(30.0f * delta);
  Vector3 lastPos = m_Position;
  switch (position) {
    case YgCameraPosition::eCAMERA_FORWARD: {
      m_Position += m_CameraSpeed * m_CameraFront;
    } break;
    case YgCameraPosition::eCAMERA_BACKWARD: {
      m_Position -= m_CameraSpeed * m_CameraFront;
    } break;
    case YgCameraPosition::eCAMERA_RIGHT: {
      m_Position += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
    } break;
    case YgCameraPosition::eCAMERA_LEFT: {
      m_Position -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
    } break;
  }
  CharacterControllerMove(physx::PxVec3(Yeager::Vector3ToPxVec3(m_Position - lastPos)), 0.1f, delta, NULL);
}

PhysXCollisionDetection PlayerCamera::CharacterControllerMove(const physx::PxVec3& disp, physx::PxF32 minDist,
                                                              physx::PxF32 elapsedTime,
                                                              const physx::PxControllerFilters& filters,
                                                              const physx::PxObstacleContext* obstacles)
{
  return m_Application->GetPhysXHandle()->GetCharacterController()->Move(m_Controller, disp, minDist, elapsedTime,
                                                                         filters, obstacles);
}

void PlayerCamera::ApplyGravity(float delta)
{
  if (!m_CollisionFloor) {
    PhysXCollisionDetection det = CharacterControllerMove(physx::PxVec3(0.0f, -1.0f, 0.0f), 0.01f, delta, NULL);
    if (det.bHasCollideBelow)
      m_CollisionFloor = true;
  } else {
    PhysXCollisionDetection det = CharacterControllerMove(physx::PxVec3(0.0f, 0.0f, 0.0f), 0.01f, delta, NULL);
    if (!det.bHasCollideBelow) {
      m_CollisionFloor = false;
    }
  }
}

void PlayerCamera::SetPosition(const Vector3& pos)
{
  m_Position = pos;
  Teleport(pos);  // Must teleport to the position
}

void PlayerCamera::AttachCameraPosToCharacterPos()
{
  physx::PxExtendedVec3 pos = m_Controller->getPosition();
  SetPosition(Vector3(pos.x, pos.y, pos.z));
}

void PlayerCamera::Update(float delta)
{
  ApplyGravity(delta);
  AttachCameraPosToCharacterPos();
}
