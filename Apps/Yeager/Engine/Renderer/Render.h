//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
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

#include "../../Application.h"
#include "../../Common/Common.h"
#include "../../Common/Utilities.h"
#include "../../InputHandle.h"
#include "ImportedObj.h"
#include "ShaderHandle.h"
#include "TextureHandle.h"

class Application;

enum class RendererEngineName { kOpenGL = 0, kDirectX = 1, kVulkan = 2 };

/* TODO: make sure the program args and argv are taken to here for revision, user choices i mean*/
struct RendererEngineHandle {
  RendererEngineName name;
};

class RendererEngine {
 public:
  RendererEngine(RendererEngineName name, Application* app);
  ~RendererEngine(){};

  void Render();
  void checkGLAD();

 private:
  RendererEngineHandle m_handle;
  Application* m_app;

  void RendererOpenGL();
  void ManifestShaderProps(Yeager::Shader* shader, yg_mat4 view, yg_mat4 projection, yg_vec3 viewPos);
};
