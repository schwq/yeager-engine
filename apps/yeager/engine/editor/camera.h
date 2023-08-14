#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../renderer/shader.h"
#include "../../application.h"

class Application;
class Shader;
class RendererEngine;

enum class CameraPosition { kForward, kBackward, kRight, kLeft };

class EditorCamera {
  public:
    EditorCamera(Application *app, Vector3 cameraPosition = Vector3(0.0f, 0.0f, 0.0f),
                 Vector3 cameraFront = Vector3(0.0f, 0.0f, -1.0f),
                 Vector3 cameraUp = Vector3(0.0f, 1.0f, 0.0f));

    void UpdatePosition(CameraPosition position, float delta);
    void UpdateDirection(float xoffset, float yoffset);
    void UpdateSpeed(float speed);
    void Update(Shader *shader);
    void MouseCallback(bool &firstMouse, float &lastX, float &lastY, double xpos, double ypos);
    void SetShouldMove(bool move);
    bool GetShouldMove();
    const Matrix4 ReturnViewMatrix();
    const Vector3 GetPosition();
    const Vector3 GetDirection();
    const float &GetSensitivity();

  protected:
    Vector3 m_position;
    Vector3 m_cameraFront;
    Vector3 m_cameraUp;
    Vector3 m_cameraDirection;
    float m_sensitivity = 0.1f;
    float m_cameraYaw = 0.0f;
    float m_cameraPitch = 0.0f;
    float m_cameraSpeed = 0.2f;
    Application *m_app;
    bool m_cameraShouldMove = false;
};

class PlayerCamera : public EditorCamera {
    // TODO
};