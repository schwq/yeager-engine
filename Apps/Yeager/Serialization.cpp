#include "Serialization.h"
#include "Scene.h"
using namespace Yeager;

YAML::Emitter& Yeager::operator<<(YAML::Emitter& out, const yg_vec3& vector)
{
  out << YAML::Flow;
  out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
  return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const YgMatrix4& matrix)
{
  out << YAML::Flow;
  out << YAML::BeginSeq;
  for (unsigned int x = 0; x < matrix.length(); x++) {
    out << matrix[x];
  }
  out << YAML::EndSeq;
  return out;
}

template <typename Type>
void inline Serialization::SerializeObject(YAML::Emitter& out, const char* key, Type obj)
{
  out << YAML::Key << key << YAML::Value << obj;
}

void inline Serialization::SerializeBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip)
{
  out << YAML::Key << key << YAML::Value << manip;
}

void inline Serialization::SerializeBasicObjectType(YAML::Emitter& out, Yeager::Object* obj)
{
  SerializeObject(out, "Position", obj->GetTransformation().position);
  SerializeObject(out, "Rotation", obj->GetTransformation().rotation);
  SerializeObject(out, "Scale", obj->GetTransformation().scale);
  SerializeObject(out, "Path", obj->GetPath());
  SerializeObject(out, "Geometry", ObjectGeometryTypeToString(obj->GetGeometry()));
  SerializeBegin(out, "TexturesLoaded", YAML::BeginMap);

  if (obj->GetGeometry() == ObjectGeometryType::ECustom) {
    for (unsigned int y = 0; y < obj->GetModelData()->TexturesLoaded.size(); y++) {
      ObjectTexture tex = obj->GetModelData()->TexturesLoaded.at(y)->first;
      SerializeObject(out, "Path", tex.Path);
      SerializeObject(out, "Type", tex.Type);
      SerializeObject(out, "Name", tex.Name);
      SerializeObject(out, "Flip", tex.FlipImage);
    }
  } else {
    for (unsigned int y = 0; y < obj->GetGeometryData()->Textures.size(); y++) {
      ObjectTexture tex = obj->GetGeometryData()->Textures.at(y);
      SerializeObject(out, "Path", tex.Path);
      SerializeObject(out, "Type", tex.Type);
      SerializeObject(out, "Name", tex.Name);
      SerializeObject(out, "Flip", tex.FlipImage);
    }
  }
  out << YAML::EndMap;
}

inline void Yeager::Serialization::SerializeSystemInfo(YAML::Emitter& out, Yeager::Scene* scene)
{
#ifdef YEAGER_SYSTEM_WINDOWS_x64
  out << YAML::Key << "Operating System" << YAML::Value << "Windows_x64";
#elif defined(YEAGER_SYSTEM_LINUX)
  out << YAML::Key << "Operating System" << YAML::Value << "Linux";
#endif
}

Serialization::Serialization(Yeager::ApplicationCore* app) : m_Application(app) {}

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
  out << YAML::Comment(
      "DO NOT CHANGE THIS FILE! (Keep in mind that editing this file might corrupt your saving! Only do if you "
      "understand the architeture of the save file!)");

  SerializeSystemInfo(out, scene);
  SerializeObject(out, "Scene", scene->GetContext().m_name);
  SerializeObject(out, "Author", scene->GetContext().m_ProjectAuthor);
  SerializeObject(out, "Renderer", SceneRendererToString(scene->GetContext().m_renderer));
  SerializeObject(out, "SceneType", SceneTypeToString(scene->GetContext().m_type));
  SerializeObject(out, "Camera Position", m_Application->GetCamera()->GetPosition());
  SerializeObject(out, "Camera Direction", m_Application->GetCamera()->GetDirection());
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

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "InstancedAnimatedObject");
    SerializeObject(out, "InstancedCount", obj->GetInstancedNumber());
    SerializeBegin(out, "Props", YAML::BeginSeq);
    for (auto& prop : *obj->GetProps()) {
      SerializeObjectTransformation(out, "ObjectTransformation", prop);
    }
    out << YAML::EndSeq;
    SerializeBasicObjectType(out, obj);
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetLightSources()->size(); x++) {
    LightSource* obj = scene->GetLightSources()->at(x).get();
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "LightSource");
    SerializeObject(out, "DrawableShaderVar", obj->GetDrawableShader()->GetVarName());
    SerializeBegin(out, "LinkedShaders", YAML::BeginSeq);
    for (unsigned int x = 0; x < obj->GetLinkedShader()->size(); x++) {
      Shader* shader = obj->GetLinkedShader()->at(x);
      out << YAML::BeginMap;
      SerializeObject(out, "VarName", shader->GetVarName());
      out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    SerializeBegin(out, "DirectionalLight", YAML::BeginMap);
    SerializeObject(out, "Direction", obj->GetDirectionalLight()->Direction);
    SerializeObject(out, "Ambient", obj->GetDirectionalLight()->Ambient);
    SerializeObject(out, "Diffuse", obj->GetDirectionalLight()->Diffuse);
    SerializeObject(out, "Specular", obj->GetDirectionalLight()->Specular);
    SerializeObject(out, "Color", obj->GetDirectionalLight()->Color);
    out << YAML::EndMap;

    SerializeBegin(out, "Material", YAML::BeginMap);
    SerializeObject(out, "Shininess", obj->GetMaterial()->Shininess);
    out << YAML::EndMap;

    SerializeBegin(out, "SpotLight", YAML::BeginMap);
    SerializeObject(out, "Position", obj->GetSpotLight()->Position);
    SerializeObject(out, "Direction", obj->GetSpotLight()->Direction);
    SerializeObject(out, "CutOff", obj->GetSpotLight()->CutOff);
    SerializeObject(out, "OuterCutOff", obj->GetSpotLight()->OuterCutOff);
    SerializeObject(out, "Constant", obj->GetSpotLight()->Constant);
    SerializeObject(out, "Linear", obj->GetSpotLight()->Linear);
    SerializeObject(out, "Quadratic", obj->GetSpotLight()->Quadratic);
    SerializeObject(out, "Ambient", obj->GetSpotLight()->Ambient);
    SerializeObject(out, "Diffuse", obj->GetSpotLight()->Diffuse);
    SerializeObject(out, "Specular", obj->GetSpotLight()->Specular);
    SerializeObject(out, "Active", obj->GetSpotLight()->Active);
    SerializeObject(out, "ObjectPointLightsCount", obj->GetObjectPointLights()->size());
    out << YAML::EndMap;
    SerializeBegin(out, "ObjectPointLight", YAML::BeginSeq);
    for (unsigned int x = 0; x < obj->GetObjectPointLights()->size(); x++) {
      ObjectPointLight light = obj->GetObjectPointLights()->at(x);
      out << YAML::BeginMap;
      SerializeBasicEntity(out, light.ObjSource->GetName(), light.ObjSource->GetId(), "ObjectPointLight");
      SerializeBasicObjectType(out, light.ObjSource);
      /** Remember, this object point light, have a base class point light, which have a position variable, 
       * the object point light ONLY serialize the position of its engine object */
      SerializeObject(out, "Constant", light.Constant);
      SerializeObject(out, "Linear", light.Linear);
      SerializeObject(out, "Quadratic", light.Quadratic);
      SerializeObject(out, "Ambient", light.Ambient);
      SerializeObject(out, "Diffuse", light.Diffuse);
      SerializeObject(out, "Specular", light.Specular);
      SerializeObject(out, "Active", light.Active);
      SerializeObject(out, "Color", light.Color);
      out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    out << YAML::EndMap;
  }

  out << YAML::EndSeq;

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
        YgString name = shader["Shader"].as<YgString>();
        YgString fragment = shader["FragmentPath"].as<YgString>();
        YgString vertex = shader["VertexPath"].as<YgString>();
        YgString var = shader["VarName"].as<YgString>();
        auto ps_shader = std::make_shared<Yeager::Shader>(GetPath(fragment).c_str(), GetPath(vertex).c_str(), name, m_Application);
        ps_shader->SetVarName(var);
        m_Application->AddConfigShader(ps_shader, var);
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
    if (node["Camera Position"]) {
      m_Application->GetCamera()->SetPosition(node["Camera Position"].as<YgVector3>());
    }
    if (node["Camera Direction"]) {
      m_Application->GetCamera()->SetDirection(node["Camera Direction"].as<YgVector3>());
    }
  }
}

ObjectGeometryType inline Serialization::DeserializeBasicObject(Yeager::Object* BaseClassObj,
                                                                YAML::detail::iterator_value& entity)
{
  BaseClassObj->GetTransformationPtr()->position = entity["Position"].as<YgVector3>();
  BaseClassObj->GetTransformationPtr()->rotation = entity["Rotation"].as<YgVector3>();
  BaseClassObj->GetTransformationPtr()->scale = entity["Scale"].as<YgVector3>();
  ObjectGeometryType geometry = StringToObjectGeometryType(entity["Geometry"].as<YgString>());
  BaseClassObj->SetGeometry(geometry);
  return geometry;
}

std::vector<Transformation> inline Serialization::DeserializeObjectProperties(YAML::detail::iterator_value& entity)
{
  std::vector<Transformation> tr;
  YAML::Node props = entity["Props"];
  for (const auto& propertie : props) {
    Transformation trans;
    trans.position = propertie["Position"].as<YgVector3>();
    trans.scale = propertie["Scale"].as<YgVector3>();
    trans.rotation = propertie["Rotation"].as<YgVector3>();
    tr.push_back(trans);
  }
  return tr;
}

void inline Serialization::DeserializeEntity(Yeager::Scene* scene, YAML::Node& node,
                                             YAML::detail::iterator_value& entity)
{
  /// All entities have theses values, name, id and type, if for some reason, one of them is missing, them the user have change something in the configuration file itself
  /// or someone implemented a new type of entity without the proper configuration in the serialization class for saving it correctly
  unsigned int id = entity["Entity"].as<unsigned int>();
  YgString name = entity["Name"].as<YgString>();
  YgString type = entity["Type"].as<YgString>();
    
  switch (StringToInteger(type.c_str())) {

    case StringToInteger("AudioHandle"):
    {
      YgString path = entity["Path"].as<YgString>();
      auto audio = std::make_shared<Yeager::AudioHandle>(path, name, m_Application->GetAudioEngine(), false);
      scene->GetAudios()->push_back(audio);
      auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
      toolbox->SetEntity(audio.get());
      toolbox->SetType(EExplorerTypeAudio);
      scene->GetToolboxs()->push_back(toolbox);
    } break;

    case StringToInteger("Audio3DHandle"): 
    {
      YgString path = entity["Path"].as<YgString>();
      YgVector3 position = entity["Position"].as<YgVector3>();
      auto audio = std::make_shared<Yeager::Audio3DHandle>(path, name, m_Application->GetAudioEngine(), false,
                                                           YgVec3_to_Vec3df(position));
      scene->GetAudios3D()->push_back(audio);
      auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
      toolbox->SetType(EExplorerTypeAudio3D);
      toolbox->SetEntity(audio.get());
      scene->GetToolboxs()->push_back(toolbox);
    } break;

    case StringToInteger("Object"): 
    {
      auto obj = std::make_shared<Yeager::Object>(name, m_Application);
      // Gets transformation, and geometry of the object serialized
      ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
      bool succceded = true;

      if (geometry == ObjectGeometryType::ECustom) {
        // Import Custom model object
        if (!obj->ThreadImportObjectFromFile(entity["Path"].as<YgString>().c_str())) {
          Yeager::Log(WARNING, "Error importing object from file during deserialization!");
          succceded = false;
        }

      } else {
        // Generates geometry for object
        if (!obj->GenerateObjectGeometry(geometry)) {
          Yeager::Log(WARNING, "Error generating object geometry during deserialization!");
          succceded = false;
        }
      }
      // Push to scene vector if succeded
      if (succceded) {
        scene->GetObjects()->push_back(obj);
      }
    } break;

    case StringToInteger("InstancedObject"): 
    {
      unsigned int instanced = entity["InstancedCount"].as<unsigned int>();
      auto obj = std::make_shared<Yeager::InstancedObject>(name, m_Application, instanced);
      ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
      std::vector<Transformation> positions = DeserializeObjectProperties(entity);
      obj->BuildProp(positions, m_Application->ShaderFromVarName("SimpleInstanced"));
      bool succceded = true;

      if (geometry == ObjectGeometryType::ECustom) {
        if (!obj->ThreadImportObjectFromFile(entity["Path"].as<YgString>().c_str())) {
          Yeager::Log(WARNING, "Error importing instanced object from file during deserialization!");
          succceded = false;
        }
      } else {
        if (!obj->GenerateObjectGeometry(geometry)) {
          Yeager::Log(WARNING, "Error generating instanced object geometry during deserialization!");
          succceded = false;
        }
      }
      if (succceded) {
        scene->GetInstancedObjects()->push_back(obj);
      }
    } break;

    case StringToInteger("AnimatedObject"): {
      auto obj = std::make_shared<Yeager::AnimatedObject>(name, m_Application);
      ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
      bool succceded = true;

      if (geometry == ObjectGeometryType::ECustom) {
        if (!obj->ThreadImportObjectFromFile(entity["Path"].as<YgString>().c_str(), false)) {
          Yeager::Log(WARNING, "Error importing animated object from file during deserialization!");
          succceded = false;
        }
      } else {
        if (!obj->GenerateObjectGeometry(geometry)) {
          Yeager::Log(WARNING, "Error generating animated object geometry during deserialization!");
          succceded = false;
        }
      }
      if (succceded) {
        obj->BuildAnimation(entity["Path"].as<YgString>());
        scene->GetAnimatedObject()->push_back(obj);
      }
    } break;

    case StringToInteger("InstancedAnimatedObject"): 
    {
      unsigned int instanced = entity["InstancedCount"].as<unsigned int>();
      auto obj = std::make_shared<Yeager::InstancedAnimatedObject>(name, m_Application, instanced);
      ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
      std::vector<Transformation> positions = DeserializeObjectProperties(entity);
      obj->BuildProp(positions, m_Application->ShaderFromVarName("SimpleInstancedAnimated"));

      bool succceded = true;

      if (geometry == ObjectGeometryType::ECustom) {
        if (!obj->ThreadImportObjectFromFile(entity["Path"].as<YgString>().c_str(), false)) {
          Yeager::Log(WARNING, "Error importing instanced animated object from file during deserialization!");
          succceded = false;
        }
      } else {
        if (!obj->GenerateObjectGeometry(geometry)) {
          Yeager::Log(WARNING, "Error generating instanced animated object geometry during deserialization!");
          succceded = false;
        }
      }
      if (succceded) {
        obj->BuildAnimation(entity["Path"].as<YgString>());
        scene->GetInstancedAnimatedObjects()->push_back(obj);
      }
    } break;
    
    case StringToInteger("LightSource"):
    {
      YgString drawable_shader_var = entity["DrawableShaderVar"].as<YgString>();
      std::vector<Shader*> link_shader;
      for (const auto& shader : entity["LinkedShaders"]) {
        if (m_Application->ShaderFromVarName(shader["VarName"].as<YgString>())->IsInitialized()) {
          link_shader.push_back(m_Application->ShaderFromVarName(shader["VarName"].as<YgString>()));
        } else {
          Yeager::Log(WARNING, "Configuration file trying to push a uninitialized shader to the light source!");
        }
      }
      auto obj = std::make_shared<Yeager::LightSource>(name, m_Application, link_shader,
                                                       m_Application->ShaderFromVarName(drawable_shader_var));
      YAML::Node DirectionalLight = entity["DirectionalLight"];
      obj->GetDirectionalLight()->Direction = DirectionalLight["Direction"].as<YgVector3>();
      obj->GetDirectionalLight()->Ambient = DirectionalLight["Ambient"].as<YgVector3>();
      obj->GetDirectionalLight()->Diffuse = DirectionalLight["Diffuse"].as<YgVector3>();
      obj->GetDirectionalLight()->Specular = DirectionalLight["Specular"].as<YgVector3>();
      obj->GetDirectionalLight()->Color = DirectionalLight["Color"].as<YgVector3>();

      YAML::Node Material = entity["Material"];
      obj->GetMaterial()->Shininess = Material["Shininess"].as<float>();

      YAML::Node SpotLight = entity["SpotLight"];
      obj->GetSpotLight()->Position = SpotLight["Position"].as<YgVector3>();
      obj->GetSpotLight()->Direction = SpotLight["Direction"].as<YgVector3>();
      obj->GetSpotLight()->CutOff = SpotLight["CutOff"].as<float>();
      obj->GetSpotLight()->OuterCutOff = SpotLight["OuterCutOff"].as<float>();
      obj->GetSpotLight()->Constant = SpotLight["Constant"].as<float>();
      obj->GetSpotLight()->Linear = SpotLight["Linear"].as<float>();
      obj->GetSpotLight()->Quadratic = SpotLight["Quadratic"].as<float>();
      obj->GetSpotLight()->Ambient = SpotLight["Ambient"].as<YgVector3>();
      obj->GetSpotLight()->Diffuse = SpotLight["Diffuse"].as<YgVector3>();
      obj->GetSpotLight()->Specular = SpotLight["Specular"].as<YgVector3>();
      obj->GetSpotLight()->Active = SpotLight["Active"].as<bool>();

      for (const auto& pointLight : entity["ObjectPointLight"]) {
        ObjectPointLight light;
        Transformation trans;
        light.Constant = pointLight["Constant"].as<float>();
        light.Linear = pointLight["Linear"].as<float>();
        light.Quadratic = pointLight["Quadratic"].as<float>();
        light.Ambient = pointLight["Ambient"].as<YgVector3>();
        light.Diffuse = pointLight["Diffuse"].as<YgVector3>();
        light.Specular = pointLight["Specular"].as<YgVector3>();
        light.Active = pointLight["Active"].as<bool>();
        light.Color = pointLight["Color"].as<YgVector3>();
        trans.position = pointLight["Position"].as<YgVector3>();
        trans.rotation = pointLight["Rotation"].as<YgVector3>();
        trans.scale = pointLight["Scale"].as<YgVector3>();
        obj->AddObjectPointLight(light, trans);
      }

      scene->GetLightSources()->push_back(obj);
    } break;

    default:
      Yeager::Log(WARNING, "Entity Type cannot been found! Value [{}]", type);
    }
}

void Serialization::ReadEditorVariables(YgCchar path)
{
  YAML::Node node = YAML::LoadFile(path);
  if (node) {
    if (node["EditorScreenWidth"]) {
      ygWindowWidth = node["EditorScreenWidth"].as<int>();
    }
    if (node["EditorScreenHeight"]) {
      ygWindowHeight = node["EditorScreenHeight"].as<int>();
    }
  }
}

void Serialization::SaveEditorVariables(YgCchar path)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  SerializeObject(out, "EditorScreenWidth", ygWindowWidth);
  SerializeObject(out, "EditorScreenHeight", ygWindowHeight);

  out << YAML::EndMap;

  std::ofstream fout;
  fout.open(path);
  if (fout.is_open()) {
    fout << out.c_str();
  } else {
    Yeager::Log(ERROR, "Cannot create editor variables configuration file {}", path);
  }
}

void Serialization::ReadLoadedProjectsHandles(YgString externalFolder) {
  YAML::Node node = YAML::LoadFile(externalFolder + YG_PS + "LoadedProjectsPath.yml");
  for (const auto& project : node["ProjectsLoaded"]) {
    LoadedProjectHandle handle;
    handle.m_ProjectName = project["ProjectName"].as<YgString>();
    handle.m_ProjectFolderPath = project["ProjectFolderPath"].as<YgString>();
    handle.m_ProjectConfigurationPath = project["ProjectConfigurationPath"].as<YgString>();
    m_Application->GetLoadedProjectsHandles()->push_back(handle);
  }
}

void Serialization::WriteLoadedProjectsHandles(YgString externalFolder) {
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "ProjectsLoaded" << YAML::Value << YAML::BeginSeq;
  for (const auto& project : *m_Application->GetLoadedProjectsHandles()) {
    out << YAML::BeginMap;
    out << YAML::Key << "ProjectName" << YAML::Value << project.m_ProjectName;
    out << YAML::Key << "ProjectFolderPath" << YAML::Value << project.m_ProjectFolderPath;
    out << YAML::Key << "ProjectConfigurationPath" << YAML::Value << project.m_ProjectConfigurationPath;
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream of(externalFolder + YG_PS + "LoadedProjectsPath.yml");
  if (of.is_open()) {
    of << out.c_str();
  } else {
    Yeager::Log(ERROR, "Cannot open external project paths configuration file!");
  }
  of.close();
}