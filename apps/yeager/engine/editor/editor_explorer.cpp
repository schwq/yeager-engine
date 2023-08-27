#include "editor_explorer.h"
#include "camera.h"
#include "toolbox_object.h"
using namespace ImGui;

uint ExplorerObject::m_object_count = 0;

String ExplorerTypeToString(ExplorerObjectType type) {
  switch (type) {
    case ExplorerObjectType::kShapes:
      return "Shape";
    case ExplorerObjectType::kShader:
      return "Shader";
    case ExplorerObjectType::kTexture:
      return "Texture";
    case ExplorerObjectType::kSkybox:
      return "Skybox";
    case ExplorerObjectType::kScene:
      return "Scene";
    case ExplorerObjectType::kPlayer:
      return "Player";
    case ExplorerObjectType::kNetwork:
      return "Network";
    case ExplorerObjectType::kImage:
      return "Image";
    case ExplorerObjectType::kVideo:
      return "Video";
    case ExplorerObjectType::kAudio:
      return "Audio";
    case ExplorerObjectType::kImportedObject:
      return "Imported Object";
    case ExplorerObjectType::kNone:
    case ExplorerObjectType::kNoChange:
    default:
      return "NOT_FOUND_ERROR";
  }
}

ExplorerObject::ExplorerObject(const char* name, ExplorerObjectType type,
                               ToolBoxObject* object_toolbox)
    : m_name(name), m_type(type), m_object_toolbox(object_toolbox) {
  m_id = m_object_count++;
}

void EditorExplorer::AddObject(ExplorerObject object) {
  m_objects.push_back(object);
}

void EditorExplorer::AddObject(const char* name, ExplorerObjectType type,
                               ToolBoxObject* toolbox_object) {
  ExplorerObject object(name, type, toolbox_object);
  m_objects.push_back(object);
}

void EditorExplorer::AddCubeWindow() {
  if (Button("Add Cube")) {
    m_add_cube_window_open = true;
  }

  if (m_add_cube_window_open) {
    m_app->GetInterface()->CenteredWindow(400, 100);

    Begin("Adding cube to scene");
    InputText("Cube Name", &m_new_cube_name, 100);

    if (Button("Select Texture")) {
      auto selection =
          pfd::open_file("Select Texture to Cube", ".",
                         {"Image Files", "*.png *.jpg *.jpeg *.bmp"});
      m_new_cube_texture_path = selection.result()[0].c_str();
    }

    SameLine();

    Text("Cube Texture path: %s", m_new_cube_texture_path.c_str());

    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_cube_name.empty()) {

        m_app->GetInterface()->AddWarningWindow("Cube must have a name!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {

        EngineTexture2D* texture =
            new EngineTexture2D(m_new_cube_texture_path.c_str(), m_app);
        YeagerCube* cube =
            new YeagerCube(m_new_cube_name.c_str(), m_app, texture);

        m_app->GetVectorUserCube()->push_back(cube);
        m_app->GetVectorUserTexture2D()->push_back(texture);
        m_new_cube_name.clear();
        m_new_cube_texture_path.clear();
        m_add_cube_window_open = false;
        m_app->GetEditorCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
      }

      m_everything_fine_to_create = true;
    }

    End();
  }
}

void EditorExplorer::DrawExplorer() {

  AddCubeWindow();

  Text("Main Scene");
  for (uint x = 0; x < m_objects.size(); x++) {
    String label = "(" + ExplorerTypeToString(m_objects[x].m_type) + ") " +
                   m_objects[x].m_name;

    if (Selectable(label.c_str(), &m_objects[x].m_selected,
                   ImGuiSelectableFlags_AllowDoubleClick)) {
      m_first_time_toolbox = false;
      toolbox_selected = m_objects[x];
    };
  }
  if (m_first_time_toolbox) {
    toolbox_selected = ExplorerObject("NONE", ExplorerObjectType::kNone);
  }
}