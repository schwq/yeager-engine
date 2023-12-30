#include "Scene.h"
#include "Application.h"
#include "Engine/Renderer/Skybox.h"
using namespace Yeager;

SceneType Yeager::StringToScreneType(YgString str)
{
  switch (Yeager::StringToInteger(str.c_str())) {
    case Yeager::StringToInteger("Scene2D"):
      return Scene2D;
    case Yeager::StringToInteger("Scene3D"):
      return Scene3D;
    default:
      Yeager::Log(ERROR, "Cannot convert string to scene type!");
      return SceneError;
  }
}

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

Scene::Scene(YgString name, YgString Author, SceneType type, YgString folder_path, SceneRenderer renderer,
             Yeager::ApplicationCore* app)
    : m_Application(app)
{
  m_Context.m_name = name;
  m_Context.m_type = type;
  m_Context.m_ProjectAuthor = Author;
  m_Context.m_renderer = renderer;
  m_Context.m_ProjectFolderPath = folder_path;
  m_Context.m_ProjectSavePath = GetConfigurationFilePath(m_Context.m_ProjectFolderPath);
  m_Serial = Yeager::Serialization(m_Application);
  m_CurrentSkybox = new Skybox("Default Skybox", ObjectGeometryType::ESphere, m_Application, false);
  ValidatesCommonFolders();

  Log(INFO, "Created Scene name {}", m_Context.m_name);
}

YgString Scene::GetConfigurationFilePath(YgString path)
{
#ifdef YEAGER_SYSTEM_LINUX
  return path + "/" + m_Context.m_name + ".yml";
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  return path + "\\" + m_Context.m_name + ".yml";
#endif
}

void Scene::ValidatesCommonFolders()
{
  if (!Yeager::ValidatesPath(m_Context.m_ProjectFolderPath + YG_PS + "Assets")) {
    std::filesystem::create_directory(m_Context.m_ProjectFolderPath + YG_PS + "Assets");
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
  delete m_CurrentSkybox;
  Log(INFO, "Destroring Scene name {}", m_Context.m_name);
}

YgString Scene::GetPathRelative(YgString path)
{
#ifdef YEAGER_SYSTEM_LINUX
  YgString str = m_Context.m_ProjectFolderPath + path;
  Yeager::ValidatesPath(str);
  return str;
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  std::replace(path.begin(), path.end(), '/', '\\');
  YgString str = m_Context.m_ProjectFolderPath + path;
  Yeager::ValidatesPath(str);
  return str;
#else
  Yeager::Log(
      "This engine build does not support system other that Windows_x64 and Linux, Scene cannot return a decent path "
      "relative to the project folder!");
  return YEAGER_NULL_LITERAL;
#endif
}

void Scene::Save()
{
  m_Serial.SerializeScene(this, m_Context.m_ProjectSavePath);
}

void Scene::LoadEditorColorscheme(Interface* intr)
{
  intr->ApplyColorscheme(m_Serial.ReadColorschemeConfig());
}

void Scene::Load(YgString path)
{
  m_Serial.DeserializeScene(this, path);
}

void Scene::LoadSceneSave()
{
  m_Serial.DeserializeScene(this, m_Context.m_ProjectSavePath);
}

void Scene::SetContextType(SceneType type)
{
  m_Context.m_type = type;
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

std::vector<std::shared_ptr<Yeager::LightSource>>* Scene::GetLightSources()
{
  return &m_LightSources;
}
