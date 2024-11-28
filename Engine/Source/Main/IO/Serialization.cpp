#include "Serialization.h"
#include "Components/Renderer/Skybox/Skybox.h"
#include "Main/Core/Application.h"
#include "Main/Scene/Scene.h"
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
          YAML::Node iproj = YAML::LoadFile(project["ProjectConfigurationPath"].as<String>());
          disp.Path = project["ProjectConfigurationPath"].as<String>();
          if (iproj["Renderer"]) {
            disp.RendererType = iproj["Renderer"].as<String>();
          }
          if (iproj["SceneType"]) {
            disp.SceneType = iproj["SceneType"].as<String>();
          }
          if (iproj["Author"]) {
            disp.Author = iproj["Author"].as<String>();
          }
          if (iproj["TimeOfCreation"]) {
            disp.TimeOfCreation = app->GetSerial()->DeserializeProjectTimeOfCreation(iproj["TimeOfCreation"]);
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

std::vector<Yeager::TemplateHandle> Serialization::FindTemplatesFromSharedFolder(const std::filesystem::path& path)
{
  if (!std::filesystem::is_directory(path))
    return std::vector<Yeager::TemplateHandle>();

  std::vector<Yeager::TemplateHandle> templates;

  for (const auto& dir : std::filesystem::directory_iterator{path}) {
    const String p = dir.path().string() + YG_PS + "Template.yml";
    if (std::filesystem::is_directory(dir) && Yeager::ValidatesPath(p)) {
      templates.push_back(ReadTemplateConfiguration(p, dir));
    }
  }
  return templates;
}

Yeager::TemplateHandle Serialization::ReadTemplateConfiguration(const std::filesystem::path& path,
                                                                const std::filesystem::path& folder)
{
  TemplateHandle handle;
  handle.FolderPath = folder;
  handle.TemplateConfigurationPath = path;

  YAML::Node node = YAML::LoadFile(path.string());

  if (node["TemplateName"])
    handle.Name = node["TemplateName"].as<String>();

  if (node["TemplateDescription"])
    handle.Description = node["TemplateDescription"].as<String>();

  if (node["TemplateAssetsConfigurationPath"]) {
    String p = node["TemplateAssetsConfigurationPath"].as<String>();
    if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING)
      std::replace(p.begin(), p.end(), '/', '\\');
    handle.AssetsConfigurationPath = std::filesystem::path(folder.string() + YG_PS + p);
  }
  return handle;
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

void YEAGER_FORCE_INLINE Serialization::SerializeBasicEntity(YAML::Emitter& out, String name, uuids::uuid uuid,
                                                             String type)
{
  out << YAML::Key << "Entity" << YAML::Value << uuids::to_string(uuid);
  out << YAML::Key << "Name" << YAML::Value << name;
  out << YAML::Key << "Type" << YAML::Value << type;
}

template <typename Type>
std::optional<Type> Serialization::DeserializeObject(const YAML::Node& node, Cchar key)
{
  if (node[key])
    return node[key].as<Type>();
  return std::nullopt;
}

void YEAGER_FORCE_INLINE Serialization::SerializeObjectTransformation(YAML::Emitter& out, String name,
                                                                      Yeager::Transformation3D& transf) noexcept
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
  SerializeObject(out, "Scene", scene->GetContext()->Name);
  SerializeProjectTimeOfCreation(out, scene, "TimeOfCreation");
  SerializeObject(out, "Author", scene->GetContext()->ProjectAuthor);
  SerializeObject(out, "Renderer", SceneRendererToString(scene->GetContext()->ProjectSceneRenderer));
  SerializeObject(out, "SceneType", SceneTypeToString(scene->GetContext()->ProjectSceneType));
  SerializeObject(out, "Camera Position", m_Application->GetCamera()->GetPosition());
  SerializeObject(out, "Camera Direction", m_Application->GetCamera()->GetFront());
  SerializeBegin(out, "SceneEntities", YAML::BeginSeq);

  if (scene->GetSkybox()->CanBeSerialize() && scene->GetSkybox()->IsLoaded()) {
    out << YAML::BeginMap;
    SerializeBasicEntity(out, scene->GetSkybox()->GetName(), scene->GetSkybox()->GetEntityUUID(), "Skybox");
    SerializeObject(out, "Path", scene->GetSkybox()->GetPath());
    SerializeObject(out, "FromTemplate", false);
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetAudios()->size(); x++) {
    AudioHandle* audio = scene->GetAudios()->at(x).get();
    if (!audio->CanBeSerialize())
      continue;
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetEntityUUID(), "AudioHandle");
    SerializeObject(out, "Path", audio->GetPath());
    out << YAML::EndMap;
  }

  for (Uint x = 0; x < scene->GetAudios3D()->size(); x++) {
    Audio3DHandle* audio = scene->GetAudios3D()->at(x).get();
    if (!audio->CanBeSerialize())
      continue;
    out << YAML::BeginMap;
    SerializeBasicEntity(out, audio->GetName(), audio->GetEntityUUID(), "Audio3DHandle");
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

    SerializeBasicEntity(out, obj->GetName(), obj->GetEntityUUID(), "Object");
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

    SerializeBasicEntity(out, obj->GetName(), obj->GetEntityUUID(), "AnimatedObject");
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

    SerializeBasicEntity(out, obj->GetName(), obj->GetEntityUUID(), "LightSource");
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
      SerializeBasicEntity(out, light.ObjSource->GetName(), light.ObjSource->GetEntityUUID(), "ObjectPointLight");
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

  Yeager::CreateFileAndWrites(path.c_str(), out.c_str());
}

template <typename T>
bool Serialization::DeserializeIfExistsIntoRef(const YAML::Node& node, const String& key, T& hold)
{
  try {
    if (node[key.c_str()]) {
      hold = node[key.c_str()].as<T>();
      return true;
    } else {
      Yeager::Log(WARNING, "Key {} does not exists in the node! Cannot deserialize!", key);
    }
  } catch (const YAML::Exception& exc) {
    Yeager::Log(ERROR, "YAML exception raised! What: {}", exc.what());
  }
  return false;
}

ColorschemeConfig Serialization::ReadColorschemeConfig()
{
  ColorschemeConfig colorscheme;
  YAML::Node node = YAML::LoadFile(GetPathFromShared("/Configuration/Theme/Colorscheme/dark_mode.yaml").value());

  DeserializeIfExistsIntoRef(node, "WindowRounding", colorscheme.WindowRounding);
  DeserializeIfExistsIntoRef(node, "FrameRounding", colorscheme.FrameRounding);
  DeserializeIfExistsIntoRef(node, "ScrollbarRounding", colorscheme.ScrollbarRounding);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_Text", colorscheme.Colors_ImGuiCol_Text);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_TextDisabled", colorscheme.Colors_ImGuiCol_TextDisabled);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_WindowBg", colorscheme.Colors_ImGuiCol_WindowBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ChildBg", colorscheme.Colors_ImGuiCol_ChildBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_PopupBg", colorscheme.Colors_ImGuiCol_PopupBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_Border", colorscheme.Colors_ImGuiCol_Border);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_BorderShadow", colorscheme.Colors_ImGuiCol_BorderShadow);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_FrameBg", colorscheme.Colors_ImGuiCol_FrameBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_FrameBgHovered", colorscheme.Colors_ImGuiCol_FrameBgHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_FrameBgActive", colorscheme.Colors_ImGuiCol_FrameBgActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_TitleBg", colorscheme.Colors_ImGuiCol_TitleBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_TitleBgCollapsed", colorscheme.Colors_ImGuiCol_TitleBgCollapsed);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_TitleBgActive", colorscheme.Colors_ImGuiCol_TitleBgActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_MenuBarBg", colorscheme.Colors_ImGuiCol_MenuBarBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ScrollbarBg", colorscheme.Colors_ImGuiCol_ScrollbarBg);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ScrollbarGrab", colorscheme.Colors_ImGuiCol_ScrollbarGrab);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ScrollbarGrabHovered",
                             colorscheme.Colors_ImGuiCol_ScrollbarGrabHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ScrollbarGrabActive",
                             colorscheme.Colors_ImGuiCol_ScrollbarGrabActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_CheckMark", colorscheme.Colors_ImGuiCol_CheckMark);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_SliderGrab", colorscheme.Colors_ImGuiCol_SliderGrab);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_SliderGrabActive", colorscheme.Colors_ImGuiCol_SliderGrabActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_Button", colorscheme.Colors_ImGuiCol_Button);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ButtonHovered", colorscheme.Colors_ImGuiCol_ButtonHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ButtonActive", colorscheme.Colors_ImGuiCol_ButtonActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_Header", colorscheme.Colors_ImGuiCol_Header);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_HeaderHovered", colorscheme.Colors_ImGuiCol_HeaderHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_HeaderActive", colorscheme.Colors_ImGuiCol_HeaderActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ResizeGrip", colorscheme.Colors_ImGuiCol_ResizeGrip);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ResizeGripHovered", colorscheme.Colors_ImGuiCol_ResizeGripHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_ResizeGripActive", colorscheme.Colors_ImGuiCol_ResizeGripActive);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_PlotLines", colorscheme.Colors_ImGuiCol_PlotLines);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_PlotLinesHovered", colorscheme.Colors_ImGuiCol_PlotLinesHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_PlotHistogram", colorscheme.Colors_ImGuiCol_PlotHistogram);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_PlotHistogramHovered",
                             colorscheme.Colors_ImGuiCol_PlotHistogramHovered);
  DeserializeIfExistsIntoRef(node, "Colors_ImGuiCol_TextSelectedBg", colorscheme.Colors_ImGuiCol_TextSelectedBg);

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
        auto ps_shader = std::make_shared<Yeager::Shader>(GetPathFromShared(fragment).value().c_str(),
                                                          GetPathFromShared(vertex).value().c_str(), name);
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

bool Serialization::ReadEditorSoundsConfiguration(const String& path)
{
  YAML::Node node = YAML::LoadFile(path);
  auto sounds = node["Sounds"];
  if (sounds) {

    for (auto s : sounds) {
      String name = s["Name"].as<String>();
      String filename = s["Filename"].as<String>();

      const String path = Yeager::GetPathFromShared("/Resources/Sound/" + filename).value();

      EngineSoundHandle handle(name, path);
      if (m_Application->GetAudioFromEngine()->AddSound(handle)) {
        Yeager::LogDebug(INFO, "Loaded audio to engine, Name {}, Path {}", name, path);
      }
    }
    return true;

  } else {
    return false;
  }
}

void Serialization::DeserializeTemplateAssetsIntoScene(Yeager::Scene* scene, const std::filesystem::path& path)
{
  YAML::Node node = YAML::LoadFile(path.string());
  if (node["TemplateEntities"]) {
    auto entities = node["TemplateEntities"];
    for (auto entity : entities) {
      try {
        DeserializeEntity(scene, node, entity);
      } catch (YAML::BadConversion& exc) {
        Yeager::Log(
            ERROR, "Exception: YAML::BadConversion, Something went wrong reading the template assets configuration! {}",
            exc.what());
      } catch (YAML::BadSubscript& exc) {
        Yeager::Log(ERROR,
                    "Exception: YAML::BadSubscript, Something went wrong reading the template assets configuration! {}",
                    exc.what());
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
      scene->GetContext()->Name = node["Scene"].as<String>();
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
      scene->GetContext()->TimeOfCreation = DeserializeProjectTimeOfCreation(node["TimeOfCreation"]);
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

YEAGER_FORCE_INLINE std::vector<std::shared_ptr<Transformation3D>> Serialization::DeserializeObjectProperties(
    YAML::detail::iterator_value& entity)
{
  std::vector<std::shared_ptr<Transformation3D>> tr;
  YAML::Node props = entity["Props"];
  for (const auto& propertie : props) {
    auto trans = std::make_shared<Transformation3D>(Vector3(0.0f), Vector3(0.0f), Vector3(0.0f));
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

  auto uuid = uuids::uuid::from_string(entity["Entity"].as<String>());
  String name = entity["Name"].as<String>();
  String type = entity["Type"].as<String>();

  if (!uuid.has_value()) {
    Yeager::Log(WARNING, "UUID from entity {} is not valid!", name);
    uuid = uuids::uuid();
  }

  switch (StringToInterger(type.c_str())) {

    case StringToInterger("AudioHandle"): {
      DeserializeAudioHandle(entity, scene, name, type, uuid.value());
    } break;

    case StringToInterger("Audio3DHandle"): {
      DeserializeAudio3DHandle(entity, scene, name, type, uuid.value());
    } break;

    case StringToInterger("Object"): {
      DeserializeObject(entity, scene, name, type, uuid.value());
    } break;

    case StringToInterger("AnimatedObject"): {
      DeserializeAnimatedObject(entity, scene, name, type, uuid.value());
    } break;

    case StringToInterger("LightSource"): {
      DeserializeLightSource(entity, scene, name, type, uuid.value());
    } break;

    case StringToInterger("Skybox"): {
      DeserializeSkybox(entity, scene, name, type, uuid.value());
    } break;

    default:
      Yeager::Log(WARNING, "Entity Type cannot been found! Value [{}]", type);
  }
}

void Serialization::DeserializeSkybox(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                                      const String& type, const uuids::uuid uuid)
{

  bool fromTemplate = entity["FromTemplate"].as<bool>();
  String path;

  if (fromTemplate)
    path += scene->GetTemplateHandle().FolderPath.string();

  String p = entity["Path"].as<String>();
  if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING)
    std::replace(p.begin(), p.end(), '/', '\\');
  path += p;

  auto skybox = std::make_shared<Skybox>(EntityBuilder(m_Application, name, EntityObjectType::SKYBOX, uuid),
                                         ObjectGeometryType::eCUSTOM);
  skybox->BuildSkyboxFromImport(path, true);
  scene->SetSkybox(skybox);
}

void Serialization::DeserializeAudioHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                           const String& name, const String& type, const uuids::uuid uuid)
{
  String path = entity["Path"].as<String>();
  auto audio =
      std::make_shared<Yeager::AudioHandle>(EntityBuilder(m_Application, name, EntityObjectType::AUDIO_HANDLE, uuid),
                                            path, m_Application->GetAudioEngineHandle(), false);
  scene->GetAudios()->push_back(audio);
}

void Serialization::DeserializeAudio3DHandle(YAML::detail::iterator_value& entity, Yeager::Scene* scene,
                                             const String& name, const String& type, const uuids::uuid uuid)
{
  String path = entity["Path"].as<String>();
  Vector3 position = entity["Position"].as<Vector3>();
  auto audio = std::make_shared<Yeager::Audio3DHandle>(
      EntityBuilder(m_Application, name, EntityObjectType::AUDIO_3D_HANDLE, uuid), path,
      m_Application->GetAudioEngineHandle(), false, GLMVec3ToVec3df(position));
  scene->GetAudios3D()->push_back(audio);
};

void Serialization::DeserializeObject(YAML::detail::iterator_value& entity, Yeager::Scene* scene, const String& name,
                                      const String& type, const uuids::uuid uuid)
{
  std::shared_ptr<Yeager::Object> obj;

  String instancedType = entity["InstancedType"].as<String>();
  if (instancedType == "Instanced") {
    obj = std::make_shared<Yeager::Object>(EntityBuilder(m_Application, name, EntityObjectType::OBJECT_INSTANCED, uuid),
                                           entity["InstancedCount"].as<int>());
    std::vector<std::shared_ptr<Transformation3D>> positions = DeserializeObjectProperties(entity);
    obj->BuildProps(positions, m_Application->ShaderFromVarName("SimpleInstanced"));
  } else {
    obj = std::make_shared<Yeager::Object>(EntityBuilder(m_Application, name, EntityObjectType::OBJECT, uuid));
  }

  // Gets transformation, and geometry of the object serialized
  ObjectGeometryType::Enum geometry = DeserializeBasicObject(obj.get(), entity);
  ObjectCreationConfiguration configuration = ObjectCreationConfiguration();
  bool succceded = true;
  bool flip = false;

  const auto textures = entity["TexturesLoaded"];
  for (auto texture : textures) {
    String path = texture["Path"].as<String>();
    flip = texture["Flip"].as<bool>();
  }

  if (geometry == ObjectGeometryType::eCUSTOM) {
    // Import Custom model object
    String path = entity["Path"].as<String>().c_str();
    bool val = Yeager::ValidatesPath(path, false);
    if (!val) {
      Yeager::LogDebug(ERROR, "Cannot validade path for serialization {}", entity["Path"].as<String>().c_str());
    }

    if (!obj->ThreadImportObjectFromFile(entity["Path"].as<String>().c_str(), configuration, flip)) {
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
                                              const String& name, const String& type, const uuids::uuid uuid)
{
  std::shared_ptr<Yeager::AnimatedObject> obj;
  String instancedType = entity["InstancedType"].as<String>();
  if (instancedType == "Instanced") {
    obj = std::make_shared<Yeager::AnimatedObject>(
        EntityBuilder(m_Application, name, EntityObjectType::OBJECT_INSTANCED_ANIMATED, uuid),
        entity["InstancedCount"].as<int>());
    std::vector<std::shared_ptr<Transformation3D>> positions = DeserializeObjectProperties(entity);
    obj->BuildProps(positions, m_Application->ShaderFromVarName("SimpleInstanced"));
  } else {
    obj = std::make_shared<Yeager::AnimatedObject>(
        EntityBuilder(m_Application, name, EntityObjectType::OBJECT_ANIMATED, uuid));
  }

  ObjectGeometryType::Enum geometry = DeserializeBasicObject(obj.get(), entity);
  ObjectCreationConfiguration configuration = ObjectCreationConfiguration();
  bool succceded = true;

  if (geometry == ObjectGeometryType::eCUSTOM) {
    if (!Yeager::ValidatesPath(entity["Path"].as<String>().c_str(), false) ||
        !obj->ThreadImportObjectFromFile(entity["Path"].as<String>().c_str(), configuration, false)) {
      Yeager::Log(ERROR, "Error importing animated object from file during deserialization!");
      return;
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
                                           const String& name, const String& type, const uuids::uuid uuid)
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
  auto obj = std::make_shared<Yeager::PhysicalLightHandle>(
      EntityBuilder(m_Application, name, EntityObjectType::LIGHT_HANDLE, uuid), link_shader,
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
    Transformation3D trans;
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
    Uint launcherWindowWidth = node["YeagerLauncherWindowWidth"].as<Uint>();
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
    Uint launcherWindowHeight = node["YeagerLauncherWindowHeight"].as<Uint>();
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
    Uint editorWindowWidth = node["YeagerEditorWindowWidth"].as<Uint>();
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
    Uint editorWindowHeight = node["YeagerEditorWindowHeight"].as<Uint>();
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

  // TODO: Security measures here to avoid the window position to be out of the montior borders !!
  if (node["YeagerWindowPositionWidth"]) {
    Uint windowPositionWidth = node["YeagerWindowPositionWidth"].as<Uint>();
    wnd->WindowPosition.x = windowPositionWidth;
  }

  if (node["YeagerWindowPositionHeight"]) {
    Uint windowPositionHeight = node["YeagerWindowPositionHeight"].as<Uint>();
    wnd->WindowPosition.y = windowPositionHeight;
  }
}

void Serialization::WriteEngineConfiguration(const String& path)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  WindowInfo* info = m_Application->GetWindow()->GetWindowInformationPtr();
  SerializeObject(out, "YeagerLauncherWindowWidth", info->LauncherSize.x);
  SerializeObject(out, "YeagerLauncherWindowHeight", info->LauncherSize.y);
  SerializeObject(out, "YeagerEditorWindowWidth", info->EditorSize.x);
  SerializeObject(out, "YeagerEditorWindowHeight", info->EditorSize.y);

  if (info->WindowPosition.x < 0)
    info->WindowPosition.x = -info->WindowPosition.x;

  if (info->WindowPosition.y < 0)
    info->WindowPosition.y = -info->WindowPosition.y;

  SerializeObject(out, "YeagerWindowPositionWidth", info->WindowPosition.x);

  SerializeObject(out, "YeagerWindowPositionHeight", info->WindowPosition.y);
  out << YAML::EndMap;

  Yeager::CreateFileAndWrites(path, out.c_str());
}

void Serialization::ReadLoadedProjectsHandles(String externalFolder)
{
  YAML::Node node = YAML::LoadFile(externalFolder + YG_PS + "LoadedProjectsPath.yml");
  for (const auto& project : node["ProjectsLoaded"]) {
    LoadedProjectHandle handle;
    handle.ProjectName = project["ProjectName"].as<String>();
    handle.ProjectFolderPath = project["ProjectFolderPath"].as<String>();
    handle.ProjectConfigurationPath = project["ProjectConfigurationPath"].as<String>();
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
    out << YAML::Key << "ProjectName" << YAML::Value << project.ProjectName;
    out << YAML::Key << "ProjectFolderPath" << YAML::Value << project.ProjectFolderPath;
    out << YAML::Key << "ProjectConfigurationPath" << YAML::Value << project.ProjectConfigurationPath;
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  Yeager::CreateFileAndWrites(externalFolder + YG_PS + "LoadedProjectsPath.yml", out.c_str());
}

TimePointType Serialization::DeserializeProjectTimeOfCreation(YAML::Node node)
{
  TimePointType time;
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

  SerializeObject(out, "Year", scene->GetContext()->TimeOfCreation.Date.Year);
  SerializeObject(out, "Month", scene->GetContext()->TimeOfCreation.Date.Month);
  SerializeObject(out, "Day", scene->GetContext()->TimeOfCreation.Date.Day);

  SerializeObject(out, "Hour", scene->GetContext()->TimeOfCreation.Time.Hours);
  SerializeObject(out, "Minutes", scene->GetContext()->TimeOfCreation.Time.Minutes);

  out << YAML::EndMap;
}

std::optional<Serialization::LocaleData> Serialization::DeserializeLocaleData(const std::filesystem::path& path)
{
  YAML::Node node;
  Serialization::LocaleData data;
  try {
    node = YAML::LoadFile(path);

  } catch (const std::exception& exc) {
    Yeager::Log(ERROR, "Cannot deserialize locale data! Path {}, What {}", path.string(), exc.what());
    return std::nullopt;
  }

  for (YAML::iterator it = node.begin(); it != node.end(); ++it) {
    String first, second;
    first = it->first.as<String>();
    second = it->second.as<String>();
    data[first] = second;
  }
  return data;
}
