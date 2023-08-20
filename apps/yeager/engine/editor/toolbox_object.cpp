#include "toolbox_object.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <filesystem>
using namespace ImGui;

void ToolBoxObject::DrawObject() {
    switch(m_type) {
        case ExplorerObjectType::kShapes:
            
            Text("Position:");
            InputFloat("Pos X", &m_position->x);
            InputFloat("Pos Y", &m_position->y);
            InputFloat("Pos Z", &m_position->z);
            
            Text("Rotation");
            SameLine();
            Checkbox("Random Rotation", &m_random_rotation_checkbox);
            if(m_random_rotation_checkbox) {
                float angles = glfwGetTime();
                m_rotation->x = 10.0f * angles;
                m_rotation->y = 20.0f * angles;
                m_rotation->z = 15.0f * angles;
            }
            InputFloat("Rot X", &m_rotation->x);
            InputFloat("Rot Y", &m_rotation->y);
            InputFloat("Rot Z", &m_rotation->z);

    }
}