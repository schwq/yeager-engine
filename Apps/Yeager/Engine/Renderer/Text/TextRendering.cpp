#include "TextRendering.h"
#include "../../../Application.h"
using namespace Yeager;

TextRenderer::TextRenderer(Yeager::ApplicationCore* application) : m_Application(application)
{
  Initialize();
}

void TextRenderer::Initialize()
{
  if (FT_Init_FreeType(&m_FTLibrary)) {
    Yeager::Log(ERROR, "FreeType library cannot initialize!");
  }
}

void TextRenderer::LoadFont(const String& path, Uint size)
{
  if (FT_New_Face(m_FTLibrary, path.c_str(), 0, &m_FTFace)) {
    Yeager::Log(ERROR, "Cannot load font into FreeType! {}", path);
  }
  FT_Set_Pixel_Sizes(m_FTFace, 0, size);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (Uint ch = 0; ch < 128; ch++) {
    if (FT_Load_Char(m_FTFace, ch, FT_LOAD_RENDER)) {
      Yeager::LogDebug(ERROR, "Cannot load Glyph {} in Face {}", ch, m_FTFace->face_index);
      continue;
    }

    Uint texture = -1;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_FTFace->glyph->bitmap.width, m_FTFace->glyph->bitmap.rows, 0, GL_RED,
                 GL_UNSIGNED_BYTE, m_FTFace->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {texture, IVector2(m_FTFace->glyph->bitmap.width, m_FTFace->glyph->bitmap.rows),
                           IVector2(m_FTFace->glyph->bitmap_left, m_FTFace->glyph->bitmap_top),
                           m_FTFace->glyph->advance.x};
    m_Characters.insert(std::pair<char, Character>(ch, character));
  }

  FT_Done_Face(m_FTFace);
  FT_Done_FreeType(m_FTLibrary);
  BuildBuffers();
}

void TextRenderer::BuildBuffers()
{
  glGenVertexArrays(1, &m_Vao);
  glGenBuffers(1, &m_Vbo);
  glBindVertexArray(m_Vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

std::pair<Uint, Text2D> TextRenderer::AddText(Text2D& text)
{
  const Uint index = m_TextIndexes++;
  const std::pair<Uint, Text2D> result(index, text);
  m_Texts.insert(result);
  return result;
}

void TextRenderer::RenderText(Yeager::Shader* shader, Transformation& transformation, const String& text, float x,
                              float y, float scale, const Vector3& color)
{
  shader->UseShader();
  shader->SetVec3("textColor", color);

  ApplyTransformation(&transformation);
  shader->SetMat4("model", transformation.model);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_Vao);

  String::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = m_Characters[*c];
    float xpos = x + ch.Bearing.x * scale;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    x += (ch.Advance >> 6) * scale;
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(Yeager::Shader* shader, const String& text, float x, float y, float scale,
                              const Vector3& color)
{

  const Vector2 size = m_Application->GetWindow()->GetWindowInformationPtr()->EditorSize;
  const Matrix4 proj = glm::ortho(0.0f, size.x, 0.0f, size.y);

  shader->UseShader();
  shader->SetVec3("textColor", color);
  shader->SetMat4("projection", proj);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_Vao);

  String::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = m_Characters[*c];
    float xpos = x + ch.Bearing.x * scale;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    x += (ch.Advance >> 6) * scale;
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

TextRenderer::~TextRenderer() {}