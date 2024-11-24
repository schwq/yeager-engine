#include "OpenGLRender.h"
using namespace Yeager;

SimpleRenderer::~SimpleRenderer()
{
  if (bIsGenerated)
    DeleteBuffers();
}

void SimpleRenderer::GenBuffers()
{
  if (!bIsGenerated) {
    GL_CALL(glGenBuffers(1, &mVbo));
    GL_CALL(glGenVertexArrays(1, &mVao));
    bIsGenerated = true;
  }
}

void SimpleRenderer::BindBuffers()
{
  if (bIsGenerated) {
    GL_CALL(glBindVertexArray(mVao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
  }
}

void SimpleRenderer::UnbindVertexArray()
{
  GL_CALL(glBindVertexArray(NULL));
}

void SimpleRenderer::UnbindBuffers()
{
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
  GL_CALL(glBindVertexArray(NULL));
}

void SimpleRenderer::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                         const void* pointer)
{
  if (bIsGenerated) {
    GL_CALL(glEnableVertexAttribArray(index));
    GL_CALL(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
  }
}

void SimpleRenderer::BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
  if (bIsGenerated) {
    GL_CALL(glBufferData(target, size, data, usage));
    mBufferUsage = usage;
  }
}

void SimpleRenderer::Draw(GLenum mode, GLint first, GLsizei count)
{
  if (bIsGenerated)
    GL_CALL(glDrawArrays(mode, first, count));
}

void SimpleRenderer::SubBufferData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
  if (bIsGenerated && mBufferUsage == GL_DYNAMIC_DRAW) {
    GL_CALL(glBindBuffer(target, mVbo));
    GL_CALL(glBufferSubData(target, offset, size, data));
    GL_CALL(glBindBuffer(target, NULL));
  }
}

void SimpleRenderer::DeleteBuffers()
{
  if (bIsGenerated) {
    GL_CALL(glDeleteBuffers(1, &mVbo));
    GL_CALL(glDeleteVertexArrays(1, &mVao));
    bIsGenerated = false;
  }
}

ElementBufferRenderer::ElementBufferRenderer() : SimpleRenderer() {}

ElementBufferRenderer::~ElementBufferRenderer()
{
  if (bIsGenerated)
    DeleteBuffers();
}

void ElementBufferRenderer::DrawInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices,
                                          GLsizei instancecount)
{
  if (bIsGenerated)
    GL_CALL(glDrawElementsInstanced(mode, count, type, indices, instancecount));
}

void ElementBufferRenderer::Draw(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
  if (bIsGenerated)
    GL_CALL(glDrawElements(mode, count, type, indices));
}

void SimpleRenderer::BindVertexArray()
{
  if (bIsGenerated)
    GL_CALL(glBindVertexArray(mVao));
}

void ElementBufferRenderer::GenBuffers()
{
  if (!bIsGenerated) {
    GL_CALL(glGenBuffers(1, &mVbo));
    GL_CALL(glGenBuffers(1, &mEbo));
    GL_CALL(glGenVertexArrays(1, &mVao));
    bIsGenerated = true;
  }
}

void ElementBufferRenderer::BindBuffers()
{
  if (bIsGenerated) {
    GL_CALL(glBindVertexArray(mVao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mVbo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo));
  }
}

void ElementBufferRenderer::UnbindBuffers()
{
  /** Unbinding the vbo and ebo causes a seg fault, why? */
  UnbindVertexArray();
}

void ElementBufferRenderer::DeleteBuffers()
{
  if (bIsGenerated) {
    GL_CALL(glDeleteBuffers(1, &mVbo));
    GL_CALL(glDeleteBuffers(1, &mEbo));
    GL_CALL(glDeleteVertexArrays(1, &mVao));
    bIsGenerated = false;
  }
}
