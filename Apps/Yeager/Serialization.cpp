#include "Serialization.h"
#include "Application.h"
#include "Engine/Renderer/Skybox.h"
#include "Scene.h"
using namespace Yeager;

/* TODO Remake this struct about project`s information, there are already 3 of them in the engine, must refactor*/
std::vector<OpenProjectsDisplay> Yeager::ReadProjectsToDisplay(YgString dir)
{
  std::vector<OpenProjectsDisplay> proj;
  YAML::Node node = YAML::LoadFile(dir);
  /* Reads the configuration file in the external folder of the engine, and gather information about projects in the current machine*/
  if (node["ProjectsLoaded"]) {
    for (const auto& project : node["ProjectsLoaded"]) {
      OpenProjectsDisplay disp;
      bool exists;

      if (project["ProjectName"]) {
        disp.Name = project["ProjectName"].as<YgString>();
      }

      if (project["ProjectFolderPath"]) {
        disp.FolderPath = project["ProjectFolderPath"].as<YgString>();
      }
      /* Reads each project configuration and return basic information about them*/
      if (project["ProjectConfigurationPath"]) {
        /* The variable inside the configuration file is alright, but the path points to a unknown place, we wouldnt load 
            TODO make the user choice the right place by displaying a screen for troubleshoot */
        exists = Yeager::ValidatesPath(project["ProjectConfigurationPath"].as<YgString>());

        if (exists) {
          YAML::Node indiv_proj = YAML::LoadFile(project["ProjectConfigurationPath"].as<YgString>());
          disp.Path = project["ProjectConfigurationPath"].as<YgString>();
          if (indiv_proj["Renderer"]) {
            disp.RendererType = indiv_proj["Renderer"].as<YgString>();
          }
          if (indiv_proj["SceneType"]) {
            disp.SceneType = indiv_proj["SceneType"].as<YgString>();
          }
          if (indiv_proj["Author"]) {
            disp.Author = indiv_proj["Author"].as<YgString>();
          }
        } else {
          Yeager::Log(ERROR,
                      "The engine external configuration points to a unknown project configuration! Verify if the "
                      "project configuration file have the same name as the project!");
        }
      } else {
        Yeager::Log(ERROR, "Cannot find configuration file path for the associate project {}", disp.Name);
      }
      /* We wont add a invalid project path to the engine display! this will cause a exception been thrown!*/
      if (exists) {
        proj.push_back(disp);
      }
    }
  } else {
    Yeager::Log(WARNING, "The current state of the engine does not have any project saved!");
  }
  return proj;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const YgVector3& vector)
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
      ObjectTexture tex = obj->GetModelData()->TexturesLoaded.at(y);
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

void inline Serialization::SerializeBasicEntity(YAML::Emitter& out, YgString name, unsigned int id, YgString type)
{
  out << YAML::Key << "Entity" << YAML::Value << id;
  out << YAML::Key << "Name" << YAML::Value << name;
  out << YAML::Key << "Type" << YAML::Value << type;
}

void inline Serialization::SerializeObjectTransformation(YAML::Emitter& out, YgString name,
                                                         Yeager::Transformation& transf) noexcept
{
  out << YAML::BeginMap;
  out << YAML::Key << "Position" << YAML::Value << transf.position;
  out << YAML::Key << "Rotation" << YAML::Value << transf.rotation;
  out << YAML::Key << "Scale" << YAML::Value << transf.scale;
  out << YAML::EndMap;
}

void Serialization::SerializeScene(Yeager::Scene* scene, YgString path)
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

  if (scene->GetCurrentSkybox() != YEAGER_NULLPTR) {
    out << YAML::BeginMap;
    SerializeBasicEntity(out, scene->GetCurrentSkybox()->GetName(), scene->GetCurrentSkybox()->GetId(), "Skybox");
    SerializeObject(out, "Path", scene->GetCurrentSkybox()->GetPath());
    out << YAML::EndMap;
  }

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

  for (unsigned int x = 0; x < scene->GetObjects()->size(); x++) {

    Object* obj = scene->GetObjects()->at(x).get();
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "Object");
    SerializeBasicObjectType(out, obj);
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetInstancedObjects()->size(); x++) {
    InstancedObject* obj = scene->GetInstancedObjects()->at(x).get();
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "InstacedObject");
    SerializeObject(out, "InstancedCount", obj->GetInstancedNumber());
    SerializeBegin(out, "Props", YAML::BeginSeq);
    for (auto& prop : *obj->GetProps()) {
      SerializeObjectTransformation(out, "ObjectTransformation", prop);
    }
    out << YAML::EndSeq;
    SerializeBasicObjectType(out, obj);
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetAnimatedObject()->size(); x++) {
    AnimatedObject* obj = scene->GetAnimatedObject()->at(x).get();
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "AnimatedObject");
    SerializeBasicObjectType(out, obj);
    out << YAML::EndMap;
  }

  for (unsigned int x = 0; x < scene->GetInstancedAnimatedObjects()->size(); x++) {
    InstancedAnimatedObject* obj = scene->GetInstancedAnimatedObjects()->at(x).get();
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
    SerializeObject(out, "LinkedShaderVar", obj->GetLinkedShader()->GetVarName());

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

  if (fout.is_open()) {
    fout << out.c_str();
  } else {
    Yeager::Log(ERROR, "Cannot write to configuration file! {}", path.c_str());
  }

  fout.close();
}

template <typename Type>
const void inline Serialization::CheckAndDeserialize(YAML::Node& node, Type& obj, YgCchar key) noexcept
{
  if (node[key]) {
    const auto result = node[key].as<Type>();
    obj = result;
  } else {
    Yeager::Log(ERROR, "Cannot deserialize: {}", key);
  }
}

ColorschemeConfig Serialization::ReadColorschemeConfig()
{
  ColorschemeConfig colorscheme;
  YAML::Node node = YAML::LoadFile(GetPath("/Configuration/Editor/Colorschemes/dark_mode.yaml"));

  CheckAndDeserialize(node, colorscheme.WindowRounding, "WindowRounding");
  CheckAndDeserialize(node, colorscheme.FrameRounding, "FrameRounding");

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
    ImVec4 vector = NodeVec4ToImVec4(node, "Colpositionors_ImGuiCol_ChildBg");
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
        ps_shader->SetVarName(var);
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
    /**
     * Beyond this part, all objects are processed in this function,
     * Instanced, animated, static, and single objects
     */
  } else if (type == "Object") {

    auto obj = std::make_shared<Yeager::Object>(name, m_Application);
    // Gets transformation, and geometry of the object serialized
    ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
    bool succceded = true;

    if (geometry == ObjectGeometryType::ECustom) {
      // Import Custom model object
      if (!obj->ImportObjectFromFile(entity["Path"].as<YgString>().c_str())) {
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

  } else if (type == "InstacedObject") {

    unsigned int instanced = entity["InstancedCount"].as<unsigned int>();
    auto obj = std::make_shared<Yeager::InstancedObject>(name, m_Application, instanced);
    ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
    std::vector<Transformation> positions = DeserializeObjectProperties(entity);
    obj->BuildProp(positions, ShaderFromVarName("SimpleInstanced"));
    bool succceded = true;

    if (geometry == ObjectGeometryType::ECustom) {
      if (!obj->ImportObjectFromFile(entity["Path"].as<YgString>().c_str())) {
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
  } else if (type == "AnimatedObject") {
    auto obj = std::make_shared<Yeager::AnimatedObject>(name, m_Application);
    ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
    bool succceded = true;

    if (geometry == ObjectGeometryType::ECustom) {
      if (!obj->ImportObjectFromFile(entity["Path"].as<YgString>().c_str(), false)) {
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
  } else if (type == "InstancedAnimatedObject") {
    unsigned int instanced = entity["InstancedCount"].as<unsigned int>();
    auto obj = std::make_shared<Yeager::InstancedAnimatedObject>(name, m_Application, instanced);
    ObjectGeometryType geometry = DeserializeBasicObject(obj.get(), entity);
    std::vector<Transformation> positions = DeserializeObjectProperties(entity);
    obj->BuildProp(positions, ShaderFromVarName("SimpleInstancedAnimated"));

    bool succceded = true;

    if (geometry == ObjectGeometryType::ECustom) {
      if (!obj->ImportObjectFromFile(entity["Path"].as<YgString>().c_str(), false)) {
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
  } else if (type == "LightSource") {
    YgString drawable_shader_var = entity["DrawableShaderVar"].as<YgString>();
    YgString linked_shader_var = entity["LinkedShaderVar"].as<YgString>();
    auto obj = std::make_shared<Yeager::LightSource>(name, m_Application, ShaderFromVarName(linked_shader_var),
                                                     ShaderFromVarName(drawable_shader_var));
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
  } else if (type == "Skybox") {
    YgString path = entity["Path"].as<YgString>();
    scene->GetCurrentSkybox()->BuildSkyboxFromImport(path);
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