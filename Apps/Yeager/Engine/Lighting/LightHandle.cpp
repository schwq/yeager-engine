#include "LightHandle.h"
using namespace Yeager;

LightHandle::LightHandle(YgString name, ApplicationCore* app, Shader* shader)
    : GameEntity(name), m_Application(app), m_LinkedShader(shader)
{
  m_PointLights.reserve(MAX_POINT_LIGHTS);
  for (int x = 0; x < MAX_POINT_LIGHTS; x++) {
    shader->UseShader();
    shader->SetBool("pointLights[" + std::to_string(x) + "].Active", false);
  }
}

void LightHandle::BuildShaderProps(YgVector3 viewPos, YgVector3 front, float shininess)
{
  m_LinkedShader->UseShader();
  for (int x = 0; x < m_PointLights.size(); x++) {
    if (x > MAX_POINT_LIGHTS) {
      break;
    }
    YgString point_name = "pointLights[" + std::to_string(x) + "].";
    m_LinkedShader->SetVec3(point_name + "Position", m_PointLights.at(x).Position);
    m_LinkedShader->SetFloat(point_name + "Constant", m_PointLights.at(x).Constant);
    m_LinkedShader->SetFloat(point_name + "Linear", m_PointLights.at(x).Linear);
    m_LinkedShader->SetFloat(point_name + "Quadratic", m_PointLights.at(x).Quadratic);
    m_LinkedShader->SetVec3(point_name + "Ambient", m_PointLights.at(x).Ambient);
    m_LinkedShader->SetVec3(point_name + "Diffuse", m_PointLights.at(x).Diffuse);
    m_LinkedShader->SetVec3(point_name + "Specular", m_PointLights.at(x).Specular);
    m_LinkedShader->SetBool(point_name + "Active", m_PointLights.at(x).Active);
  }

  m_LinkedShader->SetVec3("viewer.Position", viewPos);
  m_LinkedShader->SetFloat("material.Shininess", shininess);

  m_LinkedShader->SetVec3("directionalLight.Direction", m_DirectionalLight.Direction);
  m_LinkedShader->SetVec3("directionalLight.Ambient", m_DirectionalLight.Ambient);
  m_LinkedShader->SetVec3("directionalLight.Diffuse", m_DirectionalLight.Diffuse);
  m_LinkedShader->SetVec3("directionalLight.Specular", m_DirectionalLight.Specular);
  m_LinkedShader->SetVec3("directionalLight.Color", m_DirectionalLight.Color);

  m_LinkedShader->SetVec3("spotLight.Position", viewPos);
  m_LinkedShader->SetVec3("spotLight.Direction", front);
  m_LinkedShader->SetVec3("spotLight.Ambient", spotLight.Ambient);
  m_LinkedShader->SetVec3("spotLight.Diffuse", spotLight.Diffuse);
  m_LinkedShader->SetVec3("spotLight.Specular", spotLight.Specular);
  m_LinkedShader->SetFloat("spotLight.Constant", spotLight.Constant);
  m_LinkedShader->SetFloat("spotLight.Linear", spotLight.Linear);
  m_LinkedShader->SetFloat("spotLight.Quadratic", spotLight.Quadratic);
  m_LinkedShader->SetFloat("spotLight.CutOff", spotLight.CutOff);
  m_LinkedShader->SetFloat("spotLight.OuterCutOff", spotLight.OuterCutOff);
  m_LinkedShader->SetBool("spotLight.Active", spotLight.Active);
}

LightSource::LightSource(YgString name, ApplicationCore* app, Shader* link_shader, Shader* draw_shader)
    : LightHandle(name, app, link_shader), m_DrawableShader(draw_shader)
{
  m_ObjectPointLights.reserve(MAX_POINT_LIGHTS);
}

LightSource::~LightSource()
{
  for (const auto& obj : m_ObjectPointLights) {
    obj.Destroy();
  }
}

void LightSource::DrawLightSources()
{
  m_DrawableShader->UseShader();
  for (auto& obj : m_ObjectPointLights) {
    m_DrawableShader->SetVec3("aColor", obj.Color);
    obj.Position = obj.ObjSource->GetTransformationPtr()->position;
    obj.ObjSource->Draw(m_DrawableShader);
  }
}

void LightSource::BuildShaderProps(YgVector3 viewPos, YgVector3 front, float shininess)
{
  m_LinkedShader->UseShader();

  for (int x = 0; x < m_ObjectPointLights.size(); x++) {
    YgString point_name = "pointLights[" + std::to_string(x) + "].";

    m_LinkedShader->SetVec3(point_name + "Position", m_ObjectPointLights.at(x).Position);
    m_LinkedShader->SetFloat(point_name + "Constant", m_ObjectPointLights.at(x).Constant);
    m_LinkedShader->SetFloat(point_name + "Linear", m_ObjectPointLights.at(x).Linear);
    m_LinkedShader->SetFloat(point_name + "Quadratic", m_ObjectPointLights.at(x).Quadratic);
    m_LinkedShader->SetVec3(point_name + "Ambient",
                            YgVector3(m_ObjectPointLights.at(x).Color.x * m_ObjectPointLights.at(x).Ambient.x,
                                      m_ObjectPointLights.at(x).Color.y * m_ObjectPointLights.at(x).Ambient.y,
                                      m_ObjectPointLights.at(x).Color.z * m_ObjectPointLights.at(x).Ambient.z));
    m_LinkedShader->SetVec3(point_name + "Diffuse", m_ObjectPointLights.at(x).Color);
    m_LinkedShader->SetVec3(point_name + "Specular", m_ObjectPointLights.at(x).Color);
    m_LinkedShader->SetBool(point_name + "Active", m_ObjectPointLights.at(x).Active);
  }

  m_LinkedShader->SetVec3("viewer.Position", viewPos);
  m_LinkedShader->SetFloat("material.Shininess", shininess);

  m_LinkedShader->SetVec3("directionalLight.Direction", m_DirectionalLight.Direction);
  m_LinkedShader->SetVec3("directionalLight.Ambient", m_DirectionalLight.Ambient);
  m_LinkedShader->SetVec3("directionalLight.Diffuse", m_DirectionalLight.Diffuse);
  m_LinkedShader->SetVec3("directionalLight.Specular", m_DirectionalLight.Specular);
  m_LinkedShader->SetVec3("directionalLight.Color", m_DirectionalLight.Color);

  m_LinkedShader->SetVec3("spotLight.Position", viewPos);
  m_LinkedShader->SetVec3("spotLight.Direction", front);
  m_LinkedShader->SetVec3("spotLight.Ambient", spotLight.Ambient);
  m_LinkedShader->SetVec3("spotLight.Diffuse", spotLight.Diffuse);
  m_LinkedShader->SetVec3("spotLight.Specular", spotLight.Specular);
  m_LinkedShader->SetFloat("spotLight.Constant", spotLight.Constant);
  m_LinkedShader->SetFloat("spotLight.Linear", spotLight.Linear);
  m_LinkedShader->SetFloat("spotLight.Quadratic", spotLight.Quadratic);
  m_LinkedShader->SetFloat("spotLight.CutOff", spotLight.CutOff);
  m_LinkedShader->SetFloat("spotLight.OuterCutOff", spotLight.OuterCutOff);
  m_LinkedShader->SetBool("spotLight.Active", spotLight.Active);
}

void LightSource::AddObjectPointLight(const ObjectPointLight& obj, Transformation& trans)
{

  ObjectPointLight pObj = obj;
  pObj.ObjSource = new Object("SpotLight_" + std::to_string(m_ObjectPointLights.size()), m_Application);
  pObj.ObjSource->GenerateObjectGeometry(ObjectGeometryType::ESphere);
  pObj.ObjSource->SetTransformation(trans);
  pObj.Active = true;

  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
    pObj.Destroy();
  } else {
    m_ObjectPointLights.push_back(pObj);
  }
}

void LightSource::AddObjectPointLight(const ObjectPointLight& obj, Yeager::Object& custom_obj)
{
  ObjectPointLight pObj = obj;
  pObj.ObjSource = new Object(custom_obj);
  pObj.ObjSource->GenerateObjectGeometry(custom_obj.GetGeometry());
  pObj.Active = true;

  if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
    Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
    pObj.Destroy();
  } else {
    m_ObjectPointLights.push_back(pObj);
  }
}