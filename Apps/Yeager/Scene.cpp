#include "Scene.h"
#include "Application.h"
#include "Engine/Editor/NodeHierarchy.h"
#include "Engine/Renderer/Importer.h"
#include "Engine/Renderer/Skybox.h"
using namespace Yeager;

SceneRenderer Yeager::StringToSceneRenderer(String str)
{
  switch (Yeager::StringToInteger(str.c_str())) {
    case Yeager::StringToInteger("OpenGL3_3"):
      return OpenGL3_3;
    case Yeager::StringToInteger("OpenGL4"):
      return OpenGL4;
    default:
      Yeager::Log(ERROR, "Cannot convert string to scene renderer!");
      return RendererError;
  }
}

String Yeager::SceneTypeToString(SceneType type)
{
  switch (type) {
    case Scene2D:
      return "Scene2D";
    case Scene3D:
      return "Scene3D";
    default:
      return "Error_Invalid_Type";
  }
}
String Yeager::SceneRendererToString(SceneRenderer renderer)
{
  switch (renderer) {
    case OpenGL3_3:
      return "OpenGL3_3";
    case OpenGL4:
      return "OpenGL4";
    default:
      return "Error_Invalid_Renderer";
  }
}

SceneType Yeager::StringToSceneType(String str)
{
  switch (StringToInteger(str.c_str())) {
    case StringToInteger("Scene2D"):
      return Scene2D;
    case StringToInteger("Scene3D"):
      return Scene3D;
    default:
      Yeager::Log(WARNING, "Invalid string to scene type! {}", str);
      return Scene3D;
  };
}

void Scene::InitializeRootNode()
{
  m_RootNodeOfScene = std::make_shared<NodeComponent>((ApplicationCore*)m_Application, (Scene*)this);
  m_SceneContainsRootNode = true;
}

SceneContext Yeager::InitializeContext(String name, String author, SceneType type, String folderPath,
                                       SceneRenderer renderer, TimePointType dateOfCreation)
{
  SceneContext context;
  context.Name = name;
  context.ProjectSceneType = type;
  context.ProjectAuthor = author;
  context.ProjectSceneRenderer = renderer;
  context.ProjectFolderPath = folderPath;
  context.TimeOfCreation = dateOfCreation;
  return context;
}

Scene::Scene(Yeager::ApplicationCore* app) : m_Application(app)  {}

void Scene::BuildScene(const LauncherProjectPicker& project)
{
  m_Context = InitializeContext(project.m_Name, project.m_AuthorName, project.m_SceneType, project.m_ProjectFolderPath, project.m_SceneRenderer, project.m_ProjectDateOfCreation);
  m_Context.ProjectSavePath = GetConfigurationFilePath(m_Context.ProjectFolderPath);
  ValidatesCommonFolders();
  m_AssetsFolderPath = m_Context.ProjectFolderPath + YG_PS + "Assets";
  m_PlayerCamera = std::make_shared<PlayerCamera>(m_Application);
  m_Application->AttachPlayerCamera(m_PlayerCamera);
  m_Skybox = std::make_shared<Yeager::Skybox>(m_Application);

  Log(INFO, "Created Scene name {}", m_Context.Name);
  InitializeRootNode();
  
  if (project.m_TemplateHandle.has_value()) 
     BuildSceneFromTemplate(project.m_TemplateHandle.value());
}

String Scene::GetConfigurationFilePath(String path) const
{
  return path + YG_PS + m_Context.Name + ".yml";
}


void Scene::BuildSceneFromTemplate(const TemplateHandle& handle) {
  m_Template = handle;
  m_Application->GetSerial()->DeserializeTemplateAssetsIntoScene(this, handle.AssetsConfigurationPath);
}

void Scene::RemoveDuplicatesEntities() {}

void Scene::VerifyAssetsSubFolders()
{
  String assetsFolder = m_Context.ProjectFolderPath + YG_PS + "Assets";
  if (!Yeager::ValidatesPath(assetsFolder)) {
    Yeager::Log(WARNING, "Cannot verify assets sub folders! Assets folder doesnt exists!");
    return;
  }

  if (!Yeager::ValidatesPath(assetsFolder + YG_PS + "Sound")) {
    std::filesystem::create_directory(assetsFolder + YG_PS + "Sound");
  }

  if (!Yeager::ValidatesPath(assetsFolder + YG_PS + "ImportedModels")) {
    std::filesystem::create_directory(assetsFolder + YG_PS + "ImportedModels");
  }
}

std::vector<std::pair<String, String>> Scene::VerifyImportedModelsOptionsInAssetsFolder()
{
  String assetsFolder = m_Context.ProjectFolderPath + "Assets";
  std::vector<std::pair<String, String>> models;
  const String importedModelPath = String(assetsFolder + YG_PS + "ImportedModels");
  if (!Yeager::ValidatesPath(importedModelPath)) {
    Yeager::Log(WARNING, "Assets sub folder ImportedModels doesnt not exist, options and help wont be avaliable!");
    return models;
  }

  for (const auto& item : std::filesystem::directory_iterator(assetsFolder + YG_PS + "ImportedModels")) {
    if (item.is_directory()) {
      for (const auto& folder : std::filesystem::directory_iterator(item)) {
        /* check if the file path selected is listed in the engine extensions list and if the extension is supported */
        if (!folder.is_directory()) {
          std::pair<bool, FileType> folder_result =
              CheckFileExtensionType(folder.path().string(), EExtensitonType3DModel);
          if (folder_result.first && folder_result.second.Supported) {
            std::pair<String, String> model;
            String path = folder.path().string();
            model.second = path;
            String name = ReadSuffixUntilCharacter(path, YG_PS);
            name = name.substr(1);
            model.first = name;
            models.push_back(model);
          }
        }
      }
    } else {
      std::pair<bool, FileType> item_result = CheckFileExtensionType(item.path().string(), EExtensitonType3DModel);
      if (item_result.first && item_result.second.Supported) {
        std::pair<String, String> model;
        String path = item.path().string();
        model.second = path;
        String name = ReadSuffixUntilCharacter(path, YG_PS);
        name = name.substr(1);
        model.first = name;
        models.push_back(model);
      }
    }
  }
  return models;
}

std::vector<std::pair<String, String>> Scene::VerifySoundsOptionsInAssetFolder()
{
  String assetsFolder = m_Context.ProjectFolderPath + YG_PS + "Assets";
  std::vector<std::pair<String, String>> audios;
  if (!Yeager::ValidatesPath(assetsFolder + YG_PS + "Sound", false)) {
    Yeager::Log(WARNING, "Assets sub folder Sound doesnt not exist, options and help wont be avaliable!");
    return audios;
  }

  for (const auto& file : std::filesystem::directory_iterator(assetsFolder + YG_PS + "Sound")) {
    String path = file.path().string();

    if (file.path().extension() == ".wav") {
      std::pair<String, String> audio_file;
      audio_file.second = path;
      String name = ReadSuffixUntilCharacter(path, YG_PS);
      // Remove preffix
      name = name.substr(1);
      audio_file.first = name;
      audios.push_back(audio_file);
    }
  }

  return audios;
}

void Scene::CheckDuplicatesLightSources()
{
  auto pos = CheckDuplicatesEntities<PhysicalLightHandle>(&m_LightSources);
  if (!pos.has_value())
    return;
  for (const auto& index : pos.value()) {
    auto light = m_LightSources.at(index).get();

    light->ScheduleDeletionOfPointLights();
    m_LightSources.erase(m_LightSources.begin() + index);
    for (auto& indexMinus : pos.value()) {
      indexMinus--;
    }
  }

  Yeager::LogDebug(INFO, "Removed {} Duplicates Light Sources", pos.value().size());
}

void Scene::CheckScheduleDeletions()
{
  for (Uint x = 0; x < m_Objects.size(); x++) {
    Yeager::Object* obj = m_Objects.at(x).get();
    if (obj->GetScheduleDeletion()) {
      obj->GetToolbox()->SetScheduleDeletion(true);
      m_Objects.erase(m_Objects.begin() + x);
    }
  }

  for (Uint x = 0; x < m_AnimatedObject.size(); x++) {
    Yeager::AnimatedObject* obj = m_AnimatedObject.at(x).get();
    if (obj->GetScheduleDeletion()) {
      obj->GetToolbox()->SetScheduleDeletion(true);
      m_AnimatedObject.erase(m_AnimatedObject.begin() + x);
    }
  }

  CheckToolboxesScheduleDeletions();

  for (Uint x = 0; x < m_LightSources.size(); x++) {
    Yeager::PhysicalLightHandle* light = m_LightSources.at(x).get();
    if (light->GetScheduleDeletion()) {
      m_LightSources.erase(m_LightSources.begin() + x);
    }
  }
}

void Scene::CheckToolboxesScheduleDeletions()
{
  for (Uint x = 0; x < m_Toolboxes.size(); x++) {
    Yeager::ToolboxHandle* obj = m_Toolboxes.at(x).get();
    if (obj->GetScheduleDeletion()) {
      CheckToolboxIsSelectedAndDisable(obj);
      m_Toolboxes.erase(m_Toolboxes.begin() + x);
    }
  }
}

void Scene::CheckToolboxIsSelectedAndDisable(Yeager::ToolboxHandle* toolbox)
{
  if (m_Application->GetExplorer()->GetSelectedToolbox() == toolbox) {
    m_Application->GetExplorer()->ResetSelectedToolbox();
  }
}

void Scene::ValidatesCommonFolders()
{
  const String projectPath = m_Context.ProjectFolderPath + YG_PS;

  if (!Yeager::ValidatesAndCreateDirectory(projectPath + "Assets")) {
    VerifyAssetsSubFolders();
  }

  Yeager::ValidatesAndCreateDirectory(projectPath + "Configuration");
  Yeager::ValidatesAndCreateDirectory(projectPath + "Packages");
  Yeager::ValidatesAndCreateDirectory(projectPath + "Main");

  if (!Yeager::ValidatesAndCreateDirectory(projectPath + "Cache")) {
    VerifyCacheSubFolders();
  }
}

void Scene::DrawSkybox(Yeager::Shader* shader, const Matrix3& view, const Matrix4& projection)
{
  m_Skybox->Draw(shader, view, projection);
}

void Scene::VerifyCacheSubFolders()
{
  const String cacheFolder = String(m_Context.ProjectFolderPath + YG_PS + "Cache");
  if (!Yeager::ValidatesPath(cacheFolder + YG_PS + "Texture")) {
    Yeager::CreateDirectoryAndValidate(cacheFolder + YG_PS + "Texture");
  }
  if (!Yeager::ValidatesPath(cacheFolder + YG_PS + "Object")) {
    Yeager::CreateDirectoryAndValidate(cacheFolder + YG_PS + "Object");
  }
}

String Scene::GetTextureCacheFolderPath() const
{
  return String(m_Context.ProjectFolderPath + YG_PS + "Cache" + YG_PS + "Texture");
}

Scene::~Scene()
{
  if (!m_SceneWasTerminated) {
    Terminate();
  }
  m_Skybox.reset();
}

void Scene::Terminate()
{
  DeleteChildOf(m_RootNodeOfScene);
  m_PlayerCamera.reset();
  m_RootNodeOfScene.reset();
  Yeager::Log(INFO, "Destroring Scene name {}", m_Context.Name);
  m_SceneWasTerminated = true;
}

std::vector<std::pair<ImporterThreaded*, Yeager::Object*>>* Scene::GetThreadImporters()
{
  return &m_ThreadImporters;
}

std::vector<std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>>* Scene::GetThreadAnimatedImporters()
{
  return &m_ThreadAnimatedImporters;
}

void Scene::CheckAndAwaitThreadsToFinish()
{
  Yeager::Log(INFO, "Awaiting threads to finish before closing the program");
  for (Uint x = 0; x < m_ThreadImporters.size(); x++) {
    std::pair<ImporterThreaded*, Yeager::Object*>* obj = &m_ThreadImporters.at(x);
    if (obj->first->IsThreadFinish()) {
      m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
    } else {
      obj->first->GetThreadPtr()->join();
      m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
    }
  }

  for (Uint x = 0; x < m_ThreadAnimatedImporters.size(); x++) {
    std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>* obj = &m_ThreadAnimatedImporters.at(x);
    if (obj->first->IsThreadFinish()) {
      m_ThreadAnimatedImporters.erase(m_ThreadAnimatedImporters.begin() + x);
    } else {
      obj->first->GetThreadPtr()->join();
      m_ThreadAnimatedImporters.erase(m_ThreadAnimatedImporters.begin() + x);
    }
  }
}

void Scene::CheckThreadsAndTriggerActions()
{
  try {
    for (Uint x = 0; x < m_ThreadImporters.size(); x++) {
      std::pair<ImporterThreaded*, Yeager::Object*>* obj = &m_ThreadImporters.at(x);
      if (obj->first->IsThreadFinish()) {
        Yeager::Log(INFO, "Object importer thread {} has finished! Detaching", obj->second->GetName());
        obj->second->ThreadSetup();
        m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
      }
    }

    for (Uint x = 0; x < m_ThreadAnimatedImporters.size(); x++) {
      std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>* obj = &m_ThreadAnimatedImporters.at(x);
      if (obj->first->IsThreadFinish()) {
        Yeager::Log(INFO, "Object importer thread {} has finished! Detaching", obj->second->GetName());
        obj->second->ThreadSetup();
        m_ThreadAnimatedImporters.erase(m_ThreadAnimatedImporters.begin() + x);
      }
    }

  } catch (std::exception e) {
    Yeager::Log(ERROR, "Exception thrown reading thread importers {}", e.what());
  }
}

void Scene::Save()
{
  m_Application->GetSerial()->SerializeScene(this, m_Context.ProjectSavePath);
}

void Scene::LoadEditorColorscheme(Interface* intr)
{
  intr->ApplyColorscheme(m_Application->GetSerial()->ReadColorschemeConfig());
}

void Scene::Load(String path)
{
  m_Application->GetSerial()->DeserializeScene(this, path);
}

void Scene::LoadSceneSave()
{
  m_Application->GetSerial()->DeserializeScene(this, m_Context.ProjectSavePath);
}

void Scene::SetContextType(SceneType type)
{
  m_Context.ProjectSceneType = type;
}
void Scene::SetContextRenderer(SceneRenderer renderer)
{
  m_Context.ProjectSceneRenderer = renderer;
}

std::vector<std::shared_ptr<Yeager::Audio3DHandle>>* Scene::GetAudios3D()
{
  return &m_Audios3D;
}
std::vector<std::shared_ptr<Yeager::AudioHandle>>* Scene::GetAudios()
{
  return &m_Audios;
}

std::vector<std::shared_ptr<ToolboxHandle>>* Scene::GetToolboxs()
{
  return &m_Toolboxes;
}

std::vector<std::shared_ptr<Yeager::Object>>* Scene::GetObjects()
{
  return &m_Objects;
}

std::vector<std::shared_ptr<Yeager::AnimatedObject>>* Scene::GetAnimatedObject()
{
  return &m_AnimatedObject;
}

std::vector<std::shared_ptr<Yeager::PhysicalLightHandle>>* Scene::GetLightSources()
{
  return &m_LightSources;
}
