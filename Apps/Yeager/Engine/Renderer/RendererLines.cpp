#include "RendererLines.h"
#include "../../Application.h"
#include "ShaderHandle.h"
using namespace Yeager;

Line::Line(const Vector3& p1, const Vector3& p2, const Vector3& position, const Vector3& rotation, const Vector3& color)
    : m_P1(p1), m_P2(p2), m_Color(color)
{
  m_Transformation.position = position;

  m_Transformation.rotation.x = rotation.x * 360;
  m_Transformation.rotation.y = rotation.y * 360;
  m_Transformation.rotation.z = rotation.z * 360;

  Setup();
}

Line::~Line() {}

void Line::Setup()
{
  m_Data.Vertex1 = m_P1;
  m_Data.Vertex2 = m_P2;
  glGenVertexArrays(1, &m_Vao);
  glGenBuffers(1, &m_Vbo);

  glBindVertexArray(m_Vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(LineData), &m_Data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineData), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  m_Created = true;
}

void Line::Draw(Shader* shader)
{
  if (m_Created && m_Render) {
    shader->UseShader();
    glBindVertexArray(m_Vao);

    shader->SetVec3("Color", m_Color);
    ApplyTransformation(&m_Transformation);

    shader->SetMat4("model", m_Transformation.model);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
  }
}

void Line::Delete()
{
  glDeleteBuffers(1, &m_Vbo);
  glDeleteVertexArrays(1, &m_Vao);
}

RendererLines::RendererLines(Yeager::ApplicationCore* application) : m_Application(application)
{
  Yeager::LogDebug(INFO, "Lines Renderer created!");
}

RendererLines::~RendererLines()
{
  Yeager::LogDebug(INFO, "Lines Renderer deleted!");
  for (auto& line : m_Lines)
    line.Delete();
}

void RendererLines::AddLine(const Yeager::Line& line)
{
  if (m_Lines.size() <= YEAGER_MAX_DRAWN_LINES)
    m_Lines.push_back(line);
}

void RendererLines::AddLine(const Vector3& p1, const Vector3& p2, const Vector3& pos, const Vector3& rotation,
                            const Vector3& color)
{
  if (m_Lines.size() <= YEAGER_MAX_DRAWN_LINES)
    m_Lines.push_back(Line(p1, p2, pos, rotation, color));
}

void RendererLines::Draw(Shader* shader)
{
  for (auto& line : m_Lines)
    line.Draw(shader);
}
