#include "Scene.h"
using namespace Yeager;

Scene::Scene(yg_string name, SceneType type, SceneRenderer renderer)
{
  m_context.m_name = name;
  m_context.m_type = type;
  m_context.m_renderer = renderer;
  m_context.m_file_path = GetSceneFilePath();

  Log(INFO, kSystem, "Created Scene name {}", m_context.m_name);
}

Scene::~Scene()
{
  Log(INFO, kSystem, "Destroring Scene name {}", m_context.m_name);
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

void Scene::Load(yg_string path) {}