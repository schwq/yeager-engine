#include "camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

EditorCamera::EditorCamera(Application* app, Vector3 cameraPosition,
                           Vector3 cameraFront, Vector3 cameraUp)
    : m_app(app) {
  m_position = cameraPosition;
  m_cameraFront = cameraFront;
  m_cameraUp = cameraUp;
  m_cameraDirection = Vector3(0.0f, 0.0f, 0.0f);
  m_cameraPitch = 0.0f;
  m_cameraYaw = 0.0f;
}

void EditorCamera::UpdateSpeed(float speed) {
  m_cameraSpeed = speed;
}

bool EditorCamera::GetShouldMove() {
  return m_cameraShouldMove;
}

const Matrix4 EditorCamera::ReturnViewMatrix() {
  return glm::lookAt(m_position, m_position + m_cameraFront, m_cameraUp);
}

void EditorCamera::Update(Shader* shader) {
  Matrix4 view = Matrix4(1.0f);
  view = glm::lookAt(m_position, m_position + m_cameraFront, m_cameraUp);
  shader->SetMat4("view", view);
}

void EditorCamera::UpdatePosition(CameraPosition position, float delta) {
  UpdateSpeed(2.5 * delta);

  switch (position) {
    case CameraPosition::kForward:
      m_position += m_cameraSpeed * m_cameraFront;
      break;
    case CameraPosition::kBackward:
      m_position -= m_cameraSpeed * m_cameraFront;
      break;
    case CameraPosition::kRight:
      m_position +=
          glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * m_cameraSpeed;
      break;
    case CameraPosition::kLeft:
      m_position -=
          glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * m_cameraSpeed;
      break;
  }
}

void EditorCamera::UpdateDirection(float xoffset, float yoffset) {
  xoffset *= m_sensitivity;
  yoffset *= m_sensitivity;

  m_cameraYaw += xoffset;
  m_cameraPitch += yoffset;

  if (m_cameraPitch > 89.0f) {
    m_cameraPitch = 89.0f;
  }

  if (m_cameraPitch < -89.0f) {
    m_cameraPitch = -89.0f;
  }

  m_cameraDirection.x =
      cos(glm::radians(m_cameraYaw)) * cos(glm::radians(m_cameraPitch));
  m_cameraDirection.y = sin(glm::radians(m_cameraPitch));
  m_cameraDirection.z =
      sin(glm::radians(m_cameraYaw)) * cos(glm::radians(m_cameraPitch));
  m_cameraFront = glm::normalize(m_cameraDirection);
}

const Vector3 EditorCamera::GetPosition() {
  return m_position;
}

const Vector3 EditorCamera::GetDirection() {
  return m_cameraDirection;
}

const float& EditorCamera::GetSensitivity() {
  return m_sensitivity;
}

void EditorCamera::SetShouldMove(bool move) {
  m_cameraShouldMove = move;
}

void EditorCamera::MouseCallback(bool& firstMouse, float& lastX, float& lastY,
                                 double xpos, double ypos) {
  if (m_cameraShouldMove &&
      m_app->GetCurrentMode() == ApplicationCurrentMode::kEditorMode) {
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