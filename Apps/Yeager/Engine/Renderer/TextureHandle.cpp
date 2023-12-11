#include "TextureHandle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../Libraries/stb_image.h"

using namespace Yeager;

unsigned int Texture2D::m_TextureEngineCount = 0;

unsigned int LoadTextureCubeMap(std::vector<YgString> paths, bool flip)
{

  stbi_set_flip_vertically_on_load(flip);
  GLuint id = 0;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  int width, height, channels;
  for (unsigned int i = 0; i < paths.size(); i++) {
    unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);

    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

      stbi_image_free(data);
    } else {
      Yeager::Log(ERROR, "Cannot read data from skybox file {}", paths[i].c_str());
      stbi_image_free(data);
      return 0;
    }
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return id;
}

Texture2D::~Texture2D()
{
  glDeleteTextures(1, &m_OpenGL_ID);
}

unsigned int LoadTextureFromFile(YgString path, bool flip)
{
  stbi_set_flip_vertically_on_load(flip);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format = GL_RGB;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {

    stbi_image_free(data);
  }
  glBindTexture(GL_TEXTURE_2D, NULL);
  return textureID;
}

bool LoadTextureFromFile(YgCchar filename, GLuint* out_texture, int* out_width, int* out_height)
{
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);                                    // This is required on WebGL for non
                                                                        // power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  // Same

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

void Texture2D::GenerateTexture()
{
  glGenTextures(1, &m_OpenGL_ID);
  glBindTexture(GL_TEXTURE_2D, m_OpenGL_ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::ReadDataToTexture(YgCchar path)
{
  int channels;
  unsigned int format;
  unsigned char* data = stbi_load(path, &TextureWidth, &TextureHeight, &channels, 0);

  if (data) {
    if (channels == 1) {
      format = GL_RED;
    } else if (channels == 3) {
      format = GL_RGB;
    } else if (channels == 4) {
      format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, TextureWidth, TextureHeight, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    Yeager::Log(INFO, "Create texture: {}, ID: {}", m_Name.c_str(), m_TextureNumber);
    stbi_image_free(data);
  } else {
    Yeager::Log(ERROR, "Cannot read data from texture file {}", path);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(YgCchar texturePath, YgString name) : m_Name(name)
{
  m_TextureNumber = m_TextureEngineCount++;

  if (name == "DEFAULT") {
    name += std::to_string(m_TextureEngineCount);
  }

  stbi_set_flip_vertically_on_load(false);

  GenerateTexture();
  ReadDataToTexture(texturePath);
}
