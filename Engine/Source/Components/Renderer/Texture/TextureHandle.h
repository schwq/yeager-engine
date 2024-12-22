//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"

#include "Components/Kernel/Caching/TextureCache.h"
#include "Components/Renderer/Objects/Entity.h"
#include "Components/Renderer/Shader/ShaderHandle.h"

namespace Yeager {

class ApplicationCore;
struct TextureCacheHeader;

/* Materials are the process in which, objects gain some color, that can be a defined color by the user, a physical material, or a texture (even multiple textures combined) */
struct MaterialType {
  enum Enum { eTEXTURE2D, eMATERIAL_BASE, eSKYBOX, eCUBEMAP, ePHYSICAL_TEXTURE2D, eUNDEFINED };
  static String ToString(Enum type)
  {
    switch (type) {
      case eTEXTURE2D:
        return "Texture2D";
      case eMATERIAL_BASE:
        return "Material Base";
      case eSKYBOX:
        return "Skybox";
      case eCUBEMAP:
        return "Cubemap";
      case ePHYSICAL_TEXTURE2D:
        return "Physical Texture2D";
      case eUNDEFINED:
      default:
        return "Undefined";
    }
  }

  static Enum ToEnum(const String& str)
  {
    switch (StringToInterger(str.c_str())) {
      case StringToInterger("Texture2D"):
        return eTEXTURE2D;
      case StringToInterger("Material Base"):
        return eMATERIAL_BASE;
      case StringToInterger("Skybox"):
        return eSKYBOX;
      case StringToInterger("Cubemap"):
        return eSKYBOX;
      case StringToInterger("Physical Texture2D"):
        return ePHYSICAL_TEXTURE2D;
      case StringToInterger("Undefined"):
      default:
        return eUNDEFINED;
    }
  }
};

/* The surface type of a material if the kind of renderering is going to been apply to that material, for instance, 
a textured surface of a material is going to generated and handle a texture to the material */
struct MaterialSurfaceType {
  enum Enum { eTEXTURED, eCOLORED, eMATERIAL_DEFINED, eMULTI_TEXTURED, eUNDEFINED };
  static String ToString(Enum type)
  {
    switch (type) {
      case eTEXTURED:
        return "Textured";
      case eCOLORED:
        return "Colored";
      case eMATERIAL_DEFINED:
        return "Material Defined";
      case eMULTI_TEXTURED:
        return "MultiTextured";
      case eUNDEFINED:
      default:
        return "Undefined";
    }
  }
  static Enum ToEnum(const String& str)
  {
    switch (StringToInterger(str.c_str())) {
      case StringToInterger("Textured"):
        return eTEXTURED;
      case StringToInterger("Colored"):
        return eCOLORED;
      case StringToInterger("Material Defined"):
        return eMATERIAL_DEFINED;
      case StringToInterger("MultiTextured"):
        return eMULTI_TEXTURED;
      case StringToInterger("Undefined"):
      default:
        return eUNDEFINED;
    }
  }
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
    switch (StringToInterger(str.c_str())) {
      case StringToInterger("Diffuse"):
        return eDIFFUSE;
      case StringToInterger("Specular"):
        return eSPECULAR;
      case StringToInterger("Mettalic"):
        return eMETTALIC;
      case StringToInterger("NormalMap"):
        return eNORMAL_MAP;
      case StringToInterger("Roughness"):
        return eROUGHNESS;
      case StringToInterger("Undefined"):
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

/**
 * @brief STBI data refrers to the data generated by the stbi librarie of a image.
 * This is used when multithreading the generation of textures and images.
 * 
*/
struct STBIDataOutput {
  unsigned char* Data = YEAGER_NULLPTR;
  int Width = 0;
  int Height = 0;
  int NrComponents = 0;
  bool Flip = true;
  String OriginalPath = YEAGER_NULL_LITERAL;
};

struct TextureCacheData {
  TextureCacheHeader* mHeader;
  unsigned char* mData = YEAGER_NULLPTR;
  String mPath = YEAGER_NULL_LITERAL;
};

struct TextureHandleBase {
  GLuint Texture = -1;
  GLenum Format = GL_RGBA;
  GLuint BindTarget = GL_TEXTURE_2D;
  int Width = 0;
  int Height = 0;
};

struct MaterialTextureDataHandle : TextureHandleBase {
  MateriaTextureParameterGL Parameter;
  String Path = YEAGER_NULL_LITERAL;
  std::vector<String> CubeMapsPaths;
  bool Flipped = false;
  bool Generated = false;
  bool ImcompletedID = false;  // Multithreaded texture loaded, data is readed but there is still a id missing
};

extern GLenum ChannelsToFormat(const int channels);
extern std::optional<Uint> FormatToChannels(GLenum format);

class MaterialBase : public EditorEntity {
 public:
  MaterialBase(const EntityBuilder& builder, const MaterialType::Enum type, const MaterialSurfaceType::Enum surface);
  ~MaterialBase();

  YEAGER_CONSTEXPR Vector4 GetColor() const { return m_Color; }
  YEAGER_CONSTEXPR void SetColor(const Vector4& color) { m_Color = color; }

  virtual void ApplyToShader(Yeager::Shader* shader);

  YEAGER_CONSTEXPR MaterialType::Enum GetMaterialType() const { return m_MaterialType; }
  YEAGER_CONSTEXPR void SetMaterialType(const MaterialType::Enum material) { m_MaterialType = material; }

  YEAGER_CONSTEXPR MaterialSurfaceType::Enum GetSurfaceType() const { return m_SurfaceType; }
  YEAGER_CONSTEXPR void SetSurfaceType(const MaterialSurfaceType::Enum type) { m_SurfaceType = type; }

 protected:
  Vector4 m_Color = YEAGER_MATERIAL_DEFAULT_COLOR;
  MaterialType::Enum m_MaterialType = MaterialType::eUNDEFINED;
  MaterialSurfaceType::Enum m_SurfaceType = MaterialSurfaceType::eUNDEFINED;
};

class MaterialTexture2D : public MaterialBase {
 public:
  MaterialTexture2D(const EntityBuilder& builder, const MaterialTextureType::Enum texture);
  MaterialTexture2D()  // FIXME: this is shit, the applicationCore is set to nullptr that crashes the application! i need to review know to fix this mess
      : MaterialBase(EntityBuilder(YEAGER_NULLPTR, YEAGER_NULL_LITERAL), MaterialType::eTEXTURE2D,
                     MaterialSurfaceType::eTEXTURED){};
  ~MaterialTexture2D();

  MaterialTextureDataHandle* GetTextureDataHandle() { return &m_TextureHandle; }

  bool CreateCache(const String& path);
  bool LoadCache(const String& path);

  YEAGER_CONSTEXPR int GetWidth() const { return m_TextureHandle.Width; }
  YEAGER_CONSTEXPR int GetHeight() const { return m_TextureHandle.Height; }

  YEAGER_CONSTEXPR GLuint GetTextureID() const { return m_TextureHandle.Texture; }
  YEAGER_CONSTEXPR void SetTextureID(const GLuint& id) { m_TextureHandle.Texture = id; }

  String GetPath() const { return m_TextureHandle.Path; }
  void SetPath(String& path) { m_TextureHandle.Path = path; }

  YEAGER_CONSTEXPR bool IsGenerated() const { return m_TextureHandle.Generated; }
  YEAGER_CONSTEXPR void SetIsGenerated(bool gen) { m_TextureHandle.Generated = gen; }

  YEAGER_CONSTEXPR MaterialTextureType::Enum GetTextureType() const { return m_TextureType; }
  YEAGER_CONSTEXPR void SetTextureType(const MaterialTextureType::Enum type) { m_TextureType = type; }

  virtual void BindTexture();

  YEAGER_CONSTEXPR bool IsFlipped() const { return m_TextureHandle.Flipped; }

  /* The format returned is a OpenGL macro defined as unsigned 32bit int, 
  you need to test the value returned against the possibles values avaliable*/
  YEAGER_CONSTEXPR uint32_t GetFormat() const { return m_TextureHandle.Format; }

  virtual bool GenerateFromFile(const String& path, bool flip = false,
                                const MateriaTextureParameterGL parameteri =
                                    MateriaTextureParameterGL(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT, GL_REPEAT, 0,
                                                              GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR));

  void GenerateFromCacheData(TextureCacheData* data,
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
  /* Regenerate the texture handle with the same path and parameteri, but the flip setting is changed! 
  This is used when we load a model with the texture flipped to the wrong side, and this function fixs it!*/
  bool RegenerateTextureFlipped(bool flip = false);

  static void Unbind2DTextures();

 protected:
  virtual void GenerateTextureParameter(const MateriaTextureParameterGL& parameter);
  MaterialTextureType::Enum m_TextureType = MaterialTextureType::eUNDEFINED;
  MaterialTextureDataHandle m_TextureHandle;
};

extern void DisplayImageImGui(MaterialTexture2D* texture, Uint resize = 1);

}  // namespace Yeager