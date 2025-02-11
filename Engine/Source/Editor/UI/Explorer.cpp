#include "Explorer.h"
#include "Editor/Camera/Camera.h"
#include "Main/Core/Application.h"
#include "ToolboxObj.h"

using namespace ImGui;
using namespace Yeager;

void EditorExplorer::DrawExplorer()
{
  AddImportedObjectWindow();
  SameLine();
  AddAudioWindow();
  SameLine();
  AddGeometryObjectWindow();
  Separator();

  Interface::CenteredText(ICON_FA_CAMERA " [Main Scene]");
  BeginChild("Scene");
  DrawToolboxesList();
  EndChild();
}

EditorExplorer::EditorExplorer(Yeager::ApplicationCore* app) : m_Application(app)
{
  Yeager::Log(INFO, "Creating Editor Explorer");
}

EditorExplorer::~EditorExplorer()
{
  m_FileSelection.reset();
  m_FolderSelection.reset();
}

void EditorExplorer::AsynchronousAwaitUserTextureFolder()
{
  if (m_FolderSelection->ready(2)) {

    if (m_FolderSelection->result().empty()) {  // If the user clicks in cancel
      Yeager::LogDebug(INFO, "User cancelled the folder selection operation");
      m_FolderSelection.reset();
      m_AwaintingUserChoiceFolder = false;
      return;
    }

    const String path = m_FolderSelection->result();

    if (!std::filesystem::is_directory(path)) {
      Yeager::LogDebug(ERROR, "Selected path is not a folder for textures! {}", path);
      m_CreationConfiguration.TextureFolder.Valid = false;
    } else {
      m_CreationConfiguration.TextureFolder.Valid = true;
      m_CreationConfiguration.TextureFolder.path = path;
    }

    m_AwaintingUserChoiceFolder = false;
    m_FolderSelection.reset();
  }
}

void EditorExplorer::AsynchronousAwaitUserFolder(FileExtensionType type)
{
  if (m_FileSelection->ready(2)) {

    if (m_FileSelection->result().empty()) {  // If the user clicks in cancel
      Yeager::LogDebug(INFO, "User cancelled the file selection operation");
      m_FileSelection.reset();
      m_AwaitingUserChoiceFile = false;
      return;
    }

    const String path = m_FileSelection->result()[0];

    const std::pair<bool, FileType> result = CheckFileExtensionType(path, type);
    bool success = true;

    if (!result.first || !result.second.Supported) {
      m_Application->GetInterface()->AddWarningWindow("The extension of the file selected is not supported!");
      success = false;
    }

    if (!Yeager::ValidatesPath(path)) {
      m_Application->GetInterface()->AddWarningWindow("The file path is not valid!");
      success = false;
    }

    if (success)
      m_NewObjectPath = path;

    m_AwaitingUserChoiceFile = false;
    m_FileSelection.reset();
  }
}

void EditorExplorer::AddAudioWindow()
{

  if (Button(ICON_FA_MUSIC " Add Audio")) {
    if (!m_AddGeometryWindowOpen && !m_AddImportedObjectWindowOpen) {
      m_AddAudioWindowOpen = true;
      m_SelectableOptions = m_Application->GetScene()->VerifySoundsOptionsInAssetFolder();
    }
  }

  if (m_AddAudioWindowOpen) {

    m_Application->GetInput()->SetCameraCursorToWindowState(true);  // Stops camera and release cursor
    m_Application->GetInterface()->CenteredWindow(600, 250);

    Begin(ICON_FA_PLUS " Add Audio Entity", NULL, YEAGER_WINDOW_STATIC);
    InputText(ICON_FA_PENCIL " Audio's name", &m_NewObjectName, YEAGER_EXPLORER_MAX_STRING_INPUT);

    if (Button(ICON_FA_FILE " Select Audio file")) {
      StartFolderSelection("Select Audio file");
    }

    if (m_AwaitingUserChoiceFile) {  // asynchronous file select
      AsynchronousAwaitUserFolder(EExtensionTypeAudio);
    }

    Text(ICON_FA_ROAD " Path: %s", m_NewObjectPath.c_str());

    Checkbox(ICON_FA_LOCK " Looped", &m_LoopedAudio);
    SameLine();
    Checkbox(ICON_FA_CUBE " 3D Audio", &m_AddAudioIs3D);

    HandleSelectableOptions("Sounds Options in Assets folder");

    if (Button(ICON_FA_DOWNLOAD " Create")) {
      HandleAudioCreation();
    }

    SameLine();
    if (Button(ICON_FA_X " Cancel")) {
      HandleAudioCleanup();
    }
    End();
  }
}

void EditorExplorer::HandleAudioCleanup()
{
  m_NewObjectName.clear();
  m_NewObjectPath.clear();
  m_EverythingFineToCreate = false;
  m_AddAudioWindowOpen = false;
  m_AddAudioIs3D = false;
  m_Application->GetInput()->SetCameraCursorToWindowState(false);
  m_AwaitingUserChoiceFile = false;
  m_SelectableOptions.clear();
  m_FileSelection.reset();
}

void EditorExplorer::HandleAudioCreation()
{
  if (m_NewObjectName.empty() || m_NewObjectPath.empty()) {
    m_Application->GetInterface()->AddWarningWindow("Audio must have a name and path!");
    m_EverythingFineToCreate = false;
  }

  if (m_EverythingFineToCreate) {
    if (m_AddAudioIs3D) {
      auto audio = BaseAllocator::MakeSharedPtr<Yeager::Audio3DHandle>(
          EntityBuilder(m_Application, m_NewObjectName), m_NewObjectPath, m_Application->GetAudioEngineHandle(),
          m_LoopedAudio, irrklang::vec3df(0.0f, 0.0f, 0.0f));

      m_Application->GetScene()->GetAudios3D()->push_back(audio);

    } else {
      auto audio = BaseAllocator::MakeSharedPtr<Yeager::AudioHandle>(
          EntityBuilder(m_Application, m_NewObjectName), m_NewObjectPath, m_Application->GetAudioEngineHandle(),
          m_LoopedAudio);

      m_Application->GetScene()->GetAudios()->push_back(audio);
    }
    HandleAudioCleanup();
  }
  m_EverythingFineToCreate = true;
}

void EditorExplorer::HandleSelectableOptions(const String& name)
{
  if (CollapsingHeader(name.c_str())) {
    int index = 0;
    for (auto& obj : m_SelectableOptions) {
      PushID(index++);
      if (Button(obj.first.c_str())) {
        m_NewObjectPath = obj.second;
        if (m_NewObjectName.empty() || m_NewObjectName == YEAGER_NULL_LITERAL) {
          m_NewObjectName = Yeager::RemoveExtensionFromFilename(obj.first);
        }
      }
      PopID();
    }
  }
}

void EditorExplorer::HandleGeometryCleanup()
{
  m_NewObjectName.clear();
  m_AddGeometryShapeCube = false;
  m_AddImportedObjectWindowOpen = false;
  m_Application->GetCamera()->SetShouldMove(true);
  m_Application->GetInput()->SetCursorCanDisappear(true);
  m_Application->GetInput()->SetCameraCursorToWindowState(false);
}

void EditorExplorer::HandleGeometryCreation()
{
  if (m_NewObjectName.empty() || !m_AddGeometryShapeCube) {
    m_Application->GetInterface()->AddWarningWindow("Geometry must have a name and shape!");
    m_EverythingFineToCreate = false;
  }

  if (m_EverythingFineToCreate) {
    Yeager::ObjectGeometryType::Enum shape;
    if (m_AddGeometryShapeCube) {
      shape = Yeager::ObjectGeometryType::eSPHERE;
    }
    auto obj = BaseAllocator::MakeSharedPtr<Yeager::Object>(EntityBuilder(m_Application, m_NewObjectName));
    obj->GenerateObjectGeometry(shape, ObjectPhysXCreationStatic(Vector3(0.0f)));
    m_Application->GetScene()->GetObjects()->push_back(obj);
    HandleGeometryCleanup();
  }
  m_EverythingFineToCreate = true;
}

void EditorExplorer::AddGeometryObjectWindow()
{
  if (Button(ICON_FA_BOX_OPEN " Add Geometry Object")) {
    if (!m_AddAudioWindowOpen && !m_AddImportedObjectWindowOpen) {
      m_AddGeometryWindowOpen = true;
    }
  }
  if (m_AddGeometryWindowOpen) {

    m_Application->GetInput()->SetCameraCursorToWindowState(true);
    m_Application->GetInterface()->CenteredWindow(400, 150);

    Begin("Add Geometry", NULL, YEAGER_WINDOW_STATIC);
    InputText("Geometry's name", &m_NewObjectName, YEAGER_EXPLORER_MAX_STRING_INPUT);

    Text("Shape");
    SameLine();
    Checkbox("Cube", &m_AddGeometryShapeCube);

    if (Button("Create")) {
      HandleGeometryCreation();
    }
    SameLine();
    if (Button("Cancel")) {
      HandleGeometryCleanup();
    }
    End();
  }
}

void EditorExplorer::QuickLoadObject(const std::filesystem::path& path)
{

  /* Forces the windows to open */
  if (!m_AddGeometryWindowOpen && !m_AddAudioWindowOpen) {
    m_AddImportedObjectWindowOpen = true;
    m_SelectableOptions = m_Application->GetScene()->VerifyImportedModelsOptionsInAssetsFolder();
  }

  m_NewObjectPath = path.string();                                          // Adds the path loaded
  m_NewObjectName = RemoveExtensionFromFilename(path.filename().string());  // Gives the file name to the object
}

void EditorExplorer::AddImportedObjectWindow()
{
  if (Button(ICON_FA_DOWNLOAD " Add Imported Object")) {
    if (!m_AddGeometryWindowOpen && !m_AddAudioWindowOpen) {
      m_AddImportedObjectWindowOpen = true;
      m_SelectableOptions = m_Application->GetScene()->VerifyImportedModelsOptionsInAssetsFolder();
    }
  }
  if (m_AddImportedObjectWindowOpen) {
    m_Application->GetInput()->SetCameraCursorToWindowState(true);
    m_Application->GetInterface()->CenteredWindow(600, 300);

    Begin("Add Imported Object", NULL, YEAGER_WINDOW_STATIC);
    InputText("Object's name", &m_NewObjectName, YEAGER_EXPLORER_MAX_STRING_INPUT);

    if (Button("Select Imported Object file")) {
      StartFolderSelection("Select 3D model file");
    }

    if (Button(ICON_FA_FILE " Select Custom Texture Folder")) {
      StartFolderSelectionFromTexture("Select texture folder");
    }

    if (m_AwaitingUserChoiceFile) {  // asynchronous file select
      AsynchronousAwaitUserFolder(EExtensitonType3DModel);
    }

    if (m_AwaintingUserChoiceFolder) {
      AsynchronousAwaitUserTextureFolder();
    }

    if (m_CreationConfiguration.TextureFolder.Valid) {
      Text("Custom texture folder: %s", m_CreationConfiguration.TextureFolder.path.c_str());
    }

    Text("Path: %s", m_NewObjectPath.c_str());
    m_Application->GetCamera()->SetShouldMove(false);
    m_Application->GetInput()->SetCursorCanDisappear(false);

    HandleSelectableOptions("Models Options in Assets folder");

    Checkbox("Flip texture", &m_ImportedObjectFlipTexture);
    Checkbox("Animated", &m_AddObjectIsAnimated);
    Checkbox("Instanced", &m_AddObjectIsInstanced);
    Checkbox("Playable", &m_AddObjectIsPlayable);

    if (m_AddObjectIsInstanced) {
      InputInt("Grid factor", &m_InstancedGridFactor);
      InputInt("Count", &m_InstancedObjectsCount);
    }

    if (Button("Create")) {
      HandleObjectCreation();
    }
    SameLine();
    if (Button("Cancel")) {
      CleanupAfterObjectCreation();
    }
    End();
  }
}

void EditorExplorer::StartFolderSelectionFromTexture(const String& title)
{
  if (!m_AwaitingUserChoiceFile && !m_AwaitingUserChoiceFile) {
    m_FolderSelection = BaseAllocator::MakeSharedPtr<pfd::select_folder>(
        title, m_Application->GetScene()->GetContext()->ProjectFolderPath.c_str());
    m_AwaintingUserChoiceFolder = true;
  }
}

void EditorExplorer::StartFolderSelection(const String& title)
{
  if (!m_AwaitingUserChoiceFile && !m_AwaintingUserChoiceFolder) {
    const String path = m_Application->GetScene()->GetContext()->ProjectFolderPath;
    m_FileSelection = BaseAllocator::MakeSharedPtr<pfd::open_file>(title, path);
    m_AwaitingUserChoiceFile = true;
  }
}

void EditorExplorer::BuildInstancedObjectTransformation(std::vector<std::shared_ptr<Transformation3D>>& pos)
{
  for (int x = 0; x < m_InstancedObjectsCount; x++) {
    auto vec = BaseAllocator::MakeSharedPtr<Transformation3D>(Vector3(1 * x * m_InstancedGridFactor, 0, 0), Vector3(0),
                                                              Vector3(1));
    pos.push_back(vec);
  }
}

void EditorExplorer::DeleteInstancedObjectTransformation(std::vector<std::shared_ptr<Transformation3D>>& pos)
{
  for (auto& t : pos) {
    t.reset();
  }
}

void EditorExplorer::HandleObjectCreation()
{
  if (m_NewObjectName.empty() || m_NewObjectPath.empty()) {
    m_Application->GetInterface()->AddWarningWindow("Imported Model must have a name and path!");
    m_EverythingFineToCreate = false;
    m_SelectableOptions.clear();
  }

  if (m_EverythingFineToCreate) {
    if (!m_AddObjectIsAnimated && !m_AddObjectIsPlayable) {
      CreateObject();
    } else if (!m_AddObjectIsPlayable) {
      CreateAnimatedObject();
    } else {
      CreatePlayableObject();
    }
  }
  m_EverythingFineToCreate = true;
}

void EditorExplorer::CreateObject()
{
  auto obj = BaseAllocator::MakeSharedPtr<Yeager::Object>(EntityBuilder(m_Application, m_NewObjectName));
  if (obj->ThreadImportObjectFromFile(m_NewObjectPath.c_str(), m_CreationConfiguration, m_ImportedObjectFlipTexture)) {

    m_Application->GetScene()->GetObjects()->push_back(obj);
    CleanupAfterObjectCreation();
  } else {
    m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
    m_EverythingFineToCreate = false;
  }
}

void EditorExplorer::CreateAnimatedObject()
{
  if (m_AddObjectIsInstanced) {
    auto obj = BaseAllocator::MakeSharedPtr<Yeager::AnimatedObject>(EntityBuilder(m_Application, m_NewObjectName),
                                                                    m_InstancedObjectsCount);
    if (obj->ImportObjectFromFile(m_NewObjectPath.c_str(), m_CreationConfiguration, m_ImportedObjectFlipTexture)) {
      std::vector<std::shared_ptr<Transformation3D>> pos;
      BuildInstancedObjectTransformation(pos);

      obj->BuildAnimation(m_NewObjectPath);
      obj->BuildProps(pos, m_Application->ShaderFromVarName("SimpleAnimated"));

      DeleteInstancedObjectTransformation(pos);  // cleanup smart pointers

      m_Application->GetScene()->GetAnimatedObject()->push_back(obj);
      CleanupAfterObjectCreation();
    } else {
      m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
      m_EverythingFineToCreate = false;
    }
  } else {
    auto obj = BaseAllocator::MakeSharedPtr<Yeager::AnimatedObject>(EntityBuilder(m_Application, m_NewObjectName));
    if (obj->ThreadImportObjectFromFile(m_NewObjectPath.c_str(), m_CreationConfiguration,
                                        m_ImportedObjectFlipTexture)) {
      m_Application->GetScene()->GetAnimatedObject()->push_back(obj);
      CleanupAfterObjectCreation();
    } else {
      m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
      m_EverythingFineToCreate = false;
    }
  }
}

void EditorExplorer::CreatePlayableObject()
{
  if (!m_AddObjectIsAnimated) {
    auto obj = BaseAllocator::MakeSharedPtr<PlayableObject>(EntityBuilder(m_Application, m_NewObjectName));
    if (obj->ThreadImportObjectFromFile(m_NewObjectPath.c_str(), m_CreationConfiguration,
                                        m_ImportedObjectFlipTexture)) {
      m_Application->GetScene()->GetObjects()->push_back(obj);
      CleanupAfterObjectCreation();
    } else {
      m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
      m_EverythingFineToCreate = false;
    }
  } else {
    auto obj = BaseAllocator::MakeSharedPtr<PlayableAnimatedObject>(EntityBuilder(m_Application, m_NewObjectName));
    if (obj->ThreadImportObjectFromFile(m_NewObjectPath.c_str(), m_CreationConfiguration,
                                        m_ImportedObjectFlipTexture)) {
      m_Application->GetScene()->GetAnimatedObject()->push_back(obj);
      CleanupAfterObjectCreation();
    } else {
      m_Application->GetInterface()->AddWarningWindow("Error processing the path from the imported model!");
      m_EverythingFineToCreate = false;
    }
  }
}

void EditorExplorer::CleanupAfterObjectCreation()
{
  m_CreationConfiguration.ResetValues();
  m_NewObjectName.clear();
  m_NewObjectPath.clear();
  m_SelectableOptions.clear();
  m_AddImportedObjectWindowOpen = false;
  m_EverythingFineToCreate = false;
  m_Application->GetCamera()->SetShouldMove(true);
  m_Application->GetInput()->SetCursorCanDisappear(true);
  m_Application->GetInput()->SetCameraCursorToWindowState(false);
  m_AwaitingUserChoiceFile = false;
  m_FileSelection.reset();
}

const String Yeager::GetExplorerSymbolForToolbox(EntityObjectType::Enum type)
{
  switch (type) {
    case EntityObjectType::OBJECT:
    case EntityObjectType::OBJECT_INSTANCED:
      return String(ICON_FA_CUBE);
    case EntityObjectType::OBJECT_ANIMATED:
    case EntityObjectType::OBJECT_INSTANCED_ANIMATED:
      return String(ICON_FA_VIDEO);
    case EntityObjectType::SKYBOX:
      return String(ICON_FA_CLOUD);
    case EntityObjectType::AUDIO_HANDLE:
    case EntityObjectType::AUDIO_3D_HANDLE:
      return String(ICON_FA_MUSIC);
    case EntityObjectType::OBJECT_PLAYABLE:
    case EntityObjectType::OBJECT_ANIMATED_PLAYABLE:
      return String(ICON_FA_CHESS_PAWN);
    default:
      return String(ICON_FA_LOCK);
  }
}

const String Yeager::CreateToolboxLabel(Yeager::ToolboxHandle* obj)
{
  return GetExplorerSymbolForToolbox(obj->GetEntity()->GetEntityType()) + " [" +
         EntityObjectType::ToString(obj->GetEntity()->GetEntityType()) + "] " + obj->GetEntity()->GetName() + "##";
}

void EditorExplorer::DrawToolboxesList()
{
  for (Uint x = 0; x < m_Application->GetScene()->GetToolboxs()->size(); x++) {
    Yeager::ToolboxHandle* obj = m_Application->GetScene()->GetToolboxs()->at(x).get();
    if (!obj->IsSeen())
      continue;

    if (!obj->GetScheduleDeletion()) {
      const String label = CreateToolboxLabel(obj);

      if (Selectable(label.c_str(), obj->IsSelected(), ImGuiSelectableFlags_AllowDoubleClick)) {
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