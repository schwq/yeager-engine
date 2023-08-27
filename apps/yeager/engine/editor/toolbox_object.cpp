#include "toolbox_object.h"

using namespace ImGui;

void ToolBoxObject::DrawObject() {
  switch (m_type) {
    case ExplorerObjectType::kShapes:
    case ExplorerObjectType::kImportedObject:

      Text("Position:");
      InputFloat("Pos X", &m_position->x);
      InputFloat("Pos Y", &m_position->y);
      InputFloat("Pos Z", &m_position->z);

      Text("Rotation");
      SameLine();
      Checkbox("Random Rotation", &m_random_rotation_checkbox);
      if (m_random_rotation_checkbox) {
        float angles = glfwGetTime();
        m_rotation->x = 10.0f * angles;
        m_rotation->y = 20.0f * angles;
        m_rotation->z = 15.0f * angles;
      }
      InputFloat("Rot X", &m_rotation->x);
      InputFloat("Rot Y", &m_rotation->y);
      InputFloat("Rot Z", &m_rotation->z);

      SliderFloat("Scale X", &m_scale->x, 0.01f, 100.0f);
      SliderFloat("Scale Y", &m_scale->y, 0.01f, 100.0f);
      SliderFloat("Scale Z", &m_scale->z, 0.01f, 100.0f);
      break;
  }
}