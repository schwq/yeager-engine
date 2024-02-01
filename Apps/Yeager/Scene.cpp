#include "Scene.h"
#include "Application.h"
#include "Engine/Renderer/Importer.h"
using namespace Yeager;

SceneRenderer Yeager::StringToSceneRenderer(YgString str)
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

YgString Yeager::SceneTypeToString(SceneType type)
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
YgString Yeager::SceneRendererToString(SceneRenderer renderer)
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

SceneType Yeager::StringToSceneType(YgString str)
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

Scene::Scene(YgString name, YgString Author, SceneType type, YgString folder_path, SceneRenderer renderer,
             Yeager::ApplicationCore* app)
    : m_Application(app)
{
  m_Context.m_name = name;
  m_Context.m_ExplorerType = type;
  m_Context.m_ProjectAuthor = Author;
  m_Context.m_renderer = renderer;
  m_Context.m_ProjectFolderPath = folder_path;
  m_Context.m_ProjectSavePath = GetConfigurationFilePath(m_Context.m_ProjectFolderPath);
  ValidatesCommonFolders();
  m_AssetsFolderPath = m_Context.m_ProjectFolderPath + YG_PS + "Assets";

  Log(INFO, "Created Scene name {}", m_Context.m_name);
}

YgString Scene::GetConfigurationFilePath(YgString path)
{
  return path + YG_PS + m_Context.m_name + ".yml";
}

void Scene::RemoveDuplicatesEntities() {}

void Scene::VerifyAssetsSubFolders()
{
  YgString assetsFolder = m_Context.m_ProjectFolderPath + YG_PS + "Assets";
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

std::vector<std::pair<YgString, YgString>> Scene::VerifyImportedModelsOptionsInAssetsFolder()
{
  YgString assetsFolder = m_Context.m_ProjectFolderPath + YG_PS + "Assets";
  std::vector<std::pair<YgString, YgString>> models;
  if (!Yeager::ValidatesPath(assetsFolder + YG_PS + "ImportedModels", false)) {
    Yeager::Log(WARNING, "Assets sub folder ImportedModels doesnt not exist, options and help wont be avaliable!");
    return models;
  }

  for (const auto& item : std::filesystem::directory_iterator(assetsFolder + YG_PS + "ImportedModels")) {
    if (item.is_directory()) {
      for (const auto& folder : std::filesystem::directory_iterator(item)) {
        if (folder.path().extension() == ".obj") {
          std::pair<YgString, YgString> model;
          YgString path = folder.path().string();
          model.second = path;
          YgString name = ReadSuffixUntilCharacter(path, YG_PS);
          name = name.substr(1);
          model.first = name;
          models.push_back(model);
        }
      }
    } else {
      if (item.path().extension() == ".obj") {
        std::pair<YgString, YgString> model;
        YgString path = item.path().string();
        model.second = path;
        YgString name = ReadSuffixUntilCharacter(path, YG_PS);
        name = name.substr(1);
        model.first = name;
        models.push_back(model);
      }
    }
  }
  return models;
}

std::vector<std::pair<YgString, YgString>> Scene::VerifySoundsOptionsInAssetFolder()
{
  YgString assetsFolder = m_Context.m_ProjectFolderPath + YG_PS + "Assets";
  std::vector<std::pair<YgString, YgString>> audios;
  if (!Yeager::ValidatesPath(assetsFolder + YG_PS + "Sound", false)) {
    Yeager::Log(WARNING, "Assets sub folder Sound doesnt not exist, options and help wont be avaliable!");
    return audios;
  }

  for (const auto& file : std::filesystem::directory_iterator(assetsFolder + YG_PS + "Sound")) {
    YgString path = file.path().string();

    if (file.path().extension() == ".wav") {
      std::pair<YgString, YgString> audio_file;
      audio_file.second = path;
      YgString name = ReadSuffixUntilCharacter(path, YG_PS);
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
  std::vector<size_t> pos = CheckDuplicatesEntities<PhysicalLightHandle>(&m_LightSources);
  for (const auto& index : pos) {
    PhysicalLightHandle* light = m_LightSources.at(index).get();

    light->ScheduleDeletionOfPointLights();
    m_LightSources.erase(m_LightSources.begin() + index);
    for (auto& indexMinus : pos) {
      indexMinus--;
    }
  }
#ifdef YEAGER_DEBUG
  Yeager::Log(INFO, "Removed {} Duplicates Light Sources", pos.size());
#endif
}

void Scene::CheckScheduleDeletions()
{
  for (YEAGER_UINT x = 0; x < m_Objects.size(); x++) {
    Yeager::Object* obj = m_Objects.at(x).get();
    if (obj->GetScheduleDeletion()) {
      obj->GetToolbox()->SetScheduleDeletion(true);
      m_Objects.erase(m_Objects.begin() + x);
    }
  }

  CheckToolboxesScheduleDeletions();

  for (YEAGER_UINT x = 0; x < m_LightSources.size(); x++) {
    Yeager::PhysicalLightHandle* light = m_LightSources.at(x).get();
    if (light->GetScheduleDeletion()) {
      m_LightSources.erase(m_LightSources.begin() + x);
    }
  }
}

void Scene::CheckToolboxesScheduleDeletions()
{
  for (YEAGER_UINT x = 0; x < m_Toolboxes.size(); x++) {
    Yeager::ToolBoxObject* obj = m_Toolboxes.at(x).get();
    if (obj->GetScheduleDeletion()) {
      CheckToolboxIsSelectedAndDisable(obj);
      m_Toolboxes.erase(m_Toolboxes.begin() + x);
    }
  }
}

void Scene::CheckToolboxIsSelectedAndDisable(Yeager::ToolBoxObject* toolbox)
{
  if (m_Application->GetExplorer()->GetSelectedToolbox() == toolbox) {
    m_Application->GetExplorer()->ResetSelectedToolbox();
  }
}

void Scene::ValidatesCommonFolders()
{
  if (!Yeager::ValidatesPath(m_Context.m_ProjectFolderPath + YG_PS + "Assets")) {
    std::filesystem::create_directory(m_Context.m_ProjectFolderPath + YG_PS + "Assets");

    VerifyAssetsSubFolders();
  }
  if (!Yeager::ValidatesPath(m_Context.m_ProjectFolderPath + YG_PS + "Configuration")) {
    std::filesystem::create_directory(m_Context.m_ProjectFolderPath + YG_PS + "Configuration");
  }
  if (!Yeager::ValidatesPath(m_Context.m_ProjectFolderPath + YG_PS + "Packages")) {
    std::filesystem::create_directory(m_Context.m_ProjectFolderPath + YG_PS + "Packages");
  }
  if (!Yeager::ValidatesPath(m_Context.m_ProjectFolderPath + YG_PS + "Main")) {
    std::filesystem::create_directory(m_Context.m_ProjectFolderPath + YG_PS + "Main");
  }
}

Scene::~Scene()
{
  Yeager::Log(INFO, "Destroring Scene name {}", m_Context.m_name);
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
  for (unsigned int x = 0; x < m_ThreadImporters.size(); x++) {
    std::pair<ImporterThreaded*, Yeager::Object*>* obj = &m_ThreadImporters.at(x);
    if (obj->first->IsThreadFinish()) {
      m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
    } else {
      obj->first->GetThreadPtr()->join();
      m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
    }
  }

  for (unsigned int x = 0; x < m_ThreadAnimatedImporters.size(); x++) {
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
    for (unsigned int x = 0; x < m_ThreadImporters.size(); x++) {
      std::pair<ImporterThreaded*, Yeager::Object*>* obj = &m_ThreadImporters.at(x);
      if (obj->first->IsThreadFinish()) {
        Yeager::Log(INFO, "Object importer thread {} has finished! Detaching", obj->second->GetName());
        obj->second->ThreadSetup();
        m_ThreadImporters.erase(m_ThreadImporters.begin() + x);
      }
    }

    for (unsigned int x = 0; x < m_ThreadAnimatedImporters.size(); x++) {
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
  m_Application->GetSerial()->SerializeScene(this, m_Context.m_ProjectSavePath);
}

void Scene::LoadEditorColorscheme(Interface* intr)
{
  intr->ApplyColorscheme(m_Application->GetSerial()->ReadColorschemeConfig());
}

void Scene::Load(YgString path)
{
  m_Application->GetSerial()->DeserializeScene(this, path);
}

void Scene::LoadSceneSave()
{
  m_Application->GetSerial()->DeserializeScene(this, m_Context.m_ProjectSavePath);
}

void Scene::SetContextType(SceneType type)
{
  m_Context.m_ExplorerType = type;
}
void Scene::SetContextRenderer(SceneRenderer renderer)
{
  m_Context.m_renderer = renderer;
}

std::vector<std::shared_ptr<Yeager::Audio3DHandle>>* Scene::GetAudios3D()
{
  return &m_Audios3D;
}
std::vector<std::shared_ptr<Yeager::AudioHandle>>* Scene::GetAudios()
{
  return &m_Audios;
}

std::vector<std::shared_ptr<ToolBoxObject>>* Scene::GetToolboxs()
{
  return &m_Toolboxes;
}

std::vector<std::shared_ptr<Yeager::Object>>* Scene::GetObjects()
{
  return &m_Objects;
}

std::vector<std::shared_ptr<Yeager::InstancedObject>>* Scene::GetInstancedObjects()
{
  return &m_InstancedObjects;
}
std::vector<std::shared_ptr<Yeager::AnimatedObject>>* Scene::GetAnimatedObject()
{
  return &m_AnimatedObject;
}

std::vector<std::shared_ptr<Yeager::InstancedAnimatedObject>>* Scene::GetInstancedAnimatedObjects()
{
  return &m_InstancedAnimatedObjects;
}

std::vector<std::shared_ptr<Yeager::PhysicalLightHandle>>* Scene::GetLightSources()
{
  return &m_LightSources;
}
