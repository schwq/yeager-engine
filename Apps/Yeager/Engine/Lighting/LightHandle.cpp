#include "LightHandle.h"
using namespace Yeager;

LightHandle::LightHandle(Shader* shader) {
  m_PointLights.reserve(MAX_POINT_LIGHTS);
  for (int x = 0; x < MAX_POINT_LIGHTS; x++) {
    shader->UseShader();
    shader->SetBool("pointLights[" + std::to_string(x) + "].Active", false);
  }
}

void LightHandle::BuildShaderProps(Shader* shader, YgVector3 viewPos, YgVector3 front, float shininess) {
  shader->UseShader();
	for (int x = 0; x < m_PointLights.size(); x++) {
		if (x > MAX_POINT_LIGHTS) {
        break;
		}
        YgString point_name = "pointLights[" + std::to_string(x) + "]."; 
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
 }

 LightSource::LightSource(Shader* shader) : LightHandle(shader) {
    m_ObjectPointLights.reserve(MAX_POINT_LIGHTS);
 }

 LightSource::~LightSource() {
    for (const auto& obj : m_ObjectPointLights) {
        obj.Destroy();
    }
  }


 void LightSource::DrawLightSources(Shader* shader) {
    shader->UseShader();
     for (auto& obj : m_ObjectPointLights) {
        shader->SetVec3("aColor", YgVector3(1.0f));
        obj.Position = obj.ObjSource->GetTransformationPtr()->position;
        obj.ObjSource->Draw(shader);
    }
  }

 void LightSource::BuildShaderProps(Shader* shader, YgVector3 viewPos, YgVector3 front, float shininess) {
    shader->UseShader();
    
    for (int x = 0; x < m_ObjectPointLights.size(); x++) {
        YgString point_name = "pointLights[" + std::to_string(x) + "].";
        shader->SetVec3(point_name + "Position", m_ObjectPointLights.at(x).Position);
        shader->SetFloat(point_name + "Constant", m_ObjectPointLights.at(x).Constant);
        shader->SetFloat(point_name + "Linear", m_ObjectPointLights.at(x).Linear);
        shader->SetFloat(point_name + "Quadratic", m_ObjectPointLights.at(x).Quadratic);
        shader->SetVec3(point_name + "Ambient", m_ObjectPointLights.at(x).Ambient);
        shader->SetVec3(point_name + "Diffuse", m_ObjectPointLights.at(x).Diffuse);
        shader->SetVec3(point_name + "Specular", m_ObjectPointLights.at(x).Specular);
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
 }

 void LightSource::AddSpotLight(const ObjectPointLight& obj, ApplicationCore* app)
 {
    
    ObjectPointLight pObj = obj;
    pObj.ObjSource = new Object("SpotLight_" + std::to_string(m_ObjectPointLights.size()), app);
    pObj.ObjSource->GenerateObjectGeometry(ObjectGeometryType::ESphere);
    pObj.Active = true;
    
    if (m_ObjectPointLights.size() >= MAX_POINT_LIGHTS) {
        Yeager::Log(WARNING, "Trying to add a object spot light! The maximum have already been reach!");
        pObj.Destroy();
    } else {
        m_ObjectPointLights.push_back(pObj);
    }
}