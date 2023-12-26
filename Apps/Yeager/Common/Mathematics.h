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
#include "Common.h"
#include "LogEngine.h"
#include "Utilities.h"

namespace Yeager {

extern int CalculateNextPowerOfTwo(int x);
extern float RandomFloat();
extern float RandomFloatRange(float Start, float End);

template <typename Type>
class Array2D {
 public:
  Array2D() {}
  Array2D(int Cols, int Rows)
  {
    m_Rows = Rows;
    m_Cols = Cols;

    if (m_Pointer) {
      free(m_Pointer);
    }

    m_Pointer = (Type*)malloc(Cols * Rows * sizeof(Type));
  }

  Type DirectAccess(int index) { return m_Pointer[index]; }

  Array2D(int Cols, int Rows, Type InitValue)
  {
    m_Rows = Rows;
    m_Cols = Cols;

    if (m_Pointer) {
      free(m_Pointer);
    }

    m_Pointer = (Type*)malloc(Cols * Rows * sizeof(Type));
    for (int x = 0; x < Cols * Rows; x++) {
      m_Pointer[x] = InitValue;
    }
  }

  ~Array2D()
  {
    if (m_Pointer) {
      free(m_Pointer);
    }
  }

  constexpr inline Type& At(int Col, int Row)
  {
    size_t Index = CalculateIndex(Col, Row);
    return m_Pointer[Index];
  }

  constexpr inline void Set(int Col, int Row, const Type& val) { *GetAddr(Col, Row) = val; }

  constexpr inline Type* GetAddr(int Col, int Row)
  {
    size_t Index = CalculateIndex(Col, Row);
    return &m_Pointer[Index];
  }

  constexpr inline int GetSize() { return m_Cols * m_Rows; }
  constexpr inline Type* GetAddress(int Col, int Row) { return &m_Pointer[CalculateIndex(Col, Row)]; }
  void Normalize(Type MinRange, Type MaxRange)
  {
    Type Min, Max;
    GetMinMax(Min, Max);

    if (Max <= Min) {
      return;
    }

    Type MinMaxDelta = Max - Min;
    Type MinMaxRange = MaxRange - MinRange;
    for (int x = 0; x < m_Rows * m_Cols; x++) {
      m_Pointer[x] = ((m_Pointer[x] - Min) / MinMaxDelta) * MinMaxRange + MinRange;
    }
  }

  const Type& Get(int Col, int Row) { return *GetAddr(Col, Row); }

 private:
  size_t CalculateIndex(int Col, int Row)
  {
    size_t Index = Row * m_Cols + Col;
    return Index;
  }

  void GetMinMax(Type& Min, Type& Max)
  {
    Max = Min = m_Pointer[0];
    for (int x = 0; x < m_Rows * m_Cols; x++) {
      if (m_Pointer[x] < Min) {
        Min = m_Pointer[x];
      }
      if (m_Pointer[x] > Max) {
        Max = m_Pointer[x];
      }
    }
  }

  int m_Cols = 0;
  int m_Rows = 0;
  Type* m_Pointer = YEAGER_NULLPTR;
};
}  // namespace Yeager