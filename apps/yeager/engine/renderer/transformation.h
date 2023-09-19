#pragma once

#include "../../common/common.h"

class Transformation {
 public:
  Transformation() {}

  constexpr void SetPosition(Vector3 position) { m_position = position; }
  constexpr Vector3 GetPosition() { return m_position; }
  constexpr Vector3* PGetPosition() { return &m_position; }
  constexpr void SetRotation(Vector3 rotation) { m_rotation = rotation; }
  constexpr Vector3 GetRotation() { return m_rotation; }
  constexpr Vector3* PGetRotation() { return &m_rotation; }
  constexpr void SetScale(Vector3 scale) { m_scale = scale; }
  constexpr Vector3 GetScale() { return m_scale; }
  constexpr Vector3* PGetScale() { return &m_scale; }
  constexpr void SetModel(Matrix4 model) { m_model = model; }
  constexpr Matrix4 GetModel() { return m_model; }

 private:
  Vector3 m_position = Vector3(0.0f);
  Vector3 m_rotation = Vector3(0.0f);
  Vector3 m_scale = Vector3(1.0f);
  Matrix4 m_model = Matrix4(1.0f);
};
