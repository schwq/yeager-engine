//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"

namespace Yeager {

/**
 * @brief The simple renderer of OpenGL is a class that holds rendering information. It uses the glDrawArrays and dont have must optimizantion  
 */
class SimpleRenderer {
 public:
  SimpleRenderer() = default;
  ~SimpleRenderer();

  static void UnbindVertexArray();

  virtual void GenBuffers();
  virtual void BindBuffers();
  virtual void BindVertexArray();
  virtual void UnbindBuffers();
  virtual void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
  virtual void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                   const void* pointer);

  virtual void DeleteBuffers();

  virtual void SubBufferData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
  virtual void Draw(GLenum mode, GLint first, GLsizei count);

 protected:
  GLuint mVao = NULL, mVbo = NULL;
  GLenum mBufferUsage = GL_STATIC_DRAW;
  bool bIsGenerated = false;
};

/**
 * @brief The element buffer renderer of OpenGL is like the simple renderer, but is make uses of indices and calls glDrawElements on drawing
 */
class ElementBufferRenderer : public SimpleRenderer {
 public:
  ElementBufferRenderer();
  ~ElementBufferRenderer();

  virtual void GenBuffers();
  virtual void BindBuffers();
  virtual void UnbindBuffers();

  virtual void DeleteBuffers();
  virtual void DrawInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
  virtual void Draw(GLenum mode, GLsizei count, GLenum type, const void* indices);

 protected:
  GLuint mEbo = NULL;
};

}  // namespace Yeager