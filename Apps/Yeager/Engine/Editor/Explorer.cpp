#include "Explorer.h"
#include "Camera.h"
#include "ToolboxObj.h"
using namespace ImGui;

yg_uint ExplorerObject::m_object_count = 0;

EditorExplorer::EditorExplorer(Application* app)
    : m_app(app){

      };

ExplorerObject::ExplorerObject(Yeager::ToolBoxObject object_toolbox)
    : m_object_toolbox(object_toolbox)
{
  m_id = m_object_count++;
}

void EditorExplorer::AddObject(ExplorerObject object)
{
  m_objects.push_back(object);
}

void EditorExplorer::AddObject(Yeager::ToolBoxObject toolbox_object)
{
  ExplorerObject object(toolbox_object);
  m_objects.push_back(object);
}

void EditorExplorer::WarningExplorer(yg_string msg)
{
  m_everything_fine_to_create = false;
  m_app->GetInterface()->AddWarningWindow(msg);
}

void EditorExplorer::AddAudioWindow()
{
  if (Button("Add Audio")) {
    if (!m_add_cube_window_open && !m_add_imported_object_window_open &&
        !m_add_light_point_source_window_open) {
      m_add_audio_window_open = true;
    }
  }
  if (m_add_audio_window_open) {
    m_app->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Audio", NULL, kWindowStatic);
    InputText("Audio's name", &m_new_object_name, 100);

    if (Button("Select Audio file")) {
      auto selection = pfd::open_file(
          "Select .obj file", GetPath("/Assets/sound/").c_str(), {"*"}, false);
      if (!selection.result().empty()) {
        m_new_object_path = selection.result()[0].c_str();
      }
    }
    Checkbox("Looped", &m_looped_audio);

    Text("Path: %s", m_new_object_path.c_str());
    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty() || m_new_object_path.empty()) {
        m_app->GetInterface()->AddWarningWindow(
            "Audio must have a name and path!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        std::shared_ptr<Yeager::AudioHandle> audio(new Yeager::AudioHandle(
            m_new_object_path, m_new_object_name, m_looped_audio));
        std::shared_ptr<Yeager::ToolBoxObject> toolbox(
            new Yeager::ToolBoxObject());
        toolbox.get()->SetType(ExplorerObjectType::kAudio);
        toolbox.get()->SetAudio(audio.get());
        m_audios.push_back(audio);
        m_toolboxs.push_back(toolbox);
        m_new_object_name.clear();
        m_new_object_path.clear();
        m_looped_audio = false;
        m_add_audio_window_open = false;
        m_app->GetEditorCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
      }
      m_everything_fine_to_create = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_new_object_name.clear();
      m_new_object_path.clear();
      m_everything_fine_to_create = false;
      m_add_audio_window_open = false;
      m_app->GetEditorCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
    }
    End();
  }
}

void EditorExplorer::AddImportedObjectWindow()
{
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
      auto selection = pfd::open_file(
          "Select .obj file", GetPath("/Assets/imported_models/").c_str(),
          {"*.obj"}, false);
      if (!selection.result().empty()) {
        m_new_object_path = selection.result()[0].c_str();
      }
    }

    Text("Path: %s", m_new_object_path.c_str());
    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_new_object_name.empty() || m_new_object_path.empty()) {
        m_app->GetInterface()->AddWarningWindow(
            "Imported Model must have a name and path!");
        m_everything_fine_to_create = false;
      }

      if (m_everything_fine_to_create) {
        ImportedObject object(m_new_object_path, m_app, m_new_object_name);
        m_app->GetImportedObjects()->push_back(object);
        m_new_object_name.clear();
        m_new_object_path.clear();
        m_add_imported_object_window_open = false;
        m_app->GetEditorCamera()->SetShouldMove(true);
        m_app->GetInput()->SetCursorCanDisappear(true);
      }
      m_everything_fine_to_create = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_new_object_name.clear();
      m_new_object_path.clear();
      m_everything_fine_to_create = false;
      m_add_imported_object_window_open = false;
      m_app->GetEditorCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
    }
    End();
  }
}

void EditorExplorer::DrawExplorer()
{
  AddImportedObjectWindow();
  SameLine();
  AddAudioWindow();

  Text("Main Scene");
  for (yg_uint x = 0; x < m_objects.size(); x++) {
    yg_string label =
        "(" + ExplorerTypeToString(m_objects[x].m_object_toolbox.GetType()) +
        ") " + m_objects[x].m_object_toolbox.GetEntity()->GetName();

    if (Selectable(label.c_str(), &m_objects[x].m_selected,
                   ImGuiSelectableFlags_AllowDoubleClick)) {
      m_first_time_toolbox = false;
      toolbox_selected = &m_objects[x];
    };
  }
  if (m_first_time_toolbox) {
    toolbox_selected = nullptr;
  }
}