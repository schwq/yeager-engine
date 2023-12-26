#include "Scene.h"
#include "Application.h"
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

Scene::Scene(YgString name, SceneType type, SceneRenderer renderer, Yeager::ApplicationCore* app) : m_Application(app)
{
  m_Context.m_name = name;
  m_Context.m_type = type;
  m_Context.m_renderer = renderer;
  m_Context.m_file_path = GetSceneFilePath();
  m_Serial = Yeager::Serialization(m_Application);

  Log(INFO, "Created Scene name {}", m_Context.m_name);
}

Scene::~Scene()
{
  Log(INFO, "Destroring Scene name {}", m_Context.m_name);
}

inline YgString Scene::GetSceneFilePath()
{
  YgString conf = GetPath("/Configuration/Scenes/");
  YgString path = conf + m_Context.m_name + ".yaml";
  return path;
}

void Scene::Save()
{
  m_Serial.SerializeScene(this, m_Context.m_file_path);
}

void Scene::LoadEditorColorscheme(Interface* intr)
{
  intr->ApplyColorscheme(m_Serial.ReadColorschemeConfig());
}

void Scene::Load(YgString path)
{
  m_Serial.DeserializeScene(this, path);
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
