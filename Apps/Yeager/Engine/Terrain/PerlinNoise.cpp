#include "PerlinNoise.h"
#include "../Renderer/ImageUtilities.h"
using namespace Yeager;

PerlinNoise::PerlinNoise(int width, int lenght)
{
  m_Width = width;
  m_Lenght = lenght;
  m_Seed = new float[m_Width * m_Lenght];
  m_Noise = new float[m_Lenght * m_Width];
  RegenerateSeed();
}

void PerlinNoise::GeneratePerlin(Array2D<float>* arr, int octaves, int bias, int width, int height, float max_height,
                                 bool regenerate_seed)
{
  if (regenerate_seed) {
    RegenerateSeed();
  }
  Generated = true;
  m_Width = width;
  m_Lenght = height;

  PerlinNoise2D(m_Width, m_Lenght, m_Seed, octaves, bias, m_Noise);
  for (int x = 0; x < m_Width; x++) {
    for (int y = 0; y < m_Lenght; y++) {
      arr->At(x, y) = m_Noise[m_Width * y + x] * max_height;
    }
  }
}

bool PerlinNoise::SavePerlinNoiseMapToFile(Cchar path)
{
  if (Generated) {
    std::ofstream fl;
    fl.open(path, std::ofstream::out | std::ofstream::binary);
    if (fl.is_open()) {
      for (int x = 0; x < m_Width; x++) {
        for (int y = 0; y < m_Lenght; y++) {
          fl << m_Noise[m_Width * y + x] << ' ';
        }
      }
      return true;
      fl.close();
    } else {
      Yeager::Log(WARNING, "Cannot open file to save perlin noise! [{}]", path);
      return false;
    }
  } else {
    Yeager::Log(WARNING, "Perlin noise isnt generated yet! Cannot save to file!");
    return false;
  }
}

void PerlinNoise::PerlinNoise2D(int width, int height, float* seed, int octaves, float bias, float* output)
{
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      float fNoise = 0.0f;
      float fScaleAcc = 0.0f;
      float fScale = 1.0f;

      for (int oc = 0; oc < m_OctaveCount; oc++) {
        int nPitch = m_Width >> oc;
        int nSampleX1 = (x / nPitch) * nPitch;
        int nSampleY1 = (y / nPitch) * nPitch;

        int nSampleX2 = (nSampleX1 + nPitch) % width;
        int nSampleY2 = (nSampleY1 + nPitch) % width;

        float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
        float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

        float fSampleT =
            (1.0f - fBlendX) * m_Seed[nSampleY1 * width + nSampleX1] + fBlendX * m_Seed[nSampleY1 * width + nSampleX2];
        float fSampleB =
            (1.0f - fBlendX) * m_Seed[nSampleY2 * width + nSampleX1] + fBlendX * m_Seed[nSampleY2 * width + nSampleX2];

        fScaleAcc += fScale;
        fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
        fScale = fScale / bias;
      }

      output[y * width + x] = fNoise / fScaleAcc;
    }
  }
}

constexpr void PerlinNoise::RegenerateSeed() noexcept
{
  for (int x = 0; x < m_Lenght * m_Width; x++) {
    m_Seed[x] = (float)rand() / (float)RAND_MAX;
  }
}
