#include "lighting_source.h"

PointLightSource::PointLightSource(Application* app, String name,
                                   PointLight prop, EngineShapeType shape,
                                   Shader* prop_shader, Shader* shape_shader)
    : m_name(name),
      m_shape(shape),
      m_prop_shader(prop_shader),
      m_shape_shader(shape_shader),
      m_prop(prop),
      m_app(app) {
  Setup();
  yeager_shape = new YeagerShape("fuck off", m_app, kDefaultTexturePath,
                                 EngineShapeType::kCube);
  m_toolbox_object = new ToolBoxObject();
  m_toolbox_object->SetType(ExplorerObjectType::kPointLight);
  m_toolbox_object->SetPosition(yeager_shape->Transform()->PGetPosition());
  m_toolbox_object->SetRotation(yeager_shape->Transform()->PGetRotation());
  m_toolbox_object->SetScale(yeager_shape->Transform()->PGetScale());
  m_toolbox_object->SetPointLight(&prop);
  m_app->GetExplorer()->AddObject(
      m_name.c_str(), ExplorerObjectType::kPointLight, m_toolbox_object);
}

PointLightSource::~PointLightSource() {}

void PointLightSource::Setup() {}

void PointLightSource::Draw() {
  yeager_shape->Draw(m_shape_shader);
}

Transformation* PointLightSource::Transform() {
  return yeager_shape->Transform();
}

void PointLightSource::SetPropToShader() {
  String prefix = "lights[" + std::to_string(m_lights_array_pos) + "]";
  m_prop_shader->SetVec3(prefix + ".position", m_prop.m_position);
  m_prop_shader->SetFloat(prefix + ".constant", m_prop.m_constant);
  m_prop_shader->SetFloat(prefix + ".linear", m_prop.m_linear);
  m_prop_shader->SetFloat(prefix + ".quadratic", m_prop.m_quadratic);
  m_prop_shader->SetVec3(prefix + ".ambient", m_prop.m_ambient);
  m_prop_shader->SetVec3(prefix + ".diffuse", m_prop.m_diffuse);
  m_prop_shader->SetVec3(prefix + ".specular", m_prop.m_specular);
  m_prop_shader->SetInt(prefix + ".m_active", m_prop.m_active);
}
