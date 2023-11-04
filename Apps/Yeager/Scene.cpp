#include "Scene.h"
#include "Application.h"
using namespace Yeager;

SceneType Yeager::StringToScreneType(yg_string str)
{
  if (str == "Scene2D") {
    return Scene2D;
  } else if (str == "Scene3D") {
    return Scene3D;
  } else {
    return SceneError;
  }
}

SceneRenderer Yeager::StringToSceneRenderer(yg_string str)
{
  if (str == "OpenGL3_3") {
    return OpenGL3_3;
  } else if (str == "OpenGL4") {
    return OpenGL4;
  } else {
    return RendererError;
  }
}

yg_string Yeager::SceneTypeToString(SceneType type)
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
yg_string Yeager::SceneRendererToString(SceneRenderer renderer)
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

Scene::Scene(yg_string name, SceneType type, SceneRenderer renderer, Yeager::ApplicationCore* app) : m_app(app)
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

inline yg_string Scene::GetSceneFilePath()
{
  yg_string conf = GetPath("/Configuration/Scenes/");
  yg_string path = conf + m_context.m_name + ".yaml";
  return path;
}

void Scene::Save()
{
  m_serial.SerializeScene(this, m_context.m_file_path);
}

void Scene::LoadEditorColorscheme(Interface* interface)
{
  interface->ApplyColorscheme(m_serial.ReadColorschemeConfig());
}

void Scene::Load(yg_string path)
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
std::vector<std::shared_ptr<Yeager::Geometry>>* Scene::GetGeometry()
{
  return &m_geometry;
}
std::vector<std::shared_ptr<ImportedObject>>* Scene::GetImportedObjects()
{
  return &m_imported_objs;
}

std::vector<std::shared_ptr<ToolBoxObject>>* Scene::GetToolboxs()
{
  return &m_toolboxs;
}
