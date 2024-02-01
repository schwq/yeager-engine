#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include "../../Application.h"
using namespace Yeager;
void EditorCamera::RayCasting(int mouse_x, int mouse_y, YgMatrix4 projection, YgMatrix4 view)
{
  YEAGER_NOT_IMPLEMENTED("RayCasting");
}

BaseCamera::BaseCamera(Yeager::ApplicationCore* app, YgVector3 cameraPosition, YgVector3 cameraFront,
                       YgVector3 cameraUp)
    : EditorEntity(EntityObjectType::eCAMERA, app, "Camera")
{
  m_Position = cameraPosition;
  m_CameraFront = cameraFront;
  m_CameraUp = cameraUp;
  m_CameraDirection = YgVector3(0.0f, 0.0f, 0.0f);
  m_CameraPitch = 0.0f;
  m_CameraYaw = 0.0f;
}

EditorCamera::EditorCamera(Yeager::ApplicationCore* app, YgVector3 cameraPosition, YgVector3 cameraFront,
                           YgVector3 cameraUp)
    : BaseCamera(app, cameraPosition, cameraFront, cameraUp)
{
  Yeager::Log(INFO, "Editor Camera was created!");
}

void BaseCamera::UpdateSpeed(float speed)
{
  m_CameraSpeed = speed;
}

const YgMatrix4 BaseCamera::ReturnViewMatrix()
{
  return glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
}

void BaseCamera::Update(Yeager::Shader* shader)
{
  YgMatrix4 view = YgMatrix4(1.0f);
  view = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
  shader->SetMat4("view", view);
}

void BaseCamera::UpdatePosition(YgCameraPosition::Enum position, float delta)
{
  UpdateSpeed(30.0f * delta);
  YgVector3 lastPos = m_Position;
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
  m_Application->GetPhysXHandle()->GetCharacterController()->Move(
      m_Application->m_Controller, physx::PxVec3(Yeager::YgVector3ToPxVec3(m_Position - lastPos)), 0.1f, delta, NULL);
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