#include "Explorer.h"
#include "../../Application.h"
#include "Camera.h"
#include "ToolboxObj.h"
using namespace ImGui;
using namespace Yeager;

EditorExplorer::EditorExplorer(Yeager::ApplicationCore* app) : m_Application(app)
{
  Yeager::Log(INFO, "Creating Editor Explorer");
}

void EditorExplorer::WarningExplorer(YgString msg)
{
  m_EverythingFineToCreate = false;
  m_Application->GetInterface()->AddWarningWindow(msg);
}

void EditorExplorer::AddAudioWindow()
{
  if (Button("Add Audio")) {
    if (!m_AddGeometryWindowOpen && !m_AddImportedObjectWindowOpen) {
      m_AddAudioWindowOpen = true;
    }
  }
  if (m_AddAudioWindowOpen) {
    m_Application->GetInput()->SetCameraCursorToWindowState(true);
    m_Application->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Audio", NULL, kWindowStatic);
    InputText("Audio's name", &m_NewObjectName, 100);

    if (Button("Select Audio file")) {
      auto selection = pfd::open_file(
          "Select Audio file", m_Application->GetScene()->GetContext().m_ProjectFolderPath.c_str(), {"*"}, false);
      if (!selection.result().empty()) {
        m_NewObjectPath = selection.result()[0].c_str();
      }
    }
    Checkbox("Looped", &m_LoopedAudio);
    Checkbox("3D Audio", &m_AddAudioIs3D);

    Text("Path: %s", m_NewObjectPath.c_str());
    m_Application->GetCamera()->SetShouldMove(false);
    m_Application->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_NewObjectName.empty() || m_NewObjectPath.empty()) {
        m_Application->GetInterface()->AddWarningWindow("Audio must have a name and path!");
        m_EverythingFineToCreate = false;
      }

      if (m_EverythingFineToCreate) {
        if (m_AddAudioIs3D) {
          auto audio =
              std::make_shared<Yeager::Audio3DHandle>(m_NewObjectPath, m_NewObjectName, m_Application->GetAudioEngine(),
                                                      m_LoopedAudio, irrklang::vec3df(0.0f, 0.0f, 0.0f));
          auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
          toolbox->SetType(EExplorerTypeAudio3D);
          toolbox->SetEntity(audio.get());
          m_Application->GetScene()->GetAudios3D()->push_back(audio);
          m_Application->GetScene()->GetToolboxs()->push_back(toolbox);

        } else {
          auto audio = std::make_shared<Yeager::AudioHandle>(m_NewObjectPath, m_NewObjectName,
                                                             m_Application->GetAudioEngine(), m_LoopedAudio);
          auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
          toolbox->SetType(EExplorerTypeAudio);
          toolbox->SetEntity(audio.get());
          m_Application->GetScene()->GetAudios()->push_back(audio);
          m_Application->GetScene()->GetToolboxs()->push_back(toolbox);
        }
        m_NewObjectName.clear();
        m_NewObjectPath.clear();
        m_LoopedAudio = false;
        m_AddAudioWindowOpen = false;
        m_AddAudioIs3D = false;
        m_Application->GetInput()->SetCameraCursorToWindowState(false);
      }
      m_EverythingFineToCreate = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_NewObjectName.clear();
      m_NewObjectPath.clear();
      m_EverythingFineToCreate = false;
      m_AddAudioWindowOpen = false;
      m_Application->GetInput()->SetCameraCursorToWindowState(false);
    }
    End();
  }
}

void EditorExplorer::AddGeometryObjectWindow()
{
  if (Button("Add Geometry Object")) {
    if (!m_AddAudioWindowOpen && !m_AddImportedObjectWindowOpen) {
      m_AddGeometryWindowOpen = true;
    }
  }
  if (m_AddGeometryWindowOpen) {
    m_Application->GetInput()->SetCameraCursorToWindowState(true);
    m_Application->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Geometry", NULL, kWindowStatic);
    InputText("Geometry's name", &m_NewObjectName, 100);
    Text("Shape");
    SameLine();
    Checkbox("Cube", &m_AddGeometryShapeCube);

    m_Application->GetCamera()->SetShouldMove(false);
    m_Application->GetInput()->SetCursorCanDisappear(false);

    if (Button("Create")) {
      if (m_NewObjectName.empty() || !m_AddGeometryShapeCube) {
        m_Application->GetInterface()->AddWarningWindow("Geometry must have a name and shape!");
        m_EverythingFineToCreate = false;
      }

      if (m_EverythingFineToCreate) {
        Yeager::ObjectGeometryType shape;
        if (m_AddGeometryShapeCube) {
          shape = Yeager::ObjectGeometryType::ESphere;
        }
        auto obj = std::make_shared<Yeager::Object>(m_NewObjectName, m_Application);
        obj->GenerateObjectGeometry(shape);
        m_Application->GetScene()->GetObjects()->push_back(obj);
        m_NewObjectName.clear();
        m_AddGeometryShapeCube = false;
        m_AddImportedObjectWindowOpen = false;
        m_Application->GetCamera()->SetShouldMove(true);
        m_Application->GetInput()->SetCursorCanDisappear(true);
        m_Application->GetInput()->SetCameraCursorToWindowState(false);
      }
      m_EverythingFineToCreate = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_NewObjectName.clear();
      m_AddGeometryShapeCube = false;
      m_EverythingFineToCreate = false;
      m_AddGeometryWindowOpen = false;
      m_Application->GetCamera()->SetShouldMove(true);
      m_Application->GetInput()->SetCursorCanDisappear(true);
      m_Application->GetInput()->SetCameraCursorToWindowState(false);
    }
    End();
  }
}

void EditorExplorer::AddImportedObjectWindow()
{
  if (Button("Add Imported Object")) {
    if (!m_AddGeometryWindowOpen && !m_AddAudioWindowOpen) {
      m_AddImportedObjectWindowOpen = true;
    }
  }
  if (m_AddImportedObjectWindowOpen) {
    m_Application->GetInput()->SetCameraCursorToWindowState(true);
    m_Application->GetInterface()->CenteredWindow(400, 150);
    Begin("Add Imported Object", NULL, kWindowStatic);
    InputText("Object's name", &m_NewObjectName, 100);

    if (Button("Select Imported Object file")) {
      auto selection = pfd::open_file(
          "Select 3D model file", m_Application->GetScene()->GetContext().m_ProjectFolderPath.c_str(), {"*"}, false);
      if (!selection.result().empty()) {
        m_NewObjectPath = selection.result()[0].c_str();
      }
    }

    Text("Path: %s", m_NewObjectPath.c_str());
    m_Application->GetCamera()->SetShouldMove(false);
    m_Application->GetInput()->SetCursorCanDisappear(false);

    Checkbox("Flip texture", &m_ImportedObjectFlipTexture);
    Checkbox("Animated", &m_AddObjectIsAnimated);
    Checkbox("Instanced", &m_AddObjectIsInstanced);
    if (m_AddObjectIsInstanced) {
      InputInt("Grid factor", &m_InstancedGridFactor);
      InputInt("Count", &m_InstancedObjectsCount);
    }

    if (Button("Create")) {
      if (m_NewObjectName.empty() || m_NewObjectPath.empty()) {
        m_Application->GetInterface()->AddWarningWindow("Imported Model must have a name and path!");
        m_EverythingFineToCreate = false;
      }

      if (m_EverythingFineToCreate) {
        if (!m_AddObjectIsAnimated) {
          auto obj = std::make_shared<Yeager::Object>(m_NewObjectName, m_Application);
          if (obj->ImportObjectFromFile(m_NewObjectPath.c_str(), m_ImportedObjectFlipTexture)) {
            m_Application->GetScene()->GetObjects()->push_back(obj);
            m_NewObjectName.clear();
            m_NewObjectPath.clear();
            m_AddImportedObjectWindowOpen = false;
            m_Application->GetCamera()->SetShouldMove(true);
            m_Application->GetInput()->SetCursorCanDisappear(true);
            m_Application->GetInput()->SetCameraCursorToWindowState(false);
          } else {
            m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
            m_EverythingFineToCreate = false;
          }
        } else {
          if (m_AddObjectIsInstanced) {
            auto obj = std::make_shared<Yeager::InstancedAnimatedObject>(m_NewObjectName, m_Application,
                                                                         m_InstancedObjectsCount);
            if (obj->ImportObjectFromFile(m_NewObjectPath.c_str(), m_ImportedObjectFlipTexture)) {
              std::vector<YgVector3> pos;
              for (int x = 0; x < m_InstancedObjectsCount; x++) {
                YgVector3 vec;
                vec.x = 1 * x * m_InstancedGridFactor;
                vec.y = 0;
                vec.z = 0;
                pos.push_back(vec);
              }
              obj->BuildAnimation(m_NewObjectPath);
              //obj->BuildProp(pos, ShaderFromVarName("SimpleAnimated"));
              m_Application->GetScene()->GetInstancedAnimatedObjects()->push_back(obj);
              m_NewObjectName.clear();
              m_NewObjectPath.clear();
              m_AddImportedObjectWindowOpen = false;
              m_Application->GetCamera()->SetShouldMove(true);
              m_Application->GetInput()->SetCursorCanDisappear(true);
              m_Application->GetInput()->SetCameraCursorToWindowState(false);
            } else {
              m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
              m_EverythingFineToCreate = false;
            }
          } else {
            auto obj = std::make_shared<Yeager::AnimatedObject>(m_NewObjectName, m_Application);
            if (obj->ImportObjectFromFile(m_NewObjectPath.c_str(), m_ImportedObjectFlipTexture)) {
              obj->BuildAnimation(m_NewObjectPath);
              m_Application->GetScene()->GetAnimatedObject()->push_back(obj);
              m_NewObjectName.clear();
              m_NewObjectPath.clear();
              m_AddImportedObjectWindowOpen = false;
              m_Application->GetCamera()->SetShouldMove(true);
              m_Application->GetInput()->SetCursorCanDisappear(true);
              m_Application->GetInput()->SetCameraCursorToWindowState(false);
            } else {
              m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
              m_EverythingFineToCreate = false;
            }
          }
        }
      }
      m_EverythingFineToCreate = true;
    }
    SameLine();
    if (Button("Cancel")) {
      m_NewObjectName.clear();
      m_NewObjectPath.clear();
      m_EverythingFineToCreate = false;
      m_AddImportedObjectWindowOpen = false;
      m_Application->GetCamera()->SetShouldMove(true);
      m_Application->GetInput()->SetCursorCanDisappear(true);
      m_Application->GetInput()->SetCameraCursorToWindowState(false);
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
  for (unsigned int x = 0; x < m_Application->GetScene()->GetToolboxs()->size(); x++) {
    Yeager::ToolBoxObject* obj = m_Application->GetScene()->GetToolboxs()->at(x).get();
    if (obj->GetEntity()->IsValid()) {
      YgString label = obj->GetEntity()->GetName() + "##";
      // = "[" + ExplorerTypeToString(obj->GetType()) + "] " + obj->GetEntity()->GetName();
      if (Selectable(label.c_str(), &obj->m_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
        m_FirstTimeToolbox = false;
        m_ToolboxSelected = m_Application->GetScene()->GetToolboxs()->at(x).get();
      }
    } else {
      m_ToolboxSelected = YEAGER_NULLPTR;
    }
  }
  if (m_FirstTimeToolbox) {
    m_ToolboxSelected = YEAGER_NULLPTR;
  }
}