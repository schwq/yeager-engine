//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

/* This approximation of PI must be precise, the sphere  */
#define YEAGER_PI 3.14159265358979323f

namespace Yeager {
namespace Math {

/* Predefined planes in mathematics locks one of the coordinates of the 3d world to 0, 
and is used to draw 2d figures in the tridimensional space on the engine  */
struct PredefinedPlanes {
  enum Enum { XY_PLANE, XZ_PLANE, YZ_PLANE };
};

extern std::vector<float> GenerateCircleVertices(float cx, float cy, float r, int segments,
                                                 PredefinedPlanes::Enum plane = PredefinedPlanes::XY_PLANE);

extern int CalculateNextPowerOfTwo(int x);
extern float RandomFloat();
extern float RandomFloatRange(float Start, float End);

template <typename Type, typename = typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type>
Type SafeDivide(Type n1, Type n2)
{
  if (n2 == 0)
    return 0;
  return n1 / n2;
}

/* This template only accepts arithmetics values as type (float, int, double.. ) */
template <typename Type, typename = typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type>
Type ArithmeticMean(const std::vector<Type>& sequence)
{
  if (sequence.empty())
    return 0;

  Type sum = 0;

  for (const auto& v : sequence)
    sum += v;

  return sum / sequence.size();
}

template <typename Type, typename = typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type>
std::pair<Type, Type> MaxMin(const std::vector<Type>& list)
{
  if (list.empty())
    return std::pair<Type, Type>(0, 0);

  if (list.size() == 1)
    return std::pair<Type, Type>(list.at(0), list.at(0));

  Type maxValue = 0;
  Type minValue = list.at(0);
  for (const auto& v : list) {
    if (v < minValue)
      minValue = v;
    if (v > maxValue)
      maxValue = v;
  }
  return std::pair<Type, Type>(maxValue, minValue);
}

template <typename Type, typename = typename std::enable_if<std::is_integral<Type>::value, Type>::type>
Type GreatestCommonDivisor(const std::vector<Type>& list)
{
  if (list.empty())
    return 0;

  std::pair<Type, Type> maxMin = MaxMin(list);
  Type divisor = 1;
  for (unsigned int x = 2; x <= maxMin.second; x++) {
    bool ok = true;
    for (const auto& v : list) {
      if (v % x != 0) {
        ok = false;
        break;
      }
    }
    if (ok)
      divisor = x;
  }
  return divisor;
}

template <typename Type, typename = typename std::enable_if<std::is_integral<Type>::value, Type>::type>
Type LeastCommonMultiple(const std::vector<Type>& list)
{
  if (list.empty())
    return 0;

  std::pair<Type, Type> maxMin = MaxMin(list);
  Type multipler = 0;
  Type maxValue = 1;

  for (const auto& v : list)
    maxValue *= v;

  for (unsigned int x = maxMin.first; x <= maxValue; x++) {
    bool ok = true;
    for (const auto& v : list) {
      if (x % v != 0) {
        ok = false;
        break;
      }
    }
    if (ok)
      return x;
  }
  return maxValue;
}

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
}  // namespace Math
}  // namespace Yeager