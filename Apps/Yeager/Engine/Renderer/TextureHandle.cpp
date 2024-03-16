#include "TextureHandle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Yeager;

MaterialBase::MaterialBase(Yeager::ApplicationCore* application, const String& name, const MaterialType::Enum type,
                           const MaterialSurfaceType::Enum surface)
    : EditorEntity(EntityObjectType::TEXTURE, application, name), m_MaterialType(type), m_SurfaceType(surface)
{
  Yeager::LogDebug(INFO, "Created Material Base {} ID {}!", m_Name, m_EntityID);
}

MaterialBase::~MaterialBase()
{
  Yeager::LogDebug(INFO, "Destrorying Material Base {} ID {}!", m_Name, m_EntityID);
}

void MaterialBase::ApplyToShader(Yeager::Shader* shader)
{
  shader->UseShader();
  shader->SetVec4("Color", m_Color);
}

MaterialTexture2D::MaterialTexture2D(Yeager::ApplicationCore* application, const String& name,
                                     const MaterialTextureType::Enum texture)
    : MaterialBase(application, name, MaterialType::eTEXTURE2D, MaterialSurfaceType::eTEXTURED), m_TextureType(texture)
{}

MaterialTexture2D::~MaterialTexture2D()
{
  if (m_TextureHandle.Generated)
    glDeleteTextures(1, &m_TextureHandle.Texture);
}

GLenum Yeager::ChannelsToFormat(const int channels)
{
  switch (channels) {
    case 1: {
      return GL_RED;
    } break;
    case 3: {
      return GL_RGB;
    } break;
    case 4: {
      return GL_RGBA;
    } break;
    default:
      return GL_RGB;
  }
}

void MaterialTexture2D::GenerateFromData(STBIDataOutput* output, const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a texture 2d!");

  m_MaterialType = MaterialType::eTEXTURE2D;
  m_TextureHandle.Parameter = parameteri;

  GenerateTextureParameter(parameteri);
  m_TextureHandle.Height = output->Height;
  m_TextureHandle.Width = output->Width;
  m_TextureHandle.Path = output->OriginalPath;
  m_TextureHandle.Format = ChannelsToFormat(output->NrComponents);

  glTexImage2D(parameteri.BindTarget, 0, m_TextureHandle.Format, output->Width, output->Height, 0,
               m_TextureHandle.Format, GL_UNSIGNED_BYTE, output->Data);
  glGenerateMipmap(parameteri.BindTarget);

  stbi_image_free(output->Data);

  glBindTexture(parameteri.BindTarget, 0);

  Yeager::LogDebug(INFO, "Created Material texture2D {} ID {}", m_Name, m_EntityID);
}

void MaterialTexture2D::GenerateTextureParameter(const MateriaTextureParameterGL& parameter)
{
  if (m_TextureHandle.Generated)
    glDeleteTextures(1, &m_TextureHandle.Texture);  // kill the current texture

  glGenTextures(1, &m_TextureHandle.Texture);
  glBindTexture(parameter.BindTarget, m_TextureHandle.Texture);

  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_S, parameter.WRAP_S);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_T, parameter.WRAP_T);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_WRAP_R, parameter.WRAP_R);
  glTexParameterf(parameter.BindTarget, GL_TEXTURE_BASE_LEVEL, parameter.BASE_LEVEL);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_MIN_FILTER, parameter.MIN_FILTER);
  glTexParameteri(parameter.BindTarget, GL_TEXTURE_MAG_FILTER, parameter.MAG_FILTER);
}

bool MaterialTexture2D::GenerateFromFile(const String& path, bool flip, const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a texture 2d!");

  /* Must reforce the correct type is set to the material */
  m_MaterialType = MaterialType::eTEXTURE2D;

  stbi_set_flip_vertically_on_load(flip);
  m_TextureHandle.Flipped = flip;
  m_TextureHandle.Parameter = parameteri;

  int channels = 0;
  unsigned char* data = stbi_load(path.c_str(), &m_TextureHandle.Width, &m_TextureHandle.Height, &channels, 0);

  if (data) {
    m_TextureHandle.Format = ChannelsToFormat(channels);

    GenerateTextureParameter(parameteri);

    glTexImage2D(parameteri.BindTarget, 0, m_TextureHandle.Format, m_TextureHandle.Width, m_TextureHandle.Height, 0,
                 m_TextureHandle.Format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(parameteri.BindTarget);

    m_TextureHandle.Path = path;
    m_TextureHandle.Generated = true;

    Yeager::LogDebug(INFO, "Created Material texture2D {} ID {}", m_Name, m_EntityID);

    stbi_image_free(data);

    glBindTexture(parameteri.BindTarget, 0);

    return true;

  } else {
    Yeager::LogDebug(ERROR, "Cannot read data from texture file {}, on {} ID {}", path, m_Name, m_EntityID);
  }
  return false;
}

bool MaterialTexture2D::GenerateCubeMapFromFile(const std::vector<String>& paths, bool flip,
                                                const MateriaTextureParameterGL parameteri)
{
  if (m_TextureHandle.Generated)
    Yeager::LogDebug(WARNING, "Material texture already generated! Overrided by a cubemap texture!");

  /* Must reforce the correct type is set to the material */
  m_MaterialType = MaterialType::eCUBEMAP;

  stbi_set_flip_vertically_on_load(flip);

  m_TextureHandle.Flipped = flip;
  m_TextureHandle.Parameter = parameteri;

  int channels = 0;
  bool successed = true;
  for (unsigned int i = 0; i < paths.size(); i++) {
    unsigned char* data = stbi_load(paths[i].c_str(), &m_TextureHandle.Width, &m_TextureHandle.Height, &channels, 0);

    if (data) {
      m_TextureHandle.Format = ChannelsToFormat(channels);

      GenerateTextureParameter(parameteri);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_TextureHandle.Format, m_TextureHandle.Width,
                   m_TextureHandle.Height, 0, m_TextureHandle.Format, GL_UNSIGNED_BYTE, data);

      glGenerateMipmap(parameteri.BindTarget);

      m_TextureHandle.CubeMapsPaths = paths;
      m_TextureHandle.Generated = true;

      stbi_image_free(data);

      Yeager::LogDebug(INFO, "Created Material texture cube map {} ID {}", m_Name, m_EntityID);

    } else {
      Yeager::LogDebug(ERROR, "Cannot read data from texture file {}, on {} ID {}", paths[i].c_str(), m_Name,
                       m_EntityID);
      successed = false;
    }
  }

  glBindTexture(parameteri.BindTarget, 0);
  return successed;
}