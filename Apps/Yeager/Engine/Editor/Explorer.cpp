#include "Explorer.h"
#include "../../Application.h"
#include "Camera.h"
#include "ToolboxObj.h"
using namespace ImGui;

EditorExplorer::EditorExplorer(Yeager::ApplicationCore* app) : m_app(app)
{
  Yeager::Log(INFO, "Creating Editor Explorer");
}

void EditorExplorer::WarningExplorer(YgString msg)
{
  m_everything_fine_to_create = false;
  m_app->GetInterface()->AddWarningWindow(msg);
}

void EditorExplorer::AddAudioWindow()
{
  if (Button("Add Audio")) {
    if (!m_add_geometry_window_open && !m_add_imported_object_window_open) {
      m_add_audio_window_open = true;
    }
  }
  if (m_add_audio_window_open) {
    m_app->GetInput()->SetCameraCursorToWindowState(true);
    m_app->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Audio", NULL, kWindowStatic);
    InputText("Audio's name", &m_new_object_name, 100);

    if (Button("Select Audio file")) {
      auto selection = pfd::open_file("Select .obj file", GetPath("/Assets/sound/").c_str(), {"*"}, false);
      if (!selection.result().empty()) {
        m_new_object_path = selection.result()[0].c_str();
      }
    }
    Checkbox("Looped", &m_looped_audio);
    Checkbox("3D Audio", &m_add_audio_is_3d);

    Text("Path: %s", m_new_object_path.c_str());
    m_app->GetCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty() || m_new_object_path.empty()) {
        m_app->GetInterface()->AddWarningWindow("Audio must have a name and path!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        if (m_add_audio_is_3d) {
          auto audio = std::make_shared<Yeager::Audio3DHandle>(m_new_object_path, m_new_object_name, m_looped_audio,
                                                               irrklang::vec3df(0.0f, 0.0f, 0.0f));
          auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
          toolbox->SetType(ExplorerObjectType::k3DAudio);
          toolbox->SetAudio(audio.get());
          m_app->GetScene()->GetAudios3D()->push_back(audio);
          m_toolboxs.push_back(toolbox);
        } else {
          auto audio = std::make_shared<Yeager::AudioHandle>(m_new_object_path, m_new_object_name, m_looped_audio);
          auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
          toolbox->SetType(ExplorerObjectType::kAudio);
          toolbox->SetAudio(audio.get());
          m_app->GetScene()->GetAudios()->push_back(audio);
          m_toolboxs.push_back(toolbox);
        }
        m_new_object_name.clear();
        m_new_object_path.clear();
        m_looped_audio = false;
        m_add_audio_window_open = false;
        m_add_audio_is_3d = false;
        m_app->GetInput()->SetCameraCursorToWindowState(false);
      }
      m_everything_fine_to_create = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_new_object_name.clear();
      m_new_object_path.clear();
      m_everything_fine_to_create = false;
      m_add_audio_window_open = false;
      m_app->GetInput()->SetCameraCursorToWindowState(false);
    }
    End();
  }
}

void EditorExplorer::AddGeometryObjectWindow()
{
  if (Button("Add Geometry Object")) {
    if (!m_add_audio_window_open && !m_add_imported_object_window_open) {
      m_add_geometry_window_open = true;
    }
  }
  if (m_add_geometry_window_open) {
    m_app->GetInput()->SetCameraCursorToWindowState(true);
    m_app->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Geometry", NULL, kWindowStatic);
    InputText("Geometry's name", &m_new_object_name, 100);
    Text("Shape");
    SameLine();
    Checkbox("Cube", &m_add_geometry_shape_cube);

    m_app->GetCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty() || !m_add_geometry_shape_cube) {
        m_app->GetInterface()->AddWarningWindow("Geometry must have a name and shape!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        Yeager::GeometryShape shape;
        if (m_add_geometry_shape_cube) {
          shape = Yeager::kCube;
        }
        auto geometry = std::make_shared<Yeager::Geometry>(m_new_object_name, YgVector3(1.0f), shape, m_app);
        m_app->GetScene()->GetGeometry()->push_back(geometry);
        m_new_object_name.clear();
        m_add_geometry_shape_cube = false;
        m_add_imported_object_window_open = false;
        m_app->GetCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
        m_app->GetInput()->SetCameraCursorToWindowState(false);
      }
      m_everything_fine_to_create = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_new_object_name.clear();
      m_add_geometry_shape_cube = false;
      m_everything_fine_to_create = false;
      m_add_geometry_window_open = false;
      m_app->GetCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
      m_app->GetInput()->SetCameraCursorToWindowState(false);
    }
    End();
  }
}

void EditorExplorer::AddImportedObjectWindow()
{
  if (Button("Add Imported Object")) {
    if (!m_add_geometry_window_open && !m_add_audio_window_open) {
      m_add_imported_object_window_open = true;
    }
  }
  if (m_add_imported_object_window_open) {
    m_app->GetInput()->SetCameraCursorToWindowState(true);
    m_app->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Imported Object", NULL, kWindowStatic);
    InputText("Object's name", &m_new_object_name, 100);

    if (Button("Select Imported Object file")) {
      auto selection = pfd::open_file("Select .obj file", GetPath("/Assets/imported_models/").c_str(), {"*"}, false);
      if (!selection.result().empty()) {
        m_new_object_path = selection.result()[0].c_str();
      }
    }

    Text("Path: %s", m_new_object_path.c_str());
    m_app->GetCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    Checkbox("Flip texture", &m_imported_object_flip_tex);

    if (Button("Create")) {
      if (m_new_object_name.empty() || m_new_object_path.empty()) {
        m_app->GetInterface()->AddWarningWindow("Imported Model must have a name and path!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        auto object =
            std::make_shared<ImportedObject>(m_new_object_path, m_app, m_new_object_name, m_imported_object_flip_tex);
        m_app->GetScene()->GetImportedObjects()->push_back(object);
        m_new_object_name.clear();
        m_new_object_path.clear();
        m_add_imported_object_window_open = false;
        m_app->GetCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
        m_app->GetInput()->SetCameraCursorToWindowState(false);
      }
      m_everything_fine_to_create = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_new_object_name.clear();
      m_new_object_path.clear();
      m_everything_fine_to_create = false;
      m_add_imported_object_window_open = false;
      m_app->GetCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
      m_app->GetInput()->SetCameraCursorToWindowState(false);
    }
    End();
  }
}

void EditorExplorer::DrawExplorer()
{
  AddImportedObjectWindow();
  SameLine();
  AddAudioWindow();
  SameLine();
  AddGeometryObjectWindow();

  Text("Main Scene");
  for (unsigned int x = 0; x < m_app->GetScene()->GetToolboxs()->size(); x++) {
    Yeager::ToolBoxObject* obj = m_app->GetScene()->GetToolboxs()->at(x).get();
    YgString label = "[" + ExplorerTypeToString(obj->GetType()) + "] " + obj->GetEntity()->GetName();
    if (Selectable(label.c_str(), &obj->m_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
      m_first_time_toolbox = false;
      toolbox_selected = m_app->GetScene()->GetToolboxs()->at(x).get();
    }
  }
  if (m_first_time_toolbox) {
    toolbox_selected = nullptr;
  }
}