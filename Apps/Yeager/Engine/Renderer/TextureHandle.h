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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "Entity.h"
#include "ShaderHandle.h"

namespace Yeager {

class ApplicationCore;

struct STBIDataOutput {
  unsigned char* Data = YEAGER_NULLPTR;
  int Width = 0;
  int Height = 0;
  int NrComponents = 0;
  String OriginalPath = YEAGER_NULL_LITERAL;
};

/* Materials are the process in which, objects gain some color, that can be a defined color by the user, a physical material, or a texture (even multiple textures combined) */
struct MaterialType {
  enum Enum { eTEXTURE2D, eMATERIAL_BASE, eSKYBOX, eCUBEMAP, ePHYSICAL_TEXTURE2D, eUNDEFINED };
};

/* The surface type of a material if the kind of renderering is going to been apply to that material, for instance, 
a textured surface of a material is going to generated and handle a texture to the material */
struct MaterialSurfaceType {
  enum Enum { eTEXTURED, eCOLORED, eMATERIAL_DEFINED, eMULTI_TEXTURED, eUNDEFINED };
};

struct MaterialTextureType {
  enum Enum { eDIFFUSE, eSPECULAR, eMETTALIC, eNORMAL_MAP, eROUGHNESS, eUNDEFINED };
  static String ToString(Enum type)
  {
    switch (type) {
      case eDIFFUSE:
        return "Diffuse";
      case eSPECULAR:
        return "Specular";
      case eMETTALIC:
        return "Mettalic";
      case eNORMAL_MAP:
        return "NormalMap";
      case eROUGHNESS:
        return "Roughness";
      case eUNDEFINED:
      default:
        return "Undefined";
    }
  }
  static Enum ToEnum(const String& str)
  {
    switch (StringToInteger(str.c_str())) {
      case StringToInteger("Diffuse"):
        return eDIFFUSE;
      case StringToInteger("Specular"):
        return eSPECULAR;
      case StringToInteger("Mettalic"):
        return eMETTALIC;
      case StringToInteger("NormalMap"):
        return eNORMAL_MAP;
      case StringToInteger("Roughness"):
        return eROUGHNESS;
      case StringToInteger("Undefined"):
      default:
        return eUNDEFINED;
    }
  }
};

#define YEAGER_MATERIAL_DEFAULT_COLOR Vector4(1.0f, 1.0f, 1.0f, 1.0f)

/* Defaults are initialize for the 2d texture generation*/
struct MateriaTextureParameterGL {
  uint32_t BindTarget = GL_TEXTURE_2D;
  uint32_t WRAP_S = GL_REPEAT;
  uint32_t WRAP_T = GL_REPEAT;
  uint32_t WRAP_R = GL_REPEAT;
  uint32_t BASE_LEVEL = 0;
  uint32_t MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;
  uint32_t MAG_FILTER = GL_LINEAR;

  MateriaTextureParameterGL(uint32_t bind, uint32_t wrap_s, uint32_t wrap_t, uint32_t wrap_r, uint32_t base_level,
                            uint32_t min_filter, uint32_t mag_filter)
      : BindTarget(bind),
        WRAP_S(wrap_s),
        WRAP_T(wrap_t),
        WRAP_R(wrap_r),
        BASE_LEVEL(base_level),
        MIN_FILTER(min_filter),
        MAG_FILTER(mag_filter)
  {}
  MateriaTextureParameterGL() = default;
};

struct MaterialTextureDataHandle {
  GLuint Texture = -1;
  GLenum Format = GL_RGBA;
  MateriaTextureParameterGL Parameter;
  String Path = YEAGER_NULL_LITERAL;
  std::vector<String> CubeMapsPaths;
  int Width = 0;
  int Height = 0;
  bool Generated = false;
  bool Flipped = false;
  bool ImcompletedID = false;  // Multithreaded texture loaded, data is readed but there is still a id missing
};

static GLenum ChannelsToFormat(const int channels);

class MaterialBase : public EditorEntity {
 public:
  MaterialBase(Yeager::ApplicationCore* application, const String& name, const MaterialType::Enum type,
               const MaterialSurfaceType::Enum surface);
  ~MaterialBase();

  constexpr Vector4 GetColor() const { return m_Color; }
  constexpr void SetColor(const Vector4& color) { m_Color = color; }

  virtual void ApplyToShader(Yeager::Shader* shader);

  constexpr MaterialType::Enum GetMaterialType() const { return m_MaterialType; }
  constexpr void SetMaterialType(const MaterialType::Enum material) { m_MaterialType = material; }

  constexpr MaterialSurfaceType::Enum GetSurfaceType() const { return m_SurfaceType; }
  constexpr void SetSurfaceType(const MaterialSurfaceType::Enum type) { m_SurfaceType = type; }

 protected:
  Vector4 m_Color = YEAGER_MATERIAL_DEFAULT_COLOR;
  MaterialType::Enum m_MaterialType = MaterialType::eUNDEFINED;
  MaterialSurfaceType::Enum m_SurfaceType = MaterialSurfaceType::eUNDEFINED;
};

class MaterialTexture2D : public MaterialBase {
 public:
  MaterialTexture2D(Yeager::ApplicationCore* application, const String& name, const MaterialTextureType::Enum texture);
  MaterialTexture2D()  // Dont use this shit
      : MaterialBase(YEAGER_NULLPTR, YEAGER_NULL_LITERAL, MaterialType::eTEXTURE2D, MaterialSurfaceType::eTEXTURED){};
  ~MaterialTexture2D();

  MaterialTextureDataHandle* GetTextureDataHandle() { return &m_TextureHandle; }

  constexpr int GetWidth() const { return m_TextureHandle.Width; }
  constexpr int GetHeight() const { return m_TextureHandle.Height; }

  constexpr GLuint GetTextureID() const { return m_TextureHandle.Texture; }
  constexpr void SetTextureID(const GLuint& id) { m_TextureHandle.Texture = id; }

  String GetPath() const { return m_TextureHandle.Path; }
  void SetPath(String& path) { m_TextureHandle.Path = path; }

  constexpr bool IsGenerated() const { return m_TextureHandle.Generated; }
  constexpr void SetIsGenerated(bool gen) { m_TextureHandle.Generated = gen; }

  constexpr MaterialTextureType::Enum GetTextureType() const { return m_TextureType; }
  constexpr void SetTextureType(const MaterialTextureType::Enum type) { m_TextureType = type; }

  constexpr bool IsFlipped() const { return m_TextureHandle.Flipped; }

  /* The format returned is a OpenGL macro defined as unsigned 32bit int, 
  you need to test the value returned against the possibles values avaliable*/
  constexpr uint32_t GetFormat() const { return m_TextureHandle.Format; }

  virtual bool GenerateFromFile(const String& path, bool flip = false,
                                const MateriaTextureParameterGL parameteri =
                                    MateriaTextureParameterGL(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT, GL_REPEAT, 0,
                                                              GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR));

  void GenerateFromData(STBIDataOutput* output,
                        const MateriaTextureParameterGL parameteri = MateriaTextureParameterGL(
                            GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT, GL_REPEAT, 0, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR));

  bool GenerateCubeMapFromFile(const std::vector<String>& paths, bool flip = false,
                               const MateriaTextureParameterGL parameteri =
                                   MateriaTextureParameterGL(GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
                                                             GL_CLAMP_TO_EDGE, 0, GL_LINEAR, GL_LINEAR));

 protected:
  virtual void GenerateTextureParameter(const MateriaTextureParameterGL& parameter);
  MaterialTextureType::Enum m_TextureType = MaterialTextureType::eUNDEFINED;
  MaterialTextureDataHandle m_TextureHandle;
};

}  // namespace Yeager