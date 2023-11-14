#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include "../../Application.h"

EditorCamera::EditorCamera(Yeager::ApplicationCore* app, YgVector3 cameraPosition, YgVector3 cameraFront,
                           YgVector3 cameraUp)
    : m_Application(app)
{
  m_Position = cameraPosition;
  m_CameraFront = cameraFront;
  m_CameraUp = cameraUp;
  m_CameraDirection = YgVector3(0.0f, 0.0f, 0.0f);
  m_CameraPitch = 0.0f;
  m_CameraYaw = 0.0f;
}

void EditorCamera::UpdateSpeed(float speed)
{
  m_CameraSpeed = speed;
}

bool EditorCamera::GetShouldMove()
{
  return m_CameraShouldMove;
}

const YgMatrix4 EditorCamera::ReturnViewMatrix()
{
  return glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
}

void EditorCamera::Update(Yeager::Shader* shader)
{
  YgMatrix4 view = YgMatrix4(1.0f);
  view = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
  shader->SetMat4("view", view);
}

void EditorCamera::UpdatePosition(CameraPosition position, float delta)
{
  UpdateSpeed(2.5 * delta);

  switch (position) {
    case CameraPosition::kForward:
      m_Position += m_CameraSpeed * m_CameraFront;
      break;
    case CameraPosition::kBackward:
      m_Position -= m_CameraSpeed * m_CameraFront;
      break;
    case CameraPosition::kRight:
      m_Position += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
      break;
    case CameraPosition::kLeft:
      m_Position -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
      break;
  }
}

void EditorCamera::UpdateDirection(float xoffset, float yoffset)
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

const YgVector3 EditorCamera::GetPosition()
{
  return m_Position;
}

const YgVector3 EditorCamera::GetDirection()
{
  return m_CameraDirection;
}

const float& EditorCamera::GetSensitivity()
{
  return m_Sensitivity;
}

void EditorCamera::SetShouldMove(bool move)
{
  m_CameraShouldMove = move;
}

void EditorCamera::MouseCallback(bool& firstMouse, float& lastX, float& lastY, double xpos, double ypos)
{
  if (m_CameraShouldMove && m_Application->GetMode() == Yeager::AppEditor) {
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