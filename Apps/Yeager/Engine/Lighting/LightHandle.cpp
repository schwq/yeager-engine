#include "LightHandle.h"
using namespace Yeager;

LightBaseHandle::LightBaseHandle(String name, ApplicationCore* app, std::vector<Shader*> link_shaders)
    : EditorEntity(EntityObjectType::LIGHT_HANDLE, app, name), m_LinkedShader(link_shaders)
{
  m_PointLights.reserve(MAX_POINT_LIGHTS);
  for (int x = 0; x < MAX_POINT_LIGHTS; x++) {
    for (auto& shader : link_shaders) {
      shader->UseShader();
      shader->SetBool("pointLights[" + std::to_string(x) + "].Active", false);
    }
  }
}

void LightBaseHandle::BuildShaderProps(Vector3 viewPos, Vector3 front, float shininess)
{
  for (auto& shader : m_LinkedShader) {
    shader->UseShader();
    for (int x = 0; x < m_PointLights.size(); x++) {
      if (x > MAX_POINT_LIGHTS) {
        break;
      }
      String point_name = "pointLights[" + std::to_string(x) + "].";
      shader->SetVec3(point_name + "Position", m_PointLights.at(x).Position);
      shader->SetFloat(point_name + "Constant", m_PointLights.at(x).Constant);
      shader->SetFloat(point_name + "Linear", m_PointLights.at(x).Linear);
      shader->SetFloat(point_name + "Quadratic", m_PointLights.at(x).Quadratic);
      shader->SetVec3(point_name + "Ambient", m_PointLights.at(x).Ambient);
      shader->SetVec3(point_name + "Diffuse", m_PointLights.at(x).Diffuse);
      shader->SetVec3(point_name + "Specular", m_PointLights.at(x).Specular);
      shader->SetBool(point_name + "Active", m_PointLights.at(x).Active);
    }

    shader->SetVec3("viewer.Position", viewPos);
    shader->SetFloat("material.Shininess", shininess);

    shader->SetVec3("directionalLight.Direction", m_DirectionalLight.Direction);
    shader->SetVec3("directionalLight.Ambient", m_DirectionalLight.Ambient);
    shader->SetVec3("directionalLight.Diffuse", m_DirectionalLight.Diffuse);
    shader->SetVec3("directionalLight.Specular", m_DirectionalLight.Specular);
    shader->SetVec3("directionalLight.Color", m_DirectionalLight.Color);

    shader->SetVec3("spotLight.Position", viewPos);
    shader->SetVec3("spotLight.Direction", front);
    shader->SetVec3("spotLight.Ambient", spotLight.Ambient);
    shader->SetVec3("spotLight.Diffuse", spotLight.Diffuse);
    shader->SetVec3("spotLight.Specular", spotLight.Specular);
    shader->SetFloat("spotLight.Constant", spotLight.Constant);
    shader->SetFloat("spotLight.Linear", spotLight.Linear);
    shader->SetFloat("spotLight.Quadratic", spotLight.Quadratic);
    shader->SetFloat("spotLight.CutOff", spotLight.CutOff);
    shader->SetFloat("spotLight.OuterCutOff", spotLight.OuterCutOff);
    shader->SetBool("spotLight.Active", spotLight.Active);
  }
}
PhysicalLightHandle::PhysicalLightHandle(String name, ApplicationCore* app, std::vector<Shader*> link_shader,
                                         Shader* draw_shader)
    : LightBaseHandle(name, app, link_shader), m_DrawableShader(draw_shader)
{
  m_ObjectPointLights.reserve(MAX_POINT_LIGHTS);
}

PhysicalLightHandle::~PhysicalLightHandle()
{
  for (const auto& obj : m_ObjectPointLights) {
    obj.Destroy();
  }
  m_ObjectPointLights.clear();
}

void PhysicalLightHandle::DrawLightSources(float delta)
{
  m_DrawableShader->UseShader();
  for (auto& obj : m_ObjectPointLights) {
    m_DrawableShader->SetVec3("aColor", obj.Color);
    obj.Position = obj.ObjSource->GetTransformationPtr()->position;
    obj.ObjSource->Draw(m_DrawableShader, delta);
  }
}

void PhysicalLightHandle::BuildShaderProps(Vector3 viewPos, Vector3 front, float shininess)
{
  for (auto& shader : m_LinkedShader) {
    shader->UseShader();

    for (int x = 0; x < m_ObjectPointLights.size(); x++) {
      String point_name = "pointLights[" + std::to_string(x) + "].";

      shader->SetVec3(point_name + "Position", m_ObjectPointLights.at(x).Position);
      shader->SetFloat(point_name + "Constant", m_ObjectPointLights.at(x).Constant);
      shader->SetFloat(point_name + "Linear", m_ObjectPointLights.at(x).Linear);
      shader->SetFloat(point_name + "Quadratic", m_ObjectPointLights.at(x).Quadratic);
      shader->SetVec3(point_name + "Ambient",
                      Vector3(m_ObjectPointLights.at(x).Color.x * m_ObjectPointLights.at(x).Ambient.x,
                              m_ObjectPointLights.at(x).Color.y * m_ObjectPointLights.at(x).Ambient.y,
                              m_ObjectPointLights.at(x).Color.z * m_ObjectPointLights.at(x).Ambient.z));
      shader->SetVec3(point_name + "Diffuse", m_ObjectPointLights.at(x).Color);
      shader->SetVec3(point_name + "Specular", m_ObjectPointLights.at(x).Color);
      shader->SetBool(point_name + "Active", m_ObjectPointLights.at(x).Active);
    }

    shader->SetVec3("viewer.Position", viewPos);
    shader->SetFloat("material.Shininess", shininess);

    shader->SetVec3("directionalLight.Direction", m_DirectionalLight.Direction);
    shader->SetVec3("directionalLight.Ambient", m_DirectionalLight.Ambient);
    shader->SetVec3("directionalLight.Diffuse", m_DirectionalLight.Diffuse);
    shader->SetVec3("directionalLight.Specular", m_DirectionalLight.Specular);
    shader->SetVec3("directionalLight.Color", m_DirectionalLight.Color);

    shader->SetVec3("spotLight.Position", viewPos);
    shader->SetVec3("spotLight.Direction", front);
    shader->SetVec3("spotLight.Ambient", spotLight.Ambient);
    shader->SetVec3("spotLight.Diffuse", spotLight.Diffuse);
    shader->SetVec3("spotLight.Specular", spotLight.Specular);
    shader->SetFloat("spotLight.Constant", spotLight.Constant);
    shader->SetFloat("spotLight.Linear", spotLight.Linear);
    shader->SetFloat("spotLight.Quadratic", spotLight.Quadratic);
    shader->SetFloat("spotLight.CutOff", spotLight.CutOff);
    shader->SetFloat("spotLight.OuterCutOff", spotLight.OuterCutOff);
    shader->SetBool("spotLight.Active", spotLight.Active);
  }
}

void PhysicalLightHandle::AddObjectPointLight(const ObjectPointLight& obj, Transformation& trans)
{

  ObjectPointLight pObj = obj;
  pObj.ObjSource = new Object("SpotLight_" + std::to_string(m_ObjectPointLights.size()), m_Application);
  pObj.ObjSource->GenerateObjectGeometry(ObjectGeometryType::eSPHERE,
                                         ObjectPhysXCreationStatic(Vector3(0.0f), Vector3(0.0f), Vector3(0.1f)));
  pObj.ObjSource->SetTransformation(trans);
  pObj.Active = true;

  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
    pObj.Destroy();
  } else {
    m_ObjectPointLights.push_back(pObj);
  }
}

void PhysicalLightHandle::AddObjectPointLight(const ObjectPointLight& obj)
{
  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
    obj.Destroy();
  } else {
    obj.ObjSource->GetTransformationPtr()->scale = Vector3(0.1f);
    m_ObjectPointLights.push_back(obj);
  }
}

void PhysicalLightHandle::AddObjectPointLight(const ObjectPointLight& obj, Yeager::Object& custom_obj)
{
  ObjectPointLight pObj = obj;
  pObj.ObjSource = new Object(custom_obj);
  pObj.ObjSource->GenerateObjectGeometry(custom_obj.GetGeometry(),
                                         ObjectPhysXCreationStatic(Vector3(0.0f), Vector3(0.0f), Vector3(0.1f)));
  pObj.Active = true;

  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
    pObj.Destroy();
  } else {
    m_ObjectPointLights.push_back(pObj);
  }
}

void PhysicalLightHandle::AddObjectPointLight(ObjectPointLight* light, ObjectGeometryType::Enum type)
{
  light->ObjSource = new Object("Light Point", m_Application);
  light->ObjSource->GenerateObjectGeometry(type,
                                           ObjectPhysXCreationStatic(Vector3(0.0f), Vector3(0.0f), Vector3(0.1f)));
  light->Active = true;

  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
  } else {
    light->ObjSource->GetTransformationPtr()->scale = Vector3(0.1f);
    m_ObjectPointLights.push_back(*light);
  }
}
