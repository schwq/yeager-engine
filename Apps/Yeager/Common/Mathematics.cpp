#include "Mathematics.h"
using namespace Yeager::Math;

std::vector<float> Yeager::Math::GenerateCircleVertices(float cx, float cy, float r, int segments,
                                                        PredefinedPlanes::Enum plane)
{
  std::vector<float> vertices;
  for (int ii = 0; ii < segments; ii++) {
    float theta = 2.0f * YEAGER_PI * float(ii) / float(segments);
    float x = r * cosf(theta);
    float y = r * sinf(theta);
    switch (plane) {
      case PredefinedPlanes::XZ_PLANE:
        vertices.push_back(x);
        vertices.push_back(0);
        vertices.push_back(y);
      case PredefinedPlanes::YZ_PLANE:
        vertices.push_back(0);
        vertices.push_back(x);
        vertices.push_back(y);
      case PredefinedPlanes::XY_PLANE:
      default:
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0);
    }
  }
  return vertices;
}

int Yeager::Math::CalculateNextPowerOfTwo(int x)
{
  int ret = 1;
  if (x == 1) {
    return 2;
  }

  while (ret < x) {
    ret = ret * 2;
  }

  return ret;
}

float Yeager::Math::RandomFloat()
{
  float Max = RAND_MAX;
  return ((float)rand() / Max);
}

float Yeager::Math::RandomFloatRange(float Start, float End)
{
  if (End == Start) {
    Yeager::Log(ERROR, "Random Float Range, End is equal to Start!");
    return 0;
  }

  float Delta = End - Start;
  float RandomVal = RandomFloat() * Delta + Start;
  return RandomVal;
}
