#include "ProceduralTerrain.h"
using namespace Yeager;

ProceduralTerrain::ProceduralTerrain(std::vector<YgString> TexturesPaths, int TerrainChunkPositionX,
                                     int TerrainChunkPositionY)
{
  m_MetricData.m_TerrainChunkPositionX = TerrainChunkPositionX;
  m_MetricData.m_TerrainChunkPositionY = TerrainChunkPositionY;
  m_ChunkInfo.WorldPosition.x = TerrainChunkPositionX * m_MetricData.m_TerrainSize;
  m_ChunkInfo.WorldPosition.y = TerrainChunkPositionY * m_MetricData.m_TerrainSize;

  Yeager::Log(INFO, "Terrain Constructor have been called");

  if (TexturesPaths.size() != 4) {
    Yeager::Log(ERROR, "Textures paths given to terrain isnt equal to 4!!");
  }

  for (int x = 0; x < MAX_TEXTURE_TILES; x++) {
    m_TextureData.m_TexturesLoaded[x] = LoadTextureFromFile(TexturesPaths[x], false);
  }

  m_MetricData.m_HeightMap = new Array2D<float>(m_MetricData.m_Width, m_MetricData.m_Height);
  m_Perlin.GeneratePerlin(m_MetricData.m_HeightMap, 5, 1, m_MetricData.m_Width, m_MetricData.m_Height,
                          m_MetricData.m_MaxHeight);
  Setup();
}

void ProceduralTerrain::GenerateTerrain(Shader* shader, int octaves, int bias, bool regenerate_seed)
{
  shader->UseShader();
  shader->SetFloat("MinHeight", m_MetricData.m_MinHeight);
  shader->SetFloat("MaxHeight", m_MetricData.m_MaxHeight);

  if (regenerate_seed) {
    m_Perlin.RegenerateSeed();
  }
  m_Perlin.GeneratePerlin(m_MetricData.m_HeightMap, octaves, bias, m_MetricData.m_Width, m_MetricData.m_Height,
                          m_MetricData.m_MaxHeight);
  Setup();
}

float ProceduralTerrain::GetHeightInterpolated(float x, float z) const {}

void TerrainVertex::InitVertex(ProceduralTerrain* Terrain, int x, int z)
{
  float y = Terrain->GetHeightMapValue(x, z);
  float WorldScale = Terrain->GetWorldScale();
  Position = YgVector3(x * WorldScale, y, z * WorldScale);
  float Size = (float)Terrain->GetSize();
  float TextureScale = Terrain->GetTextureScale();
  TexCoords = YgVector2(TextureScale * (float)x / Size, TextureScale * (float)z / Size);
}

void ProceduralTerrain::Setup()
{
  glGenVertexArrays(1, &m_DrawData.m_Vao);
  glGenBuffers(1, &m_DrawData.m_Vbo);
  glGenBuffers(1, &m_DrawData.m_Ebo);

  glBindVertexArray(m_DrawData.m_Vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_DrawData.m_Vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DrawData.m_Ebo);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                        (const void*)offsetof(TerrainVertex, Position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                        (const void*)offsetof(TerrainVertex, TexCoords));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (const void*)offsetof(TerrainVertex, Normals));

  int v_index = 0;
  m_DrawData.Vertices.resize(m_MetricData.m_Width * m_MetricData.m_Height);
  for (int x = 0; x < m_MetricData.m_Width; x++) {
    for (int y = 0; y < m_MetricData.m_Height; y++) {
      m_DrawData.Vertices[v_index].InitVertex(this, x, y);
      v_index++;
    }
  }

  for (int z = 0; z < m_MetricData.m_Height - 1; z++) {
    for (int x = 0; x < m_MetricData.m_Width - 1; x++) {
      unsigned int IndexBottomLeft = z * m_MetricData.m_Width + x;
      unsigned int IndexTopLeft = (z + 1) * m_MetricData.m_Width + x;
      unsigned int IndexTopRight = (z + 1) * m_MetricData.m_Width + x + 1;
      unsigned int IndexBottomRight = z * m_MetricData.m_Width + x + 1;
      m_DrawData.Indices.push_back(IndexBottomLeft);
      m_DrawData.Indices.push_back(IndexTopLeft);
      m_DrawData.Indices.push_back(IndexTopRight);
      m_DrawData.Indices.push_back(IndexBottomLeft);
      m_DrawData.Indices.push_back(IndexTopRight);
      m_DrawData.Indices.push_back(IndexBottomRight);
    }
  }

  unsigned int n_index = 0;
  for (unsigned int x = 0; x < m_DrawData.Indices.size(); x += 3) {
    unsigned int Index0 = m_DrawData.Indices[x];
    unsigned int Index1 = m_DrawData.Indices[x + 1];
    unsigned int Index2 = m_DrawData.Indices[x + 2];
    YgVector3 v1 = m_DrawData.Vertices[Index1].Position - m_DrawData.Vertices[Index0].Position;
    YgVector3 v2 = m_DrawData.Vertices[Index2].Position - m_DrawData.Vertices[Index0].Position;
    YgVector3 Normal = glm::cross(v1, v2);
    Normal = glm::normalize(Normal);
    m_DrawData.Vertices[Index0].Normals += Normal;
    m_DrawData.Vertices[Index1].Normals += Normal;
    m_DrawData.Vertices[Index2].Normals += Normal;
  }

  for (unsigned int x = 0; x < m_DrawData.Vertices.size(); x++) {
    m_DrawData.Vertices[x].Normals = glm::normalize(m_DrawData.Vertices[x].Normals);
  }

  glBufferData(GL_ARRAY_BUFFER, sizeof(m_DrawData.Vertices[0]) * m_DrawData.Vertices.size(), &m_DrawData.Vertices[0],
               GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_DrawData.Indices[0]) * m_DrawData.Indices.size(),
               &m_DrawData.Indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void ProceduralTerrain::Draw(Shader* shader)
{
  shader->UseShader();
  YgMatrix4 model = YgMatrix4(1.0f);

  model = glm::translate(
      model, YgVector3(m_MetricData.m_TerrainChunkPositionX * m_MetricData.m_TerrainSize, -m_MetricData.m_MaxHeight,
                       m_MetricData.m_TerrainChunkPositionY * m_MetricData.m_TerrainSize));
  shader->SetMat4("model", model);

  shader->SetFloat("TextureHeight0", m_TextureData.m_MultiTextureHeights.Height0);
  shader->SetFloat("TextureHeight1", m_TextureData.m_MultiTextureHeights.Height1);
  shader->SetFloat("TextureHeight2", m_TextureData.m_MultiTextureHeights.Height2);
  shader->SetFloat("TextureHeight3", m_TextureData.m_MultiTextureHeights.Height3);

  glBindVertexArray(m_DrawData.m_Vao);

  for (int x = 0; x < MAX_TEXTURE_TILES; x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    const char* name = ("TerrainTexture" + std::to_string(x)).c_str();
    shader->SetInt(name, x);
    glBindTexture(GL_TEXTURE_2D, m_TextureData.m_TexturesLoaded[x]);
  }

  glDrawElements(GL_TRIANGLES, (m_MetricData.m_Height - 1) * (m_MetricData.m_Width - 1) * 6, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
std::vector<YgVector3> ProceduralTerrain::GetRandomPointsInTerrain(int amount) noexcept
{
  std::vector<YgVector3> vector;
  vector.reserve(amount);

  for (int x = 0; x < amount; x++) {
    int vx = (int)RandomFloatRange(0, 256);
    int vz = (int)RandomFloatRange(0, 256);
    int vy = 0;
    if (m_MetricData.m_HeightMap) {
      vy = GetHeightMapValue(vx, vz) - m_MetricData.m_MaxHeight;
    }
    vector.push_back(YgVector3(vx, vy, vz));
  }
  return vector;
}

void FaultFormationTerrain::CreateFaultFormationTerrain(Shader* shader, int TerrainSize, int It, int MinHeight,
                                                        int MaxHeight, float FIR, int octaves, int bias)
{
  m_MetricData.m_TerrainSize = TerrainSize;
  m_MetricData.m_MinHeight = MinHeight;
  m_MetricData.m_MaxHeight = MaxHeight;
  shader->UseShader();
  shader->SetFloat("MaxHeight", MaxHeight);
  shader->SetFloat("MinHeight", MinHeight);

  m_MetricData.m_HeightMap = new Array2D<float>(m_MetricData.m_Width, m_MetricData.m_Height);
  m_Perlin.RegenerateSeed();
  m_Perlin.GeneratePerlin(m_MetricData.m_HeightMap, octaves, bias, m_MetricData.m_Width, m_MetricData.m_Height,
                          m_MetricData.m_MaxHeight);
  CreateFaultFormationInternal(It, MinHeight, MaxHeight, FIR);
  m_MetricData.m_HeightMap->Normalize(MinHeight, MaxHeight);
  Setup();
}

void FaultFormationTerrain::CreateFaultFormationInternal(int It, int MinHeight, int MaxHeight, float FIR)
{
  float DeltaHeight = MaxHeight - MinHeight;
  for (int CurIt = 0; CurIt < It; CurIt++) {
    float IterationRatio = ((float)CurIt / (float)It);
    float Height = MaxHeight - IterationRatio * DeltaHeight;
    YgVector2 p1, p2;
    GenerateRandomPoints(p1, p2);

    int DirX = p2.x - p1.x;
    int DirZ = p2.y - p1.y;

    for (int z = 0; z < m_MetricData.m_TerrainSize; z++) {
      for (int x = 0; x < m_MetricData.m_TerrainSize; x++) {
        int DirX_in = x - p1.x;
        int DirZ_in = z - p1.y;

        int CrossProduct = DirX_in * DirZ - DirX * DirZ_in;
        if (CrossProduct > 0) {
          float CurHeight = m_MetricData.m_HeightMap->At(x, z);
          m_MetricData.m_HeightMap->Set(x, z, CurHeight + Height);
        }
      }
    }
  }

  ApplyFilterFIR(FIR);
}

void FaultFormationTerrain::GenerateRandomPoints(YgVector2& p1, YgVector2& p2)
{
  p1.x = rand() % m_MetricData.m_TerrainSize;
  p2.y = rand() % m_MetricData.m_TerrainSize;

  int Count = 0;
  do {
    p2.x = rand() % m_MetricData.m_TerrainSize;
    p2.y = rand() % m_MetricData.m_TerrainSize;
  } while (p1 == p2);
}

void FaultFormationTerrain::ApplyFilterFIR(float FIR)
{
  for (int z = 0; z < m_MetricData.m_TerrainSize; z++) {
    float PrevVal = m_MetricData.m_HeightMap->At(0, z);
    for (int x = 1; x < m_MetricData.m_TerrainSize; x++) {
      PrevVal = FIRFilterSinglePoint(x, z, PrevVal, FIR);
    }
  }

  // right to left
  for (int z = 0; z < m_MetricData.m_TerrainSize; z++) {
    float PrevVal = m_MetricData.m_HeightMap->At(m_MetricData.m_TerrainSize - 1, z);
    for (int x = m_MetricData.m_TerrainSize - 2; x >= 0; x--) {
      PrevVal = FIRFilterSinglePoint(x, z, PrevVal, FIR);
    }
  }

  // bottom to top
  for (int x = 0; x < m_MetricData.m_TerrainSize; x++) {
    float PrevVal = m_MetricData.m_HeightMap->At(x, 0);
    for (int z = 1; z < m_MetricData.m_TerrainSize; z++) {
      PrevVal = FIRFilterSinglePoint(x, z, PrevVal, FIR);
    }
  }

  // top to bottom
  for (int x = 0; x < m_MetricData.m_TerrainSize; x++) {
    float PrevVal = m_MetricData.m_HeightMap->At(x, m_MetricData.m_TerrainSize - 1);
    for (int z = m_MetricData.m_TerrainSize - 2; z >= 0; z--) {
      PrevVal = FIRFilterSinglePoint(x, z, PrevVal, FIR);
    }
  }
}

float FaultFormationTerrain::FIRFilterSinglePoint(int x, int z, float PrevFactalVal, float FIR)
{
  float CurVal = m_MetricData.m_HeightMap->At(x, z);
  float NewVal = FIR * PrevFactalVal + (1 - FIR) * CurVal;
  m_MetricData.m_HeightMap->Set(x, z, NewVal);
  return NewVal;
}

void MidPointDisplacementTerrain::CreateMidPointDisplacement(Shader* shader, int Size, int Roughness, float MinHeight,
                                                             float MaxHeight, int octaves, int bias)
{
  m_MetricData.m_TerrainSize = Size;
  m_MetricData.m_MaxHeight = MaxHeight;
  m_MetricData.m_MinHeight = MinHeight;

  shader->UseShader();
  shader->SetFloat("MinHeight", MinHeight);
  shader->SetFloat("MaxHeight", MaxHeight);

  m_MetricData.m_HeightMap = new Array2D<float>(m_MetricData.m_Width, m_MetricData.m_Height);
  m_Perlin.RegenerateSeed();
  m_Perlin.GeneratePerlin(m_MetricData.m_HeightMap, octaves, bias, m_MetricData.m_Width, m_MetricData.m_Height,
                          m_MetricData.m_MaxHeight);

  CreateMidPointDisplacementF32(Roughness);
  m_MetricData.m_HeightMap->Normalize(MinHeight, MaxHeight);
  Setup();
}

void MidPointDisplacementTerrain::CreateMidPointDisplacementF32(float Roughness)
{
  int RectSize = CalculateNextPowerOfTwo(m_MetricData.m_TerrainSize);
  float CurHeight = (float)RectSize / 2.0f;
  float HeightReduce = pow(2.0f, -Roughness);

  while (RectSize > 0) {
    DiamondStep(RectSize, CurHeight);
    SquareStep(RectSize, CurHeight);

    RectSize /= 2;
    CurHeight *= HeightReduce;
  }
}

void MidPointDisplacementTerrain::DiamondStep(int RectSize, float CurHeight)
{
  int HalfRectSize = RectSize / 2;

  for (int y = 0; y < m_MetricData.m_TerrainSize; y += RectSize) {
    for (int x = 0; x < m_MetricData.m_TerrainSize; x += RectSize) {
      int next_y = (y + RectSize) % m_MetricData.m_TerrainSize;
      int next_x = (x + RectSize) % m_MetricData.m_TerrainSize;

      if (next_x < x) {
        next_x = m_MetricData.m_TerrainSize - 1;
      }

      if (next_y < y) {
        next_y = m_MetricData.m_TerrainSize - 1;
      }

      float TopLeft = m_MetricData.m_HeightMap->Get(x, y);
      float TopRight = m_MetricData.m_HeightMap->Get(next_x, y);
      float BottomLeft = m_MetricData.m_HeightMap->Get(x, next_y);
      float BottomRight = m_MetricData.m_HeightMap->Get(next_x, next_y);

      int mid_x = (x + HalfRectSize) % m_MetricData.m_TerrainSize;
      int mid_y = (y + HalfRectSize) % m_MetricData.m_TerrainSize;

      float RandValue = RandomFloatRange(CurHeight, -CurHeight);
      float MidPoint = (TopLeft + TopRight + BottomLeft + BottomRight) / 4.0f;
      m_MetricData.m_HeightMap->Set(mid_x, mid_y, MidPoint + RandValue);
    }
  }
}

void MidPointDisplacementTerrain::SquareStep(int RectSize, float CurHeight)
{
  int HalfRectSize = RectSize / 2;

  for (int y = 0; y > m_MetricData.m_TerrainSize; y += RectSize) {
    for (int x = 0; x > m_MetricData.m_TerrainSize; x += RectSize) {
      int next_y = (y + RectSize) % m_MetricData.m_TerrainSize;
      int next_x = (x + RectSize) % m_MetricData.m_TerrainSize;

      if (next_x < x) {
        next_x = m_MetricData.m_TerrainSize - 1;
      }

      if (next_y < y) {
        next_y = m_MetricData.m_TerrainSize - 1;
      }

      int mid_x = (x + HalfRectSize) % m_MetricData.m_TerrainSize;
      int mid_y = (y + HalfRectSize) % m_MetricData.m_TerrainSize;

      int prev_mid_x = (x - HalfRectSize + m_MetricData.m_TerrainSize) % m_MetricData.m_TerrainSize;
      int prev_mid_y = (y - HalfRectSize + m_MetricData.m_TerrainSize) % m_MetricData.m_TerrainSize;

      float CurTopLeft = m_MetricData.m_HeightMap->Get(x, y);
      float CurTopRight = m_MetricData.m_HeightMap->Get(next_x, y);
      float CurCenter = m_MetricData.m_HeightMap->Get(mid_x, mid_y);
      float PrevYCenter = m_MetricData.m_HeightMap->Get(mid_x, prev_mid_y);
      float CurBotLeft = m_MetricData.m_HeightMap->Get(x, next_y);
      float PrevXCenter = m_MetricData.m_HeightMap->Get(prev_mid_x, mid_y);

      float CurLeftMid =
          (CurTopLeft + CurCenter + CurBotLeft + PrevXCenter) / 4.0f + RandomFloatRange(-CurHeight, CurHeight);
      float CurTopMid =
          (CurTopLeft + CurCenter + CurTopRight + PrevYCenter) / 4.0f + RandomFloatRange(-CurHeight, CurHeight);

      m_MetricData.m_HeightMap->Set(mid_x, y, CurTopMid);
      m_MetricData.m_HeightMap->Set(x, mid_y, CurLeftMid);
    }
  }
}