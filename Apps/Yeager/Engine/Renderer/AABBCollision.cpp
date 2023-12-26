#include "AABBCollision.h"
#include "Object.h"
using namespace Yeager;

AABBCollision::AABBCollision() {}

void AABBCollision::BuildCollision(Object* obj, std::vector<YgVector3>& vertices)
{
  m_ObjectCollidable = obj;
  std::pair<YgVector3, YgVector3> ext = GetExtents(vertices);
  m_AABB.Min = ext.first;
  m_AABB.Max = ext.second;
  Setup();
}

std::pair<YgVector3, YgVector3> AABBCollision::GetExtents(const std::vector<YgVector3>& vertices)
{
  YgVector3 pMin = YgVector3(0);
  YgVector3 pMax = YgVector3(0);
  for (unsigned int x = 0; x < vertices.size(); x++) {
    YgVector3 pCurrent = vertices[x];
    pMax = glm::max(pCurrent, pMax);
    pMin = glm::min(pCurrent, pMin);
  }

  return std::make_pair(pMin, pMax);
}

bool AABBCollision::CheckCollision(Object* obj1)
{
  YgVector3 aPos = m_ObjectCollidable->GetTransformation().position;
  YgVector3 bPos = obj1->GetTransformation().position;
  const YgVector3 aMax = m_AABB.Max + aPos;
  const YgVector3 aMin = m_AABB.Min + aPos;
  const YgVector3 bMax = obj1->GetCollision()->GetPositions().Max + bPos;
  const YgVector3 bMin = obj1->GetCollision()->GetPositions().Min + bPos;

  bool CollisionX = (aMin.x <= bMax.x && aMax.x >= bMin.x);
  bool CollisionY = (aMin.y <= bMax.y && aMax.y >= bMin.y);
  bool CollisionZ = (aMin.z <= bMax.z && aMax.z >= bMin.z);
  return (CollisionX && CollisionY && CollisionZ);
}

void AABBCollision::Setup()
{

  std::vector<GLfloat> vertices = {
      m_AABB.Min.x, m_AABB.Min.y, m_AABB.Max.z, m_AABB.Max.x, m_AABB.Min.y, m_AABB.Max.z,
      m_AABB.Max.x, m_AABB.Max.y, m_AABB.Max.z, m_AABB.Min.x, m_AABB.Max.y, m_AABB.Max.z,

      m_AABB.Max.x, m_AABB.Min.y, m_AABB.Max.z, m_AABB.Max.x, m_AABB.Min.y, m_AABB.Min.z,
      m_AABB.Max.x, m_AABB.Max.y, m_AABB.Min.z, m_AABB.Max.x, m_AABB.Max.y, m_AABB.Max.z,

      m_AABB.Min.x, m_AABB.Max.y, m_AABB.Max.z, m_AABB.Max.x, m_AABB.Max.y, m_AABB.Max.z,
      m_AABB.Max.x, m_AABB.Max.y, m_AABB.Min.z, m_AABB.Min.x, m_AABB.Max.y, m_AABB.Min.z,

      m_AABB.Min.x, m_AABB.Min.y, m_AABB.Min.z, m_AABB.Min.x, m_AABB.Max.y, m_AABB.Min.z,
      m_AABB.Max.x, m_AABB.Max.y, m_AABB.Min.z, m_AABB.Max.x, m_AABB.Min.y, m_AABB.Min.z,

      m_AABB.Min.x, m_AABB.Min.y, m_AABB.Min.z, m_AABB.Max.x, m_AABB.Min.y, m_AABB.Min.z,
      m_AABB.Max.x, m_AABB.Min.y, m_AABB.Max.z, m_AABB.Min.x, m_AABB.Min.y, m_AABB.Max.z,

      m_AABB.Min.x, m_AABB.Min.y, m_AABB.Min.z, m_AABB.Min.x, m_AABB.Min.y, m_AABB.Max.z,
      m_AABB.Min.x, m_AABB.Max.y, m_AABB.Max.z, m_AABB.Min.x, m_AABB.Max.y, m_AABB.Min.z,
  };
  m_VerticeSize = vertices.size();

  glGenVertexArrays(1, &m_Vao);
  glGenBuffers(1, &m_Vbo);

  glBindVertexArray(m_Vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}
void AABBCollision::Draw(Shader* shader, YgMatrix4& model)
{
  if (m_Enabled) {
    shader->UseShader();
    shader->SetMat4("model", model);

    if (m_IsColliding) {
      shader->SetVec3("Color", YgVector3(1.0f, 0.1f, 0.1f));
    } else {
      shader->SetVec3("Color", YgVector3(1.0f, 1.0f, 1.0f));
    }

    glBindVertexArray(m_Vao);
    glDrawArrays(GL_LINES, 0, m_VerticeSize / 3);
    glBindVertexArray(0);
  }
}