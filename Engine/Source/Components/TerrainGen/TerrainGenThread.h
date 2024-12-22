//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present - Present
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

#include "Common/Math/Mathematics.h"
#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"
#include "Components/Kernel/Process/WpThread.h"
#include "Components/Renderer/GL/OpenGLRender.h"
#include "Components/Renderer/Shader/ShaderHandle.h"
#include "Components/Renderer/Texture/TextureHandle.h"

#include "PerlinNoise.h"
#include "ProceduralTerrain.h"

namespace Yeager {

class TerrainGenThreadManagement {
 public:
  static void CreateThreadFaultFormationTerrain(std::vector<String> TexturesPaths, int TerrainChunkPositionX,
                                                int TerrainChunkPositionY, Shader* shader, int TerrainSize, int It,
                                                int MinHeight, int MaxHeight, float FIR, int octaves, int bias);

  static void UpdateThread();

  static void DrawTerrain();

 private:
  static std::shared_ptr<FaultFormationTerrain> mTerrain;

  static std::shared_ptr<WpThread> mThread;

  static bool bTerrainCanBeDraw;
  static bool bMustGenerateGL;

  static Shader* mShader;

  static std::atomic_bool bIsExecutingThread;
};

}  // namespace Yeager