#pragma once

#include <cassert>
#include "../../application.h"
#include "../../common/common.h"
#include "../lighting/lighting.h"
#include "../renderer/texture.h"
#include "editor_explorer.h"

enum class ExplorerObjectType;
class YeagerTexture2D;
struct PointLight;

class ToolBoxObject {
 public:
  ToolBoxObject(){};
  constexpr Vector3* GetPosition() noexcept {
    assert(m_position == nullptr);
    return m_position;
  }
  constexpr void SetPosition(Vector3* position) { m_position = position; }
  constexpr Vector3* GetRotation() noexcept {
    assert(m_rotation == nullptr);
    return m_rotation;
  }
  constexpr void SetRotation(Vector3* rotation) { m_rotation = rotation; }
  constexpr YeagerTexture2D* GetTexture() noexcept {
    assert(m_texture == nullptr);
    return m_texture;
  }
  constexpr void SetScale(Vector3* scale) { m_scale = scale; }
  constexpr Vector3* GetScale() noexcept {
    assert(m_scale == nullptr);
    return m_scale;
  }
  constexpr void SetPointLight(PointLight* prop) { m_pt_light_prop = prop; }
  constexpr PointLight* GetPointLightProp() noexcept { return m_pt_light_prop; }
  constexpr void SetTexture(YeagerTexture2D* texture) { m_texture = texture; }
  void DrawObject();
  constexpr void SetType(ExplorerObjectType type) { m_type = type; }

 private:
  ExplorerObjectType m_type;
  Vector3* m_position = nullptr;
  Vector3* m_rotation = nullptr;
  Vector3* m_scale = nullptr;
  PointLight* m_pt_light_prop = nullptr;

  YeagerTexture2D* m_texture = nullptr;
  bool m_random_rotation_checkbox = false;
};