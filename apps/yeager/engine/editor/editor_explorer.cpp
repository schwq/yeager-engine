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
    case ExplorerObjectType::kPointLight:
      return "Point Light";
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

void EditorExplorer::AddLightPointSource() {
    if( Button("Add Point Light") ) {
        if( !m_add_cube_window_open && !m_add_imported_object_window_open ){
        m_add_light_point_source_window_open = true;
        }
    }

    if( m_add_light_point_source_window_open ){
        m_app->GetInterface()->CenteredWindow(400, 150);
        Begin("Add Point Light", NULL, kWindowStatic);
        InputText("Object`s name: ", &m_new_object_name, 100);
        if( Button("Select Object texture") ) {
          auto selection =
              pfd::open_file("Select Texture to Light", ".",
                             {"Image Files", "*.png *.jpg *.jpeg *.bmp"});
          m_new_object_texture_path = selection.result()[0].c_str();
        }
        SameLine();
        Text("Light Texture path: %s", m_new_object_texture_path.c_str());
        m_app->GetEditorCamera()->SetShouldMove(false);
        m_app->GetInput()->SetCursorCanDisappear(false);
        if( Button("Create") )
            {
            if( m_new_object_name.empty() )
                {
                m_everything_fine_to_create = false;
              m_app->GetInterface()->AddWarningWindow(
                  "Object must have a name!");
              }

            if( m_everything_fine_to_create )
                {
                 // TODO finish here
                }
              m_everything_fine_to_create = true;
            }

    }
}

void EditorExplorer::AddImportedObjectWindow() {
  if (Button("Add Imported Object")) {
    if (!m_add_cube_window_open && !m_add_light_point_source_window_open) {
      m_add_imported_object_window_open = true;
    }
  }
  if (m_add_imported_object_window_open) {
    m_app->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Imported Object", NULL, kWindowStatic);
    InputText("Object's name", &m_new_object_name, 100);

    if (Button("Select Object .obj path")) {
      auto selection =
          pfd::open_file("Select .obj file", ".", {"*.obj"}, false);
      if (!selection.result().empty()) {
        m_new_object_path = selection.result()[0].c_str();
      }
    }

    Text("Path: %s", m_new_object_path.c_str());
    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty() ||
          m_new_object_path.empty()) {
        m_app->GetInterface()->AddWarningWindow(
            "Imported Model must have a name and path!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        ImportedObject* object = new ImportedObject(
            m_new_object_path, m_app, m_new_object_name);
        m_new_object_name.clear();
        m_new_object_path.clear();
        m_add_imported_object_window_open = false;
        m_app->GetEditorCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
      }
      m_everything_fine_to_create = true;
    }
    End();
  }
}

void EditorExplorer::AddCubeWindow() {
  if (Button("Add Cube")) {
    if (!m_add_imported_object_window_open && !m_add_light_point_source_window_open) {
      m_add_cube_window_open = true;
    }
  }

  if (m_add_cube_window_open) {
    m_app->GetInterface()->CenteredWindow(400, 100);

    Begin("Adding cube to scene", NULL, kWindowStatic);
    InputText("Cube Name", &m_new_object_name, 100);

    if (Button("Select Texture")) {
      auto selection =
          pfd::open_file("Select Texture to Cube", ".",
                         {"Image Files", "*.png *.jpg *.jpeg *.bmp"});
      m_new_object_texture_path = selection.result()[0].c_str();
    }

    SameLine();

    Text("Cube Texture path: %s", m_new_object_texture_path.c_str());

    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty()) {

        m_app->GetInterface()->AddWarningWindow("Cube must have a name!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {

        YeagerTexture2D texture = YeagerTexture2D(m_new_object_texture_path.c_str());
        YeagerShape cube = YeagerShape(m_new_object_name, m_app, texture, EngineShapeType::kCube);

        m_app->GetVectorUserCube()->push_back(cube);
        m_app->GetVectorUserTexture2D()->push_back(texture);
        m_new_object_name.clear();
        m_new_object_texture_path.clear();
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
  SameLine();
  AddImportedObjectWindow();

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