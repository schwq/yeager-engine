#include "Serialization.h"
#include "Application.h"
#include "Scene.h"
using namespace Yeager;

YAML::Emitter& Yeager::operator<<(YAML::Emitter& out, const yg_vec3& vector)
{
  out << YAML::Flow;
  out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
  return out;
}

template <typename _T>
void inline Serialization::SerialObj(YAML::Emitter& out, const char* key, _T obj)
{
  out << YAML::Key << key << YAML::Value << obj;
}

void inline Serialization::SerialBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip)
{
  out << YAML::Key << key << YAML::Value << manip;
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

void inline Serialization::SerialBasicEntity(YAML::Emitter& out, yg_string name, yg_uint id, yg_string type)
{
  out << YAML::Key << "Entity" << YAML::Value << id;
  out << YAML::Key << "Name" << YAML::Value << name;
  out << YAML::Key << "Type" << YAML::Value << type;
}

void Serialization::SerializeScene(Yeager::Scene* scene, yg_string path)
{
  YAML::Emitter out;

  out << YAML::BeginMap;
  SerialObj(out, "Scene", scene->GetContext().m_name);
  SerialObj(out, "Renderer", SceneRendererToString(scene->GetContext().m_renderer));
  SerialObj(out, "SceneType", SceneTypeToString(scene->GetContext().m_type));
  SerialBegin(out, "SceneEntities", YAML::BeginSeq);

  for (yg_uint x = 0; x < scene->GetAudios()->size(); x++) {
    AudioHandle* audio = scene->GetAudios()->at(x).get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, audio->GetName(), audio->GetId(), "AudioHandle");
    SerialObj(out, "Path", audio->GetPath());
    out << YAML::EndMap;
  }

  for (yg_uint x = 0; x < scene->GetAudios3D()->size(); x++) {
    Audio3DHandle* audio = scene->GetAudios3D()->at(x).get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, audio->GetName(), audio->GetId(), "Audio3DHandle");
    SerialObj(out, "Path", audio->GetPath());
    SerialObj(out, "Position", audio->GetVector3Position());
    out << YAML::EndMap;
  }

  for (yg_uint x = 0; x < scene->GetImportedObjects()->size(); x++) {
    ImportedObject* obj = scene->GetImportedObjects()->at(x).get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, obj->GetName(), obj->GetId(), "ImportedObject");
    SerialObj(out, "Path", obj->GetPath());
    SerialObj(out, "Position", obj->GetTransformation().position);
    SerialObj(out, "Rotation", obj->GetTransformation().rotation);
    SerialObj(out, "Scale", obj->GetTransformation().scale);
    SerialObj(out, "TexturesLoaded", obj->GetTexturesLoaded()->size());
    SerialBegin(out, "Textures", YAML::BeginMap);
    for (yg_uint y = 0; y < obj->GetTexturesLoaded()->size(); y++) {
      SerialObj(out, "Name", obj->GetTexturesLoaded()->at(y).m_name);
      SerialObj(out, "Path", obj->GetTexturesLoaded()->at(y).m_path);
      SerialObj(out, "ID", obj->GetTexturesLoaded()->at(y).m_id);
    }
    out << YAML::EndMap;
    out << YAML::EndMap;
  }
  for (yg_uint x = 0; x < scene->GetGeometry()->size(); x++) {
    Yeager::Geometry* geometry = scene->GetGeometry()->at(x).get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, geometry->GetName(), geometry->GetId(), "Geometry");
    SerialObj(out, "Shape", geometry->GetShape());
    SerialObj(out, "Position", geometry->GetTransformation().position);
    SerialObj(out, "Rotation", geometry->GetTransformation().rotation);
    SerialObj(out, "Scale", geometry->GetTransformation().scale);
    SerialObj(out, "IsColor", geometry->isColor());
    if (geometry->isColor()) {
      SerialObj(out, "Color", geometry->GetColor());
    } else {
      Yeager::Texture2D* tex = geometry->GetTexture();
      SerialBegin(out, "Texture", YAML::BeginMap);
      SerialObj(out, "Path", tex->GetPath());
      SerialObj(out, "ID", tex->GetID());
      SerialObj(out, "Type", tex->GetTypeName());
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

ColorschemeConfig Serialization::ReadColorschemeConfig()
{
  ColorschemeConfig colorscheme;
  YAML::Node node = YAML::LoadFile(GetPath("/Configuration/Editor/Colorschemes/dark_mode.yaml"));

  if (node["WindowRounding"]) {
    colorscheme.WindowRounding = node["WindowRounding"].as<float>();
  }
  if (node["FrameRounding"]) {
    colorscheme.FrameRounding = node["FrameRounding"].as<float>();
  }
  if (node["ScrollbarRounding"]) {
    colorscheme.ScrollbarRounding = node["ScrollbarRounding"].as<float>();
  }
  if (node["Colors_ImGuiCol_Text"]) {
    ImVec4 vector = NodeVec4ToImVec4(node, "Colors_ImGuiCol_Text");
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
void Serialization::ReadSceneShadersConfig(yg_string path)
{
  YAML::Node node = YAML::LoadFile(path);

  if (node["SceneShader"]) {
    Yeager::Log(INFO, "Reading shader configuration {}", node["SceneShader"].as<yg_string>());
  }
  auto shaders = node["Shaders"];
  if (shaders) {
    for (auto shader : shaders) {
      try {
        yg_string name = shader["Shader"].as<yg_string>();
        yg_string fragment = shader["FragmentPath"].as<yg_string>();
        yg_string vertex = shader["VertexPath"].as<yg_string>();
        yg_string var = shader["VarName"].as<yg_string>();
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

void Serialization::DeserializeScene(Yeager::Scene* scene, yg_string path)
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
      scene->GetContext().m_name = node["Scene"].as<yg_string>();
    }
    if (node["Renderer"]) {
      scene->SetContextRenderer(StringToSceneRenderer(node["Renderer"].as<yg_string>()));
    }
    if (node["SceneType"]) {
      scene->SetContextType(StringToScreneType(node["SceneType"].as<yg_string>()));
    }
  }
}

void inline Serialization::DeserializeEntity(Yeager::Scene* scene, YAML::Node& node,
                                             YAML::detail::iterator_value& entity)
{
  /// All entities have theses values, name, id and type, if for some reason, one of them is missing, them the user have change something in the configuration file itself
  /// or someone implemented a new type of entity without the proper configuration in the serialization class for saving it correctly
  yg_uint id = entity["Entity"].as<yg_uint>();
  yg_string name = entity["Name"].as<yg_string>();
  yg_string type = entity["Type"].as<yg_string>();

  if (type == "AudioHandle" || type == "Audio3DHandle") {

    /// Both 3d audio and 2d audio have a pat, we group them here because in the future, both will have more common variables
    yg_string path = entity["Path"].as<yg_string>();

    /// 3d Audio processing goes here
    if (type == "Audio3DHandle") {
      yg_vec3 position = entity["Position"].as<yg_vec3>();
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

    yg_string path = entity["Path"].as<yg_string>();
    yg_vec3 position = entity["Position"].as<yg_vec3>();
    yg_vec3 rotation = entity["Rotation"].as<yg_vec3>();
    yg_vec3 scale = entity["Scale"].as<yg_vec3>();
    auto obj = std::make_shared<ImportedObject>(path, m_app, name, true);
    obj->GetTransformationPtr()->position = position;
    obj->GetTransformationPtr()->rotation = rotation;
    obj->GetTransformationPtr()->scale = scale;
    scene->GetImportedObjects()->push_back(obj);
  }
}