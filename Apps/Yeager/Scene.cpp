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

Scene::Scene(YgString name, SceneType type, SceneRenderer renderer, Yeager::ApplicationCore* app) : m_app(app)
{
  m_context.m_name = name;
  m_context.m_type = type;
  m_context.m_renderer = renderer;
  m_context.m_file_path = GetSceneFilePath();
  m_serial = Yeager::Serialization(m_app);

  Log(INFO, "Created Scene name {}", m_context.m_name);
}

Scene::~Scene()
{
  Log(INFO, "Destroring Scene name {}", m_context.m_name);
}

inline YgString Scene::GetSceneFilePath()
{
  YgString conf = GetPath("/Configuration/Scenes/");
  YgString path = conf + m_context.m_name + ".yaml";
  return path;
}

void Scene::Save()
{
  m_serial.SerializeScene(this, m_context.m_file_path);
}

void Scene::LoadEditorColorscheme(Interface* intr)
{
  intr->ApplyColorscheme(m_serial.ReadColorschemeConfig());
}

void Scene::Load(YgString path)
{
  m_serial.DeserializeScene(this, path);
}

void Scene::SetContextType(SceneType type)
{
  m_context.m_type = type;
}
void Scene::SetContextRenderer(SceneRenderer renderer)
{
  m_context.m_renderer = renderer;
}

std::vector<std::shared_ptr<Yeager::Audio3DHandle>>* Scene::GetAudios3D()
{
  return &m_audios3d;
}
std::vector<std::shared_ptr<Yeager::AudioHandle>>* Scene::GetAudios()
{
  return &m_audios;
}

std::vector<std::shared_ptr<ToolBoxObject>>* Scene::GetToolboxs()
{
  return &m_toolboxs;
}

std::vector<std::shared_ptr<Yeager::Object>>* Scene::GetObjects()
{
  return &m_objs;
}