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

namespace Yeager {
class PerlinNoise {
 public:
  PerlinNoise(int width = 256, int lenght = 256);
  ~PerlinNoise();

  void GeneratePerlin(Yeager::Math::Array2D<float>* arr, int octaves, int bias, int width, int height, float max_height,
                      bool regenerate_seed = false);
  void PerlinNoise2D(int width, int height, float* seed, int octaves, float bias, float* output);
  constexpr void ChangeSize(int width, int height) noexcept
  {
    m_Lenght = height;
    m_Width = width;
  }
  constexpr void RegenerateSeed() noexcept;
  bool SavePerlinNoiseMapToFile(Cchar path);
  YEAGER_NODISCARD inline float* GetSeed() noexcept { return m_Seed; }
  constexpr inline int GetOctaveCount() noexcept { return m_OctaveCount; }
  constexpr inline Vector2 GetSize() noexcept { return Vector2(m_Width, m_Lenght); }
  constexpr inline bool GetIsGenerated() noexcept { return Generated; }

 protected:
  bool Generated = false;
  int m_Width = 256;
  int m_Lenght = 256;
  float* m_Seed = YEAGER_NULLPTR;
  float* m_Noise = YEAGER_NULLPTR;
  int m_OctaveCount = 5;
  float m_ScallingBias = 5.0f;
};
}  // namespace Yeager