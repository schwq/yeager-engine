#pragma once

#include "../../common/common.h"
#include "../../application.h"

class EngineTexture2D {
  public:
    EngineTexture2D(const char *texturePath, String name = "DEFAULT");
    ~EngineTexture2D();

    GLuint GetID() { return m_id; }

  private:
    void GenerateTexture();
    void ReadDataToTexture(const char *path);

    GLuint m_id;
    String m_name;
    uint m_texture_num;
    static uint m_texture_count;
};