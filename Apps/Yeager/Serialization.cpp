#include "Serialization.h"
#include "Scene.h"
using namespace Yeager;

YAML::Emitter& Yeager::operator<<(YAML::Emitter& out, const yg_vec3& vector)
{
  out << YAML::Flow;
  out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
  return out;
}

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
  out << YAML::Key << "Scene" << YAML::Value << scene->GetContext().m_name;
  out << YAML::Key << "Renderer" << YAML::Value << scene->GetContext().m_renderer;
  out << YAML::Key << "SceneType" << YAML::Value << scene->GetContext().m_type;
  out << YAML::Key << "SceneEntities" << YAML::Value << YAML::BeginSeq;
  for (yg_uint x = 0; x < ygAudioHandles.size(); x++) {
    AudioHandle* audio = ygAudioHandles[x].get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, audio->GetName(), audio->GetId(), "AudioHandle");
    out << YAML::Key << "Path" << YAML::Value << audio->GetPath();
    out << YAML::EndMap;
  }
  for (yg_uint x = 0; x < ygAudio3DHandles.size(); x++) {
    Audio3DHandle* audio = ygAudio3DHandles[x].get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, audio->GetName(), audio->GetId(), "Audio3DHandle");
    out << YAML::Key << "Path" << YAML::Value << audio->GetPath();
    out << YAML::Key << "Position" << YAML::Value << audio->GetVector3Position();
    out << YAML::EndMap;
  }
  for (yg_uint x = 0; x < ygImportedObjects.size(); x++) {
    ImportedObject* obj = ygImportedObjects[x].get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, obj->GetName(), obj->GetId(), "ImportedObject");
    out << YAML::Key << "Path" << YAML::Value << obj->GetPath();
    out << YAML::Key << "Position" << YAML::Value << obj->GetTransformation().position;
    out << YAML::Key << "Rotation" << YAML::Value << obj->GetTransformation().rotation;
    out << YAML::Key << "Scale" << YAML::Value << obj->GetTransformation().scale;
    out << YAML::Key << "TexturesLoaded" << YAML::Value << obj->GetTexturesLoaded()->size();
    out << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
    for (yg_uint y = 0; y < obj->GetTexturesLoaded()->size(); y++) {
      out << YAML::Key << "Name" << YAML::Value << obj->GetTexturesLoaded()->at(y).m_name;
      out << YAML::Key << "Path" << YAML::Value << obj->GetTexturesLoaded()->at(y).m_path;
      out << YAML::Key << "ID" << YAML::Value << obj->GetTexturesLoaded()->at(y).m_id;
    }
    out << YAML::EndMap;
    out << YAML::EndMap;
  }
  for (yg_uint x = 0; x < yg_Shapes.size(); x++) {
    Yeager::Geometry* geometry = yg_Shapes[x].get();
    out << YAML::BeginMap;
    SerialBasicEntity(out, geometry->GetName(), geometry->GetId(), "Geometry");
    out << YAML::Key << "Shape" << YAML::Value << geometry->GetShape();
    out << YAML::Key << "Position" << YAML::Value << geometry->GetTransformation().position;
    out << YAML::Key << "Rotation" << YAML::Value << geometry->GetTransformation().rotation;
    out << YAML::Key << "Scale" << YAML::Value << geometry->GetTransformation().scale;
    out << YAML::Key << "IsColor" << YAML::Value << (geometry->isColor() == true ? "true" : "false");
    if (geometry->isColor()) {
      out << YAML::Key << "Color" << YAML::Value << geometry->GetColor();
    } else {
      Yeager::Texture2D* tex = geometry->GetTexture();
      out << YAML::Key << "Texture" << YAML::Value << YAML::BeginMap;
      out << YAML::Key << "Path" << YAML::Value << tex->GetPath();
      out << YAML::Key << "ID" << YAML::Value << tex->GetID();
      out << YAML::Key << "Type" << YAML::Value << tex->GetTypeName();
      out << YAML::EndMap;
    }
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;
  std::ofstream fout(path.c_str());
  fout << out.c_str();
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
    Yeager::Log(INFO, kSystem, "Reading shader configuration {}", node["SceneShader"].as<yg_string>());
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
        Yeager::Log(ERROR, kSystem, "Exception thrown at reading shaders from config {}", exc.msg);
      }
    }
  }
}
