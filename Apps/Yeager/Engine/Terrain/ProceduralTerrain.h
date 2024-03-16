//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023-present
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
#include "../../Common/Mathematics.h"
#include "../../Common/Utilities.h"
#include "../Renderer/ShaderHandle.h"
#include "../Renderer/TextureHandle.h"
#include "PerlinNoise.h"

namespace Yeager {

class ProceduralTerrain;

/** The terrain macro for holding the maximum of textures loaded together */
#define MAX_TEXTURE_TILES 4

/** Used for single texturing terrain, it contains the values used to determine the texture to used and the interpolation between two textures */
struct TextureHeightDesc {
  float Low = 0.0f;
  float Optimal = 0.0;
  float High = 0.0f;
};

/// @brief Self explain struct, contains the openGL texture to be load an the height description for single texturing
struct TextureTile {
  MaterialTexture2D Image;
  TextureHeightDesc HeightDesc;
};

/// @todo Finish this class
class TerrainTextureGen {
 public:
  TerrainTextureGen();
  void LoadTile(const String& path);
  Texture2D* GenerateTexture(int TextureSize, ProceduralTerrain* Terrain, float MinHeight, float MaxHeight);

 protected:
  void CalculateTextureRegions(float MinHeight, float MaxHeight);
  float RegionPercent(int Tile, float Height);

  TextureTile m_TextureTiles[MAX_TEXTURE_TILES] = {};
  int m_NumTextureTiles = 0;
};

/// @brief  Self explain, this struct holds the drawable variables used for openGL to render the vertex, this is used when we are drawing mesh by mesh of a model
/// @brief  This kind of method of drawing can be used when the used is in the editor mode to manipulate the meshes and vertices of a model, but it can be costly when drawing
/// @brief  multiple instances of the same model
/// @todo   Review this kind of method
struct TerrainVertex {
  Vector3 Position;
  Vector2 TexCoords;
  Vector3 Normals = Vector3(0.0f, 0.0f, 0.0f);
  void InitVertex(ProceduralTerrain* Terrain, int x, int z);
};

/// @brief  This struct is different for the TextureHeightDesc struct, it contains the heights used when doing
///         multi texturing in the terrain, interpolating between textures used to render the mesh of the terrain
struct MultiTextureHeight {
  float Height0 = 64.0f;
  float Height1 = 128.0f;
  float Height2 = 192.0f;
  float Height3 = 256.0f;
};

/// @brief Struct for holding the drawable data for the terrain, like Vertices, Indices, and buffers IDs
struct TerrainDrawData {
  std::vector<TerrainVertex> Vertices;
  std::vector<GLuint> Indices;
  GLuint m_Vao, m_Vbo, m_Ebo;
};

/// @brief Metric data holds the values for positioning the terrain in the space, like width, lenght, height, world scale, ect...
struct TerrainMetricData {
  Array2D<float>* m_HeightMap = YEAGER_NULLPTR;
  float m_MinHeight = 0.0f;
  float m_MaxHeight = 256.0f;
  int m_TerrainSize = 256;
  float WorldScale = 1.0f;
  int m_Width = 256, m_Height = 256;
  int m_TerrainChunkPositionX = 0;
  int m_TerrainChunkPositionY = 0;
};

/// @brief  Terrain texturing holds variables for the necessarie texturing of the terrain,
///         heights descriptions, texture scales, and the array of textures loaded
struct TerrainTexturingData {
  MaterialTexture2D m_TexturesLoaded[MAX_TEXTURE_TILES] = {};
  MultiTextureHeight m_MultiTextureHeights;
  float m_TextureScale = 256.0f;
};

struct TerrainChunkInformation {
  Vector3 WorldPosition;
};

/// @brief Main class for Terrain rendering, it contains all the basic and common variables and functions of terrains in the engine
class ProceduralTerrain {
 public:
  /**
  * @brief Construct a new Procedural Terrain object, builds the hightmap array, and generate the perlin noise
  * 
  * @param TexturesPaths Vector with strings of the paths of the textures to be used, is must contain the same number as MAX_TEXTURE_TILES
  */
  ProceduralTerrain(std::vector<String> TexturesPaths, int TerrainChunkPositionX = 0, int TerrainChunkPositionY = 0);

  /**
   * @brief                 Sets attrib to the terrain, calls setup() and regenerate perlin 
   * 
   * @param shader          Shader to be used
   * @param octaves         Octaves of the Noise generation
   * @param bias            Bias of the Noise generation
   * @param regenerate_seed If is necessarie to regenerate the seed of the Perlin Noise
   */
  void GenerateTerrain(Shader* shader, int octaves, int bias, bool regenerate_seed = true);

  /** Setup the openGL buffers and vertex arrays, assign the attribs*/
  void Setup();

  /**
   * @brief         Draw the terrain, calling the meshes draw method
   * @note          Shader attrib must be declared before calling this!
   * @param shader  Shader to be used
   */
  void Draw(Shader* shader);

  /**
   * @brief     Get the value of the height interpolated between two points
   * 
   * @param x   The x position to find the value
   * @param z   The z position to find the value
   * @return    The interpolated value 
   */
  float GetHeightInterpolated(float x, float z) const;

  /**
   * @brief   Get the Texture Scale (MultiTexturing)
   * 
   * @return  Textures Scale
   */
  YEAGER_FORCE_INLINE float GetTextureScale() const { return m_TextureData.m_TextureScale; }

  /**
   * @brief     Get the value at the Heightmap
   * @note      This position in the heightmap, also represent the space position of the chunk
   * @param x   The x position to retrieve the value
   * @param z   The z position to retrieve the value
   * @return    Value in the heightmap
   */
  YEAGER_FORCE_INLINE float GetHeightMapValue(int x, int z) { return m_MetricData.m_HeightMap->At(x, z); }

  /**
   * @brief   Get the world scale
   * 
   * @return  World scale as a float
   */
  YEAGER_FORCE_INLINE float GetWorldScale() const { return m_MetricData.WorldScale; }

  /**
   * @brief Returns the terrain size, as width and lenght are the same value
   * 
   * @return Terrain size as float
   */
  YEAGER_FORCE_INLINE float GetSize() const { return m_MetricData.m_TerrainSize; }

  /**
   * @brief         Generates random points in the surface of the terrain, used to positioning trees for example
   * @note          This function must be [no_discard] because it can be expensive, and cannot be ignored
   * @param amount  The amount of values to the return
   * @return        A std::vector of 3D vectors representing the random positions generated
   */
  YEAGER_NODISCARD std::vector<Vector3> GetRandomPointsInTerrain(int amount) noexcept;

  YEAGER_NODISCARD TerrainChunkInformation* GetChunkInformation() { return &m_ChunkInfo; }

 protected:
  TerrainDrawData m_DrawData;
  TerrainMetricData m_MetricData;
  TerrainTexturingData m_TextureData;
  TerrainChunkInformation m_ChunkInfo;
  PerlinNoise m_Perlin;
};

/** Fault formation terrain is a type of terrain that creates faults like in the real life, represeting the sismics actions of nature */
class FaultFormationTerrain : public ProceduralTerrain {
 public:
  /**
   * @brief               Construct a new Fault formation Terrain object, builds the hightmap array, 
   *                      and generate the perlin noise (Calling the super constructor)
   * 
   * @param TexturesPaths Vector with strings of the paths of the textures to be used, is must contain the same number as MAX_TEXTURE_TILES
   */
  FaultFormationTerrain(std::vector<String> TexturesPaths, int TerrainChunkPositionX = 0, int TerrainChunkPositionY = 0)
      : ProceduralTerrain(TexturesPaths, TerrainChunkPositionX, TerrainChunkPositionY)
  {}

  /**
   * @brief             Creates the heightmap values, by doing mostly the same as GenerateProceduralTerrain(), 
   *                    but calling the fault formation internal for calculations, and normalizing the heights
   * 
   * @param shader      Shader to be used
   * @param TerrainSize The Terrain size to generate
   * @param It          Iterations, in the terrain, more iterations is equal to more "rough" terrain
   * @param MinHeight   The minimum height permitted to the terrain to generate
   * @param MaxHeight   The maximum height permitted to the terrain to generate
   * @param FIR         The filter to be used in the most high points of the terrain, removinf the sharp aspect of it (Erosive filter)
   * @param octaves     Octaves to generate perlin noise
   * @param bias        Bias to generete perlin noise
   */
  void CreateFaultFormationTerrain(Shader* shader, int TerrainSize, int It, int MinHeight, int MaxHeight, float FIR,
                                   int octaves, int bias);

 protected:
  /**
   * @brief           Generate the faults in the heightmap, getting random points, and choosing one side to be submerged and appling the filter
   * 
   * @param It        Iterations to create, each one is a fault in the terrain
   * @param MinHeight The minimum height permitted to the terrain to generate
   * @param MaxHeight The maximum height permitted to the terrain to generate
   * @param FIR       The filter to be used in the most high points of the terrain, removinf the sharp aspect of it (Erosive filter)
   */
  void CreateFaultFormationInternal(int It, int MinHeight, int MaxHeight, float FIR);

  /**
   * @brief     Generates two random points in the terrain for the calculation on the fault formation
   * @note      This function is different from the GetRandomPointsInTheTerrain, because in doesnt not get hte values from the heightmap!
   * @param p1  The first point
   * @param p2  The second point
   */
  void GenerateRandomPoints(Vector2& p1, Vector2& p2);

  /**
   * @brief     Applys the erosive filter to the high points of the fault, creating a more natural feel 
   * @param FIR The filter value, between 0.0 and 0.99
   */
  void ApplyFilterFIR(float FIR);

  /**
   * @brief               Applys the erosive filter in each single point of the terrain 
   * 
   * @param x             The x position of the point
   * @param z             The y position of the point
   * @param PrevFactalVal The previous factal value of the last point, is used to decrease this value and make the filter looks like rounding down
   * @param FIR           The filter constant 
   * @return              Returns the new value of the factal value, to be used the next time calling this function
   */
  float FIRFilterSinglePoint(int x, int z, float PrevFactalVal, float FIR);
};

/** Midpoint displacement terrain is used to create mountain like terrains, picking a point and decreasing the surrondings of it */
class MidPointDisplacementTerrain : public ProceduralTerrain {
 public:
  /**
  * @brief                Construct a new Midpoint displacement Terrain object, builds the hightmap array, 
  *                       and generate the perlin noise (Calling the super constructor)
  * 
  * @param TexturesPaths  Vector with strings of the paths of the textures to be used, is must contain the same number as MAX_TEXTURE_TILES
  */
  MidPointDisplacementTerrain(std::vector<String> TexturesPaths, int TerrainChunkPositionX = 0,
                              int TerrainChunkPositionY = 0)
      : ProceduralTerrain(TexturesPaths, TerrainChunkPositionX, TerrainChunkPositionY)
  {}

  /**
   * @brief             Creates the midpoint displacement terrain, sets the attribs, generates  the perlin noise, normalize the heightmap, 
   *                    and setup the openGL
   * 
   * @param shader      Shader to be used
   * @param Size        The terrain size
   * @param Roughness   Roughness of the terrain, the more rough, more details it will contain
   * @param MinHeight   The minimum height permitted to the terrain to generate
   * @param MaxHeight   The maximum height permitted to the terrain to generate
   * @param octaves     Octaves to generate perlin noise
   * @param bias        Bias to generete perlin noise
   */
  void CreateMidPointDisplacement(Shader* shader, int Size, int Roughness, float MinHeight, float MaxHeight,
                                  int octaves, int bias);

 protected:
  /**
  * @brief            Creates the montain like effect on the terrain, given a roughness value
  * 
  * @param Roughness  The ronghness value of the terrain
  */
  void CreateMidPointDisplacementF32(float Roughness);

  /**
   * @brief           Calculate the points between the vertices on the rectangle of the terrain
   * 
   * @param RectSize  Rectangle size
   * @param CurHeight The point in the line between vertices
   */
  void DiamondStep(int RectSize, float CurHeight);

  /**
   * @brief           Calculate the points on the vertices of the current square of the terrain 
   * 
   * @param RectSize  Rectangle size
   * @param CurHeight The points in the vertices of the square
   */
  void SquareStep(int RectSize, float CurHeight);
};
}  // namespace Yeager