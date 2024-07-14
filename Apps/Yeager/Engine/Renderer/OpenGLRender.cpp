#include "OpenGLRender.h"
using namespace Yeager;

void SimpleRenderer::GenBuffers()
{
  if (!m_Generated) {
    glGenBuffers(1, &m_Vbo);
    glGenVertexArrays(1, &m_Vao);
    m_Generated = true;
  }
}

void SimpleRenderer::BindBuffers()
{
  if (m_Generated) {
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
  }
}

void SimpleRenderer::UnbindVertexArray()
{
  glBindVertexArray(NULL);
}

void SimpleRenderer::UnbindBuffers()
{
  glBindBuffer(GL_ARRAY_BUFFER, NULL);
  glBindVertexArray(NULL);
}

void SimpleRenderer::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                         const void* pointer)
{
  if (m_Generated) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  }
}

void SimpleRenderer::BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
  if (m_Generated) {
    glBufferData(target, size, data, usage);
    m_BufferUsage = usage;
  }
}

void SimpleRenderer::Draw(GLenum mode, GLint first, GLsizei count)
{
  if (m_Generated) {
    glDrawArrays(mode, first, count);
  }
}

void SimpleRenderer::SubBufferData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
  if (m_Generated && m_BufferUsage == GL_DYNAMIC_DRAW) {
    glBindBuffer(target, m_Vbo);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, NULL);
  }
}

void SimpleRenderer::DeleteBuffers()
{
  if (m_Generated) {
    glDeleteBuffers(1, &m_Vbo);
    glDeleteVertexArrays(1, &m_Vao);
    m_Generated = false;
  }
}

ElementBufferRenderer::ElementBufferRenderer() : SimpleRenderer() {}
ElementBufferRenderer::~ElementBufferRenderer() {}

void ElementBufferRenderer::DrawInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices,
                                          GLsizei instancecount)
{
  if (m_Generated)
    glDrawElementsInstanced(mode, count, type, indices, instancecount);
}

void ElementBufferRenderer::Draw(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
  if (m_Generated)
    glDrawElements(mode, count, type, indices);
}

void SimpleRenderer::BindVertexArray()
{
  if (m_Generated)
    glBindVertexArray(m_Vao);
}

void ElementBufferRenderer::GenBuffers()
{
  if (!m_Generated) {
    glGenBuffers(1, &m_Vbo);
    glGenBuffers(1, &m_Ebo);
    glGenVertexArrays(1, &m_Vao);
    m_Generated = true;
  }
}

void ElementBufferRenderer::BindBuffers()
{
  if (m_Generated) {
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
  }
}

void ElementBufferRenderer::UnbindBuffers()
{
  /** Unbinding the vbo and ebo causes a seg fault, why? */
  UnbindVertexArray();
}

void ElementBufferRenderer::DeleteBuffers()
{
  if (m_Generated) {
    glDeleteBuffers(1, &m_Vbo);
    glDeleteBuffers(1, &m_Ebo);
    glDeleteVertexArrays(1, &m_Vao);
    m_Generated = false;
  }
}
