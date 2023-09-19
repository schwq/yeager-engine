#include "toolbox_object.h"

using namespace ImGui;

void ToolBoxObject::DrawObject() {
  switch (m_type) {
    case ExplorerObjectType::kShapes:
    case ExplorerObjectType::kPointLight:
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

      if (m_type == ExplorerObjectType::kPointLight) {
        Begin("Lighting Manager");
        InputFloat("Constant", &m_pt_light_prop->m_constant);
        InputFloat("Linear", &m_pt_light_prop->m_linear);
        InputFloat("Quadratic", &m_pt_light_prop->m_quadratic);
        InputFloat("Ambient X", &m_pt_light_prop->m_ambient.x);
        InputFloat("Ambient Y", &m_pt_light_prop->m_ambient.y);
        InputFloat("Ambient Z", &m_pt_light_prop->m_ambient.z);
        InputFloat("Diffuse X", &m_pt_light_prop->m_diffuse.x);
        InputFloat("Diffuse Y", &m_pt_light_prop->m_diffuse.y);
        InputFloat("Diffuse Z", &m_pt_light_prop->m_diffuse.z);
        InputFloat("Specular X", &m_pt_light_prop->m_specular.x);
        InputFloat("Specular Y", &m_pt_light_prop->m_specular.y);
        InputFloat("Specular Z", &m_pt_light_prop->m_specular.z);
        Checkbox("Active", &m_pt_light_prop->m_active);
      }

      break;
  }
}