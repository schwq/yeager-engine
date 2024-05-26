#include "Serialization.h"
#include "Application.h"
#include "Scene.h"
using namespace Yeager;

/* TODO Remake this struct about project`s information, there are already 3 of them in the engine, must refactor*/
std::vector<OpenProjectsDisplay> Yeager::ReadProjectsToDisplay(String dir, Yeager::ApplicationCore* app)
{
  std::vector<OpenProjectsDisplay> proj;
  YAML::Node node = YAML::LoadFile(dir);
  /* Reads the configuration file in the external folder of the engine, and gather information about projects in the current machine*/
  if (node["ProjectsLoaded"]) {
    for (const auto& project : node["ProjectsLoaded"]) {
      OpenProjectsDisplay disp;
      bool exists;

      if (project["ProjectName"]) {
        disp.Name = project["ProjectName"].as<String>();
      }

      if (project["ProjectFolderPath"]) {
        disp.FolderPath = project["ProjectFolderPath"].as<String>();
      }
      /* Reads each project configuration and return basic information about them*/
      if (project["ProjectConfigurationPath"]) {
        /* The variable inside the configuration file is alright, but the path points to a unknown place, we wouldnt load 
            TODO make the user choice the right place by displaying a screen for troubleshoot */
        exists = Yeager::ValidatesPath(project["ProjectConfigurationPath"].as<String>());

        if (exists) {
          YAML::Node indiv_proj = YAML::LoadFile(project["ProjectConfigurationPath"].as<String>());
          disp.Path = project["ProjectConfigurationPath"].as<String>();
          if (indiv_proj["Renderer"]) {
            disp.RendererType = indiv_proj["Renderer"].as<String>();
          }
          if (indiv_proj["SceneType"]) {
            disp.SceneType = indiv_proj["SceneType"].as<String>();
          }
          if (indiv_proj["Author"]) {
            disp.Author = indiv_proj["Author"].as<String>();
          }
          if (indiv_proj["TimeOfCreation"]) {
            disp.TimeOfCreation = app->GetSerial()->DeserializeProjectTimeOfCreation(indiv_proj["TimeOfCreation"]);
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

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& vector)
{
  out << YAML::Flow;
  out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
  return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Matrix4& matrix)
{
  out << YAML::Flow;
  out << YAML::BeginSeq;
  for (Uint x = 0; x < matrix.length(); x++) {
    out << matrix[x];
  }
  out << YAML::EndSeq;
  return out;
}

template <typename Type>
void YEAGER_FORCE_INLINE Serialization::SerializeObject(YAML::Emitter& out, const char* key, Type obj)
{
  out << YAML::Key << key << YAML::Value << obj;
}

void YEAGER_FORCE_INLINE Serialization::SerializeBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip)
{
  out << YAML::Key << key << YAML::Value << manip;
}

void YEAGER_FORCE_INLINE Serialization::SerializeBasicObjectType(YAML::Emitter& out, Yeager::Object* obj)
{
  SerializeObject(out, "Position", obj->GetTransformation().position);
  SerializeObject(out, "Rotation", obj->GetTransformation().rotation);
  SerializeObject(out, "Scale", obj->GetTransformation().scale);
  SerializeObject(out, "Path", obj->GetPath());
  SerializeObject(out, "Geometry", ObjectGeometryTypeToString(obj->GetGeometry()));
  SerializeBegin(out, "TexturesLoaded", YAML::BeginSeq);

  if (obj->GetGeometry() == ObjectGeometryType::eCUSTOM) {
    for (Uint y = 0; y < obj->GetModelData()->TexturesLoaded.size(); y++) {
      out << YAML::BeginMap;
      MaterialTexture2D* tex = &obj->GetModelData()->TexturesLoaded.at(y)->first;
      SerializeObject(out, "Path", tex->GetPath());
      SerializeObject(out, "Type", MaterialTextureType::ToString(tex->GetTextureType()));
      SerializeObject(out, "Name", tex->GetName());
      SerializeObject(out, "Flip", tex->GetTextureDataHandle()->Flipped);
      out << YAML::EndMap;
    }
  } else {
    out << YAML::BeginMap;
    MaterialTexture2D* tex = obj->GetGeometryData()->Texture;

    SerializeObject(out, "Path", tex->GetPath());
    SerializeObject(out, "Type", MaterialTextureType::ToString(tex->GetTextureType()));
    SerializeObject(out, "Name", tex->GetName());
    SerializeObject(out, "Flip", tex->GetTextureDataHandle()->Flipped);
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
}

YEAGER_FORCE_INLINE void Serialization::SerializeSystemInfo(YAML::Emitter& out, Yeager::Scene* scene)
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

void YEAGER_FORCE_INLINE Serialization::SerializeBasicEntity(YAML::Emitter& out, String name, Uint id, String type)
{
  out << YAML::Key << "Entity" << YAML::Value << id;
  out << YAML::Key << "Name" << YAML::Value << name;
  out << YAML::Key << "Type" << YAML::Value << type;
}

void YEAGER_FORCE_INLINE Serialization::SerializeObjectTransformation(YAML::Emitter& out, String name,
                                                                      Yeager::Transformation& transf) noexcept
{

  out << YAML::BeginMap;
  out << YAML::Key << "Position" << YAML::Value << transf.position;
  out << YAML::Key << "Scale" << YAML::Value << transf.position;
  out << YAML::Key << "Rotation" << YAML::Value << transf.position;
  out << YAML::Key << "Model" << YAML::Value << transf.model;
  out << YAML::EndMap;
}

void Serialization::SerializeScene(Yeager::Scene* scene, String path)
{
  YAML::Emitter out;

  out << YAML::BeginMap;
  out << YAML::Comment(
      "DO NOT CHANGE THIS FILE! (Keep in mind that editing this file might corrupt your saving! Only do if you "
      "understand the architeture of the save file!)");

  SerializeSystemInfo(out, scene);
  SerializeObject(out, "Scene", scene->GetContext()->m_name);
  SerializeProjectTimeOfCreation(out, scene, "TimeOfCreation");
  SerializeObject(out, "Author", scene->GetContext()->m_ProjectAuthor);
  SerializeObject(out, "Renderer", SceneRendererToString(scene->GetContext()->m_renderer));
  SerializeObject(out, "SceneType", SceneTypeToString(scene->GetContext()->m_ExplorerType));
  SerializeObject(out, "Camera Position", m_Application->GetCamera()->GetPosition());
  SerializeObject(out, "Camera Direction", m_Application->GetCamera()->GetFront());
  SerializeBegin(out, "SceneEntities", YAML::BeginSeq);

  for (Uint x = 0; x < scene->GetAudios()->size(); x++) {
    AudioHandle* audio = scene->GetAudios()->at(x).get();
    if (!audio->CanBeSerialize())
      continue;
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetId(), "AudioHandle");
    SerializeObject(out, "Path", audio->GetPath());
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetAudios3D()->size(); x++) {
    Audio3DHandle* audio = scene->GetAudios3D()->at(x).get();
    if (!audio->CanBeSerialize())
      continue;
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetId(), "Audio3DHandle");
    SerializeObject(out, "Path", audio->GetPath());
    SerializeObject(out, "Position", audio->GetVector3Position());
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetObjects()->size(); x++) {

    Object* obj = scene->GetObjects()->at(x).get();
    if (!obj->CanBeSerialize()) {
      continue;
    }
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "Object");
    SerializeObject(out, "InstancedType", ObjectInstancedType::EnumToString(obj->GetInstancedType()));
    if (obj->IsInstanced()) {
      SerializeObject(out, "InstancedCount", obj->GetInstancedNum());
      SerializeBegin(out, "Props", YAML::BeginSeq);
      for (auto& prop : *obj->GetInstancedProps()) {

        SerializeObjectTransformation(out, "ObjectTransformation", *prop);
      }
      out << YAML::EndSeq;
    }
    SerializeBasicObjectType(out, obj);
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetAnimatedObject()->size(); x++) {
    AnimatedObject* obj = scene->GetAnimatedObject()->at(x).get();
    if (!obj->CanBeSerialize())
      continue;
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "AnimatedObject");
    SerializeBasicObjectType(out, obj);
    SerializeObject(out, "InstancedType", ObjectInstancedType::EnumToString(obj->GetInstancedType()));
    if (obj->IsInstanced()) {
      SerializeObject(out, "InstancedCount", obj->GetInstancedNum());
      SerializeBegin(out, "Props", YAML::BeginSeq);
      for (auto& prop : *obj->GetInstancedProps()) {

        SerializeObjectTransformation(out, "ObjectTransformation", *prop);
      }
      out << YAML::EndSeq;
    }
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetLightSources()->size(); x++) {
    PhysicalLightHandle* obj = scene->GetLightSources()->at(x).get();
    if (!obj->CanBeSerialize())
      continue;
    out << YAML::BeginMap;

    SerializeBasicEntity(out, obj->GetName(), obj->GetId(), "LightSource");
    SerializeObject(out, "DrawableShaderVar", obj->GetDrawableShader()->GetVarName());

    SerializeBegin(out, "LinkedShaderVar", YAML::BeginSeq);
    for (auto& shader : *obj->GetLinkedShaders()) {
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
    for (Uint x = 0; x < obj->GetObjectPointLights()->size(); x++) {
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

void Serialization::ReadSceneShadersConfig(String path)
{
  YAML::Node node = YAML::LoadFile(path);

  if (node["SceneShader"]) {
    Yeager::Log(INFO, "Reading shader configuration {}", node["SceneShader"].as<String>());
  }
  auto shaders = node["Shaders"];
  if (shaders) {
    for (auto shader : shaders) {
      try {
        String name = shader["Shader"].as<String>();
        String fragment = shader["FragmentPath"].as<String>();
        String vertex = shader["VertexPath"].as<String>();
        String var = shader["VarName"].as<String>();
        auto ps_shader = std::make_shared<Yeager::Shader>(GetPath(fragment).c_str(), GetPath(vertex).c_str(), name);
        ps_shader->SetVarName(var);
        std::pair<std::shared_ptr<Shader>, String> sh;
        sh.first = ps_shader;
        sh.second = var;
        m_Application->GetConfigShaders()->push_back(sh);

      } catch (YAML::BadConversion exc) {
        Yeager::Log(ERROR, "Exception thrown at reading shaders from config {}", exc.msg);
      }
    }
  }
}

void Serialization::DeserializeScene(Yeager::Scene* scene, String path)
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
    } catch (YAML::BadSubscript exc) {
      Yeager::Log(ERROR, "Something went wront at reading the Scene Entities configuration! YAML::BadSubscript!");
    }
  }
}

void YEAGER_FORCE_INLINE Serialization::DeserializeSceneInfo(Yeager::Scene* scene, YAML::Node& node)
{
  if (node) {
    if (node["Scene"]) {
      scene->GetContext()->m_name = node["Scene"].as<String>();
    }
    if (node["Renderer"]) {
      scene->SetContextRenderer(StringToSceneRenderer(node["Renderer"].as<String>()));
    }
    if (node["SceneType"]) {
      scene->SetContextType(StringToSceneType(node["SceneType"].as<String>()));
    }
    if (node["Camera Position"]) {
      m_Application->GetCamera()->SetPosition(node["Camera Position"].as<Vector3>());
    }
    if (node["Camera Direction"]) {
      m_Application->GetCamera()->SetFront(node["Camera Direction"].as<Vector3>());
    }
    if (node["TimeOfCreation"]) {
      scene->GetContext()->m_TimeOfCreation = DeserializeProjectTimeOfCreation(node["TimeOfCreation"]);
    }
  }
}
ObjectGeometryType::Enum YEAGER_FORCE_INLINE Serialization::DeserializeBasicObject(Yeager::Object* BaseClassObj,
                                                                                   YAML::detail::iterator_value& entity)
{
  BaseClassObj->GetTransformationPtr()->position = entity["Position"].as<Vector3>();
  BaseClassObj->GetTransformationPtr()->rotation = entity["Rotation"].as<Vector3>();
  BaseClassObj->GetTransformationPtr()->scale = entity["Scale"].as<Vector3>();
  ObjectGeometryType::Enum geometry = StringToObjectGeometryType(entity["Geometry"].as<String>());
  BaseClassObj->SetGeometry(geometry);
  return geometry;
}

std::vector<Transformation*> YEAGER_FORCE_INLINE
Serialization::DeserializeObjectProperties(YAML::detail::iterator_value& entity)
{
  std::vector<Transformation*> tr;
  YAML::Node props = entity["Props"];
  for (const auto& propertie : props) {
    Transformation* trans = new Transformation();
    trans->position = propertie["Position"].as<Vector3>();
    trans->scale = propertie["Scale"].as<Vector3>();
    trans->rotation = propertie["Rotation"].as<Vector3>();
    tr.push_back(trans);
  }
  return tr;
}

void YEAGER_FORCE_INLINE Serialization::DeserializeEntity(Yeager::Scene* scene, YAML::Node& node,
                                                          YAML::detail::iterator_value& entity)
{
  /** All entities have theses values, name, id and type, if for some reason, one of them is missing, 
  them the user have change something in the configuration file itself
  or someone implemented a new type of entity without the proper configuration in the serialization class for saving it correctly */

  Uint id = entity["Entity"].as<unsigned int>();
  String name = entity["Name"].as<String>();
  String type = entity["Type"].as<String>();

  switch (StringToInteger(type.c_str())) {

    case StringToInteger("AudioHandle"): {
      DeserializeAudioHandle(entity, scene, name, type, id);
    } break;

    case StringToInteger("Audio3DHandle"): {
      DeserializeAudio3DHandle(entity, scene, name, type, id);
    } break;

    case StringToInteger("Object"): {
      DeserializeObject(entity, scene, name, type, id);
    } break;

    case StringToInteger("AnimatedObject"): {
      DeserializeAnimatedObject(entity, scene, name, type, id);
    } break;

    case StringToInteger("LightSource"): {
      DeserializeLightSource(entity, scene, name, type, id);
    } break;

    default:
      Yeager::Log(WARNING, "Entity Type cannot been found! Value [{}]", type);
  }
}

void Serialization::DeserializeAudioHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                           const String& name, const String& type, const Uint id)
{
  String path = entity["Path"].as<String>();
  auto audio = std::make_shared<Yeager::AudioHandle>(path, name, m_Application->GetAudioEngine(), false, m_Application);
  scene->GetAudios()->push_back(audio);
}

void Serialization::DeserializeAudio3DHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                             const String& name, const String& type, const Uint id)
{
  String path = entity["Path"].as<String>();
  Vector3 position = entity["Position"].as<Vector3>();
  auto audio = std::make_shared<Yeager::Audio3DHandle>(path, name, m_Application->GetAudioEngine(), false,
                                                       YgVec3_to_Vec3df(position), m_Application);
  scene->GetAudios3D()->push_back(audio);
};

void Serialization::DeserializeObject(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                                      const String& type, const Uint id)
{
  std::shared_ptr<Yeager::Object> obj;

  String instancedType = entity["InstancedType"].as<String>();
  if (instancedType == "Instanced") {
    obj = std::make_shared<Yeager::Object>(name, m_Application, entity["InstancedCount"].as<int>());
    std::vector<Transformation*> positions = DeserializeObjectProperties(entity);
    obj->BuildProps(positions, m_Application->ShaderFromVarName("SimpleInstanced"));
  } else {
    obj = std::make_shared<Yeager::Object>(name, m_Application);
  }

  // Gets transformation, and geometry of the object serialized
  ObjectGeometryType::Enum geometry = DeserializeBasicObject(obj.get(), entity);
  bool succceded = true;
  bool flip = false;

  auto textures = entity["TexturesLoaded"];
  for (auto texture : textures) {
    String path = texture["Path"].as<String>();
    flip = texture["Flip"].as<bool>();
  }

  if (geometry == ObjectGeometryType::eCUSTOM) {
    // Import Custom model object
    if (!Yeager::ValidatesPath(entity["Path"].as<String>().c_str(), false) ||
        !obj->ThreadImportObjectFromFile(entity["Path"].as<String>().c_str(), flip)) {
      Yeager::Log(ERROR, "Error importing object from file during deserialization!");
      succceded = false;
    }

  } else {
    // Generates geometry for object
    if (!obj->GenerateObjectGeometry(geometry, ObjectPhysXCreationStatic(obj->GetTransformationPtr()->position,
                                                                         obj->GetTransformationPtr()->rotation,
                                                                         obj->GetTransformationPtr()->scale))) {
      Yeager::Log(ERROR, "Error generating object geometry during deserialization!");
      succceded = false;
    }
  }
  // Push to scene vector if succeded
  if (succceded) {
    scene->GetObjects()->push_back(obj);
  }
};

void Serialization::DeserializeAnimatedObject(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                              const String& name, const String& type, const Uint id)
{
  std::shared_ptr<Yeager::AnimatedObject> obj;
  String instancedType = entity["InstancedType"].as<String>();
  if (instancedType == "Instanced") {
    obj = std::make_shared<Yeager::AnimatedObject>(name, m_Application, entity["InstancedCount"].as<int>());
    std::vector<Transformation*> positions = DeserializeObjectProperties(entity);
    obj->BuildProps(positions, m_Application->ShaderFromVarName("SimpleInstanced"));
  } else {
    obj = std::make_shared<Yeager::AnimatedObject>(name, m_Application);
  }

  ObjectGeometryType::Enum geometry = DeserializeBasicObject(obj.get(), entity);
  bool succceded = true;

  if (geometry == ObjectGeometryType::eCUSTOM) {
    if (!Yeager::ValidatesPath(entity["Path"].as<String>().c_str(), false) ||
        !obj->ThreadImportObjectFromFile(entity["Path"].as<String>().c_str(), false)) {
      Yeager::Log(ERROR, "Error importing animated object from file during deserialization!");
      succceded = false;
    }
  } else {
    if (!obj->GenerateObjectGeometry(geometry, ObjectPhysXCreationStatic(obj->GetTransformationPtr()->position,
                                                                         obj->GetTransformationPtr()->rotation,
                                                                         obj->GetTransformationPtr()->scale))) {
      Yeager::Log(ERROR, "Error generating animated object geometry during deserialization!");
      succceded = false;
    }
  }
  if (succceded) {
    obj->BuildAnimation(entity["Path"].as<String>());
    scene->GetAnimatedObject()->push_back(obj);
  }
};
void Serialization::DeserializeLightSource(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                           const String& name, const String& type, const Uint id)
{
  String drawable_shader_var = entity["DrawableShaderVar"].as<String>();
  std::vector<Shader*> link_shader;
  auto linked_shaders = entity["LinkedShaderVar"];
  for (const auto& shader : linked_shaders) {
    if (m_Application->ShaderFromVarName(shader["VarName"].as<String>())->IsInitialized()) {
      link_shader.push_back(m_Application->ShaderFromVarName(shader["VarName"].as<String>()));
    } else {
      Yeager::Log(WARNING, "Configuration file trying to push a uninitialized shader to the light source!");
    }
  }
  auto obj = std::make_shared<Yeager::PhysicalLightHandle>(name, m_Application, link_shader,
                                                           m_Application->ShaderFromVarName(drawable_shader_var));
  YAML::Node DirectionalLight = entity["DirectionalLight"];
  obj->GetDirectionalLight()->Direction = DirectionalLight["Direction"].as<Vector3>();
  obj->GetDirectionalLight()->Ambient = DirectionalLight["Ambient"].as<Vector3>();
  obj->GetDirectionalLight()->Diffuse = DirectionalLight["Diffuse"].as<Vector3>();
  obj->GetDirectionalLight()->Specular = DirectionalLight["Specular"].as<Vector3>();
  obj->GetDirectionalLight()->Color = DirectionalLight["Color"].as<Vector3>();

  YAML::Node Material = entity["Material"];
  obj->GetMaterial()->Shininess = Material["Shininess"].as<float>();

  YAML::Node SpotLight = entity["SpotLight"];
  obj->GetSpotLight()->Position = SpotLight["Position"].as<Vector3>();
  obj->GetSpotLight()->Direction = SpotLight["Direction"].as<Vector3>();
  obj->GetSpotLight()->CutOff = SpotLight["CutOff"].as<float>();
  obj->GetSpotLight()->OuterCutOff = SpotLight["OuterCutOff"].as<float>();
  obj->GetSpotLight()->Constant = SpotLight["Constant"].as<float>();
  obj->GetSpotLight()->Linear = SpotLight["Linear"].as<float>();
  obj->GetSpotLight()->Quadratic = SpotLight["Quadratic"].as<float>();
  obj->GetSpotLight()->Ambient = SpotLight["Ambient"].as<Vector3>();
  obj->GetSpotLight()->Diffuse = SpotLight["Diffuse"].as<Vector3>();
  obj->GetSpotLight()->Specular = SpotLight["Specular"].as<Vector3>();
  obj->GetSpotLight()->Active = SpotLight["Active"].as<bool>();

  for (const auto& pointLight : entity["ObjectPointLight"]) {
    ObjectPointLight light;
    Transformation trans;
    light.Constant = pointLight["Constant"].as<float>();
    light.Linear = pointLight["Linear"].as<float>();
    light.Quadratic = pointLight["Quadratic"].as<float>();
    light.Ambient = pointLight["Ambient"].as<Vector3>();
    light.Diffuse = pointLight["Diffuse"].as<Vector3>();
    light.Specular = pointLight["Specular"].as<Vector3>();
    light.Active = pointLight["Active"].as<bool>();
    light.Color = pointLight["Color"].as<Vector3>();
    trans.position = pointLight["Position"].as<Vector3>();
    trans.rotation = pointLight["Rotation"].as<Vector3>();
    trans.scale = pointLight["Scale"].as<Vector3>();

    obj->AddObjectPointLight(light, trans);
  }

  scene->GetLightSources()->push_back(obj);
};

void Serialization::ReadEngineConfiguration(const String& path)
{
  YAML::Node node = YAML::LoadFile(path);
  EngineConfigurationHandle* conf = m_Application->GetSettings()->GetEngineConfiguration();
  Yeager::WindowInfo* wnd = m_Application->GetWindow()->GetWindowInformationPtr();

  if (node["YeagerLauncherWindowWidth"]) {
    uint launcherWindowWidth = node["YeagerLauncherWindowWidth"].as<uint>();
    if (launcherWindowWidth < 300) {
      launcherWindowWidth = 300;
    }
    conf->LauncherWindowSize.x = launcherWindowWidth;
    wnd->LauncherSize.x = launcherWindowWidth;
  } else {
    Yeager::LogDebug(WARNING, "YeagerLauncherWindowWidth not found in the configuration file: {}", path);
    conf->LauncherWindowSize.x = 800;
    wnd->LauncherSize.x = 800;
  }

  if (node["YeagerLauncherWindowHeight"]) {
    uint launcherWindowHeight = node["YeagerLauncherWindowHeight"].as<uint>();
    if (launcherWindowHeight < 300) {
      launcherWindowHeight = 300;
    }
    conf->LauncherWindowSize.y = launcherWindowHeight;
    wnd->LauncherSize.y = launcherWindowHeight;
  } else {
    Yeager::LogDebug(WARNING, "YeagerLauncherWindowHeight not found in the configuration file: {}", path);
    conf->LauncherWindowSize.y = 800;
    wnd->LauncherSize.y = 800;
  }

  if (node["YeagerEditorWindowWidth"]) {
    uint editorWindowWidth = node["YeagerEditorWindowWidth"].as<uint>();
    if (editorWindowWidth < 800) {
      editorWindowWidth = 800;
    }
    conf->EditorWindowSize.x = editorWindowWidth;
    wnd->EditorSize.x = editorWindowWidth;
  } else {
    Yeager::LogDebug(WARNING, "YeagerEditorWindowWidth not found in the configuration file: {}", path);
    conf->EditorWindowSize.x = 1920;
    wnd->EditorSize.x = 1920;
  }

  if (node["YeagerEditorWindowHeight"]) {
    uint editorWindowHeight = node["YeagerEditorWindowHeight"].as<uint>();
    if (editorWindowHeight < 800) {
      editorWindowHeight = 800;
    }
    conf->EditorWindowSize.y = editorWindowHeight;
    wnd->EditorSize.y = editorWindowHeight;
  } else {
    Yeager::LogDebug(WARNING, "YeagerEditorWindowHeight not found in the configuration file: {}", path);
    conf->EditorWindowSize.y = 1080;
    wnd->EditorSize.y = 1080;
  }
}
void Serialization::WriteEngineConfiguration(const String& path)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  SerializeObject(out, "YeagerLauncherWindowWidth",
                  m_Application->GetWindow()->GetWindowInformationPtr()->LauncherSize.x);
  SerializeObject(out, "YeagerLauncherWindowHeight",
                  m_Application->GetWindow()->GetWindowInformationPtr()->LauncherSize.y);
  SerializeObject(out, "YeagerEditorWindowWidth", m_Application->GetWindow()->GetWindowInformationPtr()->EditorSize.x);
  SerializeObject(out, "YeagerEditorWindowHeight", m_Application->GetWindow()->GetWindowInformationPtr()->EditorSize.y);
  out << YAML::EndMap;

  std::ofstream of(path);
  if (of.is_open()) {
    of << out.c_str();
  } else {
    Yeager::Log(ERROR, "Cannot open the path to the EngineConfiguration.yml : {}", path);
  }
  of.close();
}

void Serialization::ReadLoadedProjectsHandles(String externalFolder)
{
  YAML::Node node = YAML::LoadFile(externalFolder + YG_PS + "LoadedProjectsPath.yml");
  for (const auto& project : node["ProjectsLoaded"]) {
    LoadedProjectHandle handle;
    handle.m_ProjectName = project["ProjectName"].as<String>();
    handle.m_ProjectFolderPath = project["ProjectFolderPath"].as<String>();
    handle.m_ProjectConfigurationPath = project["ProjectConfigurationPath"].as<String>();
    m_Application->GetLoadedProjectsHandles()->push_back(handle);
  }
}

void Serialization::WriteLoadedProjectsHandles(String externalFolder)
{
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

YgTime_t Serialization::DeserializeProjectTimeOfCreation(YAML::Node node)
{
  YgTime_t time;
  time.Date.Year = node["Year"].as<int>();
  time.Date.Month = node["Month"].as<int>();
  time.Date.Day = node["Day"].as<int>();
  time.Time.Hours = node["Hour"].as<int>();
  time.Time.Minutes = node["Minutes"].as<int>();
  return time;
}

void Serialization::SerializeProjectTimeOfCreation(YAML::Emitter& out, Yeager::Scene* scene, const char* key)
{
  out << YAML::Flow;
  out << YAML::Key << key << YAML::Value << YAML::BeginMap;

  SerializeObject(out, "Year", scene->GetContext()->m_TimeOfCreation.Date.Year);
  SerializeObject(out, "Month", scene->GetContext()->m_TimeOfCreation.Date.Month);
  SerializeObject(out, "Day", scene->GetContext()->m_TimeOfCreation.Date.Day);

  SerializeObject(out, "Hour", scene->GetContext()->m_TimeOfCreation.Time.Hours);
  SerializeObject(out, "Minutes", scene->GetContext()->m_TimeOfCreation.Time.Minutes);

  out << YAML::EndMap;
}
