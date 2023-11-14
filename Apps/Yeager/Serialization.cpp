#include "Serialization.h"
#include "Application.h"
#include "Scene.h"
using namespace Yeager;

YAML::Emitter& operator<<(YAML::Emitter& out, const YgVector3& vector)
{
  out << YAML::Flow;
  out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
  return out;
}

template <typename _T>
void inline Serialization::SerializeObject(YAML::Emitter& out, const char* key, _T obj)
{
  out << YAML::Key << key << YAML::Value << obj;
}

void inline Serialization::SerializeBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip)
{
  out << YAML::Key << key << YAML::Value << manip;
}

inline void Yeager::Serialization::SerializeSystemInfo(YAML::Emitter& out, Yeager::Scene* scene)
{
#ifdef YEAGER_SYSTEM_WINDOWS_x64
  out << YAML::Key << "Operating System" << YAML::Value << "Windows_x64";
#elif defined(YEAGER_SYSTEM_LINUX)
  out << YAML::Key << "Operating System" << YAML::Value << "Linux";
#endif
}

Serialization::Serialization(Yeager::ApplicationCore* app) : m_app(app) {}

ImVec4 NodeVec4ToImVec4(const YAML::Node& node, const char* item)
{
  ImVec4 vector;
  vector.x = node[item]["x"].as<float>();
  vector.y = node[item]["y"].as<float>();
  vector.z = node[item]["z"].as<float>();
  vector.w = node[item]["w"].as<float>();
  return vector;
}

void inline Serialization::SerializeBasicEntity(YAML::Emitter& out, YgString name, unsigned int id, YgString type)
{
  out << YAML::Key << "Entity" << YAML::Value << id;
  out << YAML::Key << "Name" << YAML::Value << name;
  out << YAML::Key << "Type" << YAML::Value << type;
}

void Serialization::SerializeScene(Yeager::Scene* scene, YgString path)
{
  YAML::Emitter out;

  out << YAML::BeginMap;
  SerializeSystemInfo(out, scene);
  SerializeObject(out, "Scene", scene->GetContext().m_name);
  SerializeObject(out, "Renderer", SceneRendererToString(scene->GetContext().m_renderer));
  SerializeObject(out, "SceneType", SceneTypeToString(scene->GetContext().m_type));
  SerializeBegin(out, "SceneEntities", YAML::BeginSeq);

  for (unsigned int x = 0; x < scene->GetAudios()->size(); x++) {
    AudioHandle* audio = scene->GetAudios()->at(x).get();
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetId(), "AudioHandle");
    SerializeObject(out, "Path", audio->GetPath());
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetAudios3D()->size(); x++) {
    Audio3DHandle* audio = scene->GetAudios3D()->at(x).get();
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetId(), "Audio3DHandle");
    SerializeObject(out, "Path", audio->GetPath());
    SerializeObject(out, "Position", audio->GetVector3Position());
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetImportedObjects()->size(); x++) {
    ImportedObject* obj = scene->GetImportedObjects()->at(x).get();
    out << YAML::BeginMap;
    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "ImportedObject");
    SerializeObject(out, "Path", obj->GetPath());
    SerializeObject(out, "Position", obj->GetTransformation().position);
    SerializeObject(out, "Rotation", obj->GetTransformation().rotation);
    SerializeObject(out, "Scale", obj->GetTransformation().scale);
    SerializeObject(out, "TexturesLoaded", obj->GetTexturesLoaded()->size());
    SerializeBegin(out, "Textures", YAML::BeginMap);
    for (unsigned int y = 0; y < obj->GetTexturesLoaded()->size(); y++) {
      SerializeObject(out, "Name", obj->GetTexturesLoaded()->at(y).m_name);
      SerializeObject(out, "Path", obj->GetTexturesLoaded()->at(y).m_path);
      SerializeObject(out, "ID", obj->GetTexturesLoaded()->at(y).m_id);
    }
    out << YAML::EndMap;
    out << YAML::EndMap;
  }
  for (unsigned int x = 0; x < scene->GetGeometry()->size(); x++) {
    Yeager::Geometry* geometry = scene->GetGeometry()->at(x).get();
    out << YAML::BeginMap;
    SerializeBasicEntity(out, geometry->GetName(), geometry->GetId(), "Geometry");
    SerializeObject(out, "Shape", geometry->GetShape());
    SerializeObject(out, "Position", geometry->GetTransformation().position);
    SerializeObject(out, "Rotation", geometry->GetTransformation().rotation);
    SerializeObject(out, "Scale", geometry->GetTransformation().scale);
    SerializeObject(out, "IsColor", geometry->isColor());
    if (geometry->isColor()) {
      SerializeObject(out, "Color", geometry->GetColor());
    } else {
      Yeager::Texture2D* tex = geometry->GetTexture();
      SerializeBegin(out, "Texture", YAML::BeginMap);
      SerializeObject(out, "Path", tex->GetPath());
      SerializeObject(out, "ID", tex->GetID());
      SerializeObject(out, "Type", tex->GetTypeName());
      out << YAML::EndMap;
    }
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;
  std::ofstream fout(path.c_str());

  if (fout.is_open()) {
    fout << out.c_str();
  } else {
    Yeager::Log(ERROR, "Cannot write to configuration file! {}", path.c_str());
  }

  fout.close();
}

template <typename _Ty>
const void inline Serialization::CheckAndDeserialize(YAML::Node& node, _Ty* obj, YgCchar key) noexcept
{
  if (node[key]) {
    const auto result = node[key].as<_Ty>();
    *obj = result;
  } else {
    Yeager::Log(ERROR, "Cannot deserialize: {}", key);
  }
}

ColorschemeConfig Serialization::ReadColorschemeConfig()
{
  ColorschemeConfig colorscheme;
  YAML::Node node = YAML::LoadFile(GetPath("/Configuration/Editor/Colorschemes/dark_mode.yaml"));

  CheckAndDeserialize(node, &colorscheme.WindowRounding, "WindowRounding");
  CheckAndDeserialize(node, &colorscheme.FrameRounding, "FrameRounding");

  if (node["ScrollbarRounding"]) {
    colorscheme.ScrollbarRounding = node["ScrollbarRounding"].as<float>();
  }
  if (node["Colors_ImGuiCol_Text"]) {
    ImVec4 vector = node["Colors_ImGuiCol_Text"].as<ImVec4>();
    colorscheme.Colors_ImGuiCol_Text = vector;
  }
  if (node["Colors_ImGuiCol_TextDisabled"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_TextDisabled");
    colorscheme.Colors_ImGuiCol_TextDisabled = vector;
  }
  if (node["Colors_ImGuiCol_WindowBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_WindowBg");
    colorscheme.Colors_ImGuiCol_WindowBg = vector;
  }
  if (node["Colors_ImGuiCol_ChildBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ChildBg");
    colorscheme.Colors_ImGuiCol_ChildBg = vector;
  }
  if (node["Colors_ImGuiCol_PopupBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_PopupBg");
    colorscheme.Colors_ImGuiCol_PopupBg = vector;
  }
  if (node["Colors_ImGuiCol_Border"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_Border");
    colorscheme.Colors_ImGuiCol_Border = vector;
  }
  if (node["Colors_ImGuiCol_BorderShadow"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_BorderShadow");
    colorscheme.Colors_ImGuiCol_BorderShadow = vector;
  }
  if (node["Colors_ImGuiCol_FrameBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_FrameBg");
    colorscheme.Colors_ImGuiCol_FrameBg = vector;
  }
  if (node["Colors_ImGuiCol_FrameBgHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_FrameBgHovered");
    colorscheme.Colors_ImGuiCol_FrameBgHovered = vector;
  }
  if (node["Colors_ImGuiCol_FrameBgActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_FrameBgActive");
    colorscheme.Colors_ImGuiCol_FrameBgActive = vector;
  }
  if (node["Colors_ImGuiCol_TitleBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_TitleBg");
    colorscheme.Colors_ImGuiCol_TitleBg = vector;
  }
  if (node["Colors_ImGuiCol_TitleBgCollapsed"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_TitleBgCollapsed");
    colorscheme.Colors_ImGuiCol_TitleBgCollapsed = vector;
  }
  if (node["Colors_ImGuiCol_TitleBgActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_TitleBgActive");
    colorscheme.Colors_ImGuiCol_TitleBgActive = vector;
  }
  if (node["Colors_ImGuiCol_MenuBarBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_MenuBarBg");
    colorscheme.Colors_ImGuiCol_MenuBarBg = vector;
  }
  if (node["Colors_ImGuiCol_ScrollbarBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ScrollbarBg");
    colorscheme.Colors_ImGuiCol_ScrollbarBg = vector;
  }
  if (node["Colors_ImGuiCol_ScrollbarGrab"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ScrollbarGrab");
    colorscheme.Colors_ImGuiCol_ScrollbarGrab = vector;
  }
  if (node["Colors_ImGuiCol_ScrollbarGrabHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ScrollbarGrabHovered");
    colorscheme.Colors_ImGuiCol_ScrollbarGrabHovered = vector;
  }
  if (node["Colors_ImGuiCol_ScrollbarGrabActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ScrollbarGrabActive");
    colorscheme.Colors_ImGuiCol_ScrollbarGrabActive = vector;
  }
  if (node["Colors_ImGuiCol_CheckMark"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_CheckMark");
    colorscheme.Colors_ImGuiCol_CheckMark = vector;
  }
  if (node["Colors_ImGuiCol_SliderGrab"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_SliderGrab");
    colorscheme.Colors_ImGuiCol_SliderGrab = vector;
  }
  if (node["Colors_ImGuiCol_SliderGrabActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_SliderGrabActive");
    colorscheme.Colors_ImGuiCol_SliderGrabActive = vector;
  }
  if (node["Colors_ImGuiCol_Button"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_Button");
    colorscheme.Colors_ImGuiCol_Button = vector;
  }
  if (node["Colors_ImGuiCol_ButtonHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ButtonHovered");
    colorscheme.Colors_ImGuiCol_ButtonHovered = vector;
  }
  if (node["Colors_ImGuiCol_ButtonActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ButtonActive");
    colorscheme.Colors_ImGuiCol_ButtonActive = vector;
  }
  if (node["Colors_ImGuiCol_Header"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_Header");
    colorscheme.Colors_ImGuiCol_Header = vector;
  }
  if (node["Colors_ImGuiCol_HeaderHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_HeaderHovered");
    colorscheme.Colors_ImGuiCol_HeaderHovered = vector;
  }
  if (node["Colors_ImGuiCol_HeaderActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_HeaderActive");
    colorscheme.Colors_ImGuiCol_HeaderActive = vector;
  }
  if (node["Colors_ImGuiCol_ResizeGrip"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ResizeGrip");
    colorscheme.Colors_ImGuiCol_ResizeGrip = vector;
  }
  if (node["Colors_ImGuiCol_ResizeGripHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ResizeGripHovered");
    colorscheme.Colors_ImGuiCol_ResizeGripHovered = vector;
  }
  if (node["Colors_ImGuiCol_ResizeGripActive"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_ResizeGripActive");
    colorscheme.Colors_ImGuiCol_ResizeGripActive = vector;
  }
  if (node["Colors_ImGuiCol_PlotLines"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_PlotLines");
    colorscheme.Colors_ImGuiCol_PlotLines = vector;
  }
  if (node["Colors_ImGuiCol_PlotLinesHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_PlotLinesHovered");
    colorscheme.Colors_ImGuiCol_PlotLinesHovered = vector;
  }
  if (node["Colors_ImGuiCol_PlotHistogram"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_PlotHistogram");
    colorscheme.Colors_ImGuiCol_PlotHistogram = vector;
  }
  if (node["Colors_ImGuiCol_PlotHistogramHovered"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_PlotHistogramHovered");
    colorscheme.Colors_ImGuiCol_PlotHistogramHovered = vector;
  }
  if (node["Colors_ImGuiCol_TextSelectedBg"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_TextSelectedBg");
    colorscheme.Colors_ImGuiCol_TextSelectedBg = vector;
  }
  return colorscheme;
}
void Serialization::ReadSceneShadersConfig(YgString path)
{
  YAML::Node node = YAML::LoadFile(path);

  if (node["SceneShader"]) {
    Yeager::Log(INFO, "Reading shader configuration {}", node["SceneShader"].as<YgString>());
  }
  auto shaders = node["Shaders"];
  if (shaders) {
    for (auto shader : shaders) {
      try {
        YgString name = shader["Shader"].as<YgString>();
        YgString fragment = shader["FragmentPath"].as<YgString>();
        YgString vertex = shader["VertexPath"].as<YgString>();
        YgString var = shader["VarName"].as<YgString>();
        auto ps_shader = std::make_shared<Yeager::Shader>(GetPath(fragment).c_str(), GetPath(vertex).c_str(), name);
        ShaderFromYaml yaml_shader;
        yaml_shader.m_shader = ps_shader;
        yaml_shader.m_varName = var;
        ygConfigShaders.push_back(yaml_shader);
      } catch (YAML::BadConversion exc) {
        Yeager::Log(ERROR, "Exception thrown at reading shaders from config {}", exc.msg);
      }
    }
  }
}

void Serialization::DeserializeScene(Yeager::Scene* scene, YgString path)
{
  YAML::Node node = YAML::LoadFile(path);

  DeserializeSceneInfo(scene, node);

  auto entities = node["SceneEntities"];
  for (auto entity : entities) {
    try {
      DeserializeEntity(scene, node, entity);
    } catch (YAML::BadConversion exc) {
      Yeager::Log(ERROR,
                  "Something went wrong at reading the Scene Entities configuration! Have the user change itself?");
    }
  }
}

void inline Serialization::DeserializeSceneInfo(Yeager::Scene* scene, YAML::Node& node)
{
  if (node) {
    if (node["Scene"]) {
      scene->GetContext().m_name = node["Scene"].as<YgString>();
    }
    if (node["Renderer"]) {
      scene->SetContextRenderer(StringToSceneRenderer(node["Renderer"].as<YgString>()));
    }
    if (node["SceneType"]) {
      scene->SetContextType(StringToScreneType(node["SceneType"].as<YgString>()));
    }
  }
}

void inline Serialization::DeserializeEntity(Yeager::Scene* scene, YAML::Node& node,
                                             YAML::detail::iterator_value& entity)
{
  /// All entities have theses values, name, id and type, if for some reason, one of them is missing, them the user have change something in the configuration file itself
  /// or someone implemented a new type of entity without the proper configuration in the serialization class for saving it correctly
  unsigned int id = entity["Entity"].as<unsigned int>();
  YgString name = entity["Name"].as<YgString>();
  YgString type = entity["Type"].as<YgString>();

  if (type == "AudioHandle" || type == "Audio3DHandle") {

    /// Both 3d audio and 2d audio have a pat, we group them here because in the future, both will have more common variables
    YgString path = entity["Path"].as<YgString>();

    /// 3d Audio processing goes here
    if (type == "Audio3DHandle") {
      YgVector3 position = entity["Position"].as<YgVector3>();
      auto audio = std::make_shared<Yeager::Audio3DHandle>(path, name, false, YgVec3_to_Vec3df(position));
      scene->GetAudios3D()->push_back(audio);
      auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
      toolbox->Set3DAudio(audio.get());
      toolbox->SetType(ExplorerObjectType::k3DAudio);
      scene->GetToolboxs()->push_back(toolbox);

      /// Normal Audio goes here
    } else {
      auto audio = std::make_shared<Yeager::AudioHandle>(path, name, false);
      scene->GetAudios()->push_back(audio);
      auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
      toolbox->SetAudio(audio.get());
      toolbox->SetType(ExplorerObjectType::kAudio);
      scene->GetToolboxs()->push_back(toolbox);
    }

  } else if (type == "ImportedObject") {

    YgString path = entity["Path"].as<YgString>();
    YgVector3 position = entity["Position"].as<YgVector3>();
    YgVector3 rotation = entity["Rotation"].as<YgVector3>();
    YgVector3 scale = entity["Scale"].as<YgVector3>();
    auto obj = std::make_shared<ImportedObject>(path, m_app, name, true);
    obj->GetTransformationPtr()->position = position;
    obj->GetTransformationPtr()->rotation = rotation;
    obj->GetTransformationPtr()->scale = scale;
    scene->GetImportedObjects()->push_back(obj);
  }
}