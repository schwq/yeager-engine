#include "Mathematics.h"
using namespace Yeager;

int Yeager::CalculateNextPowerOfTwo(int x)
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

float Yeager::RandomFloat()
{
  float Max = RAND_MAX;
  return ((float)rand() / Max);
}

float Yeager::RandomFloatRange(float Start, float End)
{
  if (End == Start) {
    Yeager::Log(ERROR, "Random Float Range, End is equal to Start!");
    return 0;
  }

  float Delta = End - Start;
  float RandomVal = RandomFloat() * Delta + Start;
  return RandomVal;
}
