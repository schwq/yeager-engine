#include "texture.h"
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../../../../libs/stb_image.h"

uint EngineTexture2D::m_texture_count = 0;

EngineTexture2D::~EngineTexture2D() { glDeleteTextures(1, &m_id); }

void EngineTexture2D::GenerateTexture() {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void EngineTexture2D::ReadDataToTexture(const char *path) {
    int width, height, channels;
    uint format;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

    if (data) {
        if (channels == 1) {
            format = GL_RED;
        } else if (channels == 3) {
            format = GL_RGB;
        } else if (channels == 4) {
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        VLOG_F(INFO, "Create texture: %s, ID: %u", m_name.c_str(), m_texture_num);

        stbi_image_free(data);

    } else {
        VLOG_F(ERROR, "Cannot read data from texture file %s", path);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

EngineTexture2D::EngineTexture2D(const char *texturePath, String name) : m_name(name) {
    m_texture_num = m_texture_count++;

    if (name == "DEFAULT") {
        name += std::to_string(m_texture_count);
    }

    stbi_set_flip_vertically_on_load(false);

    GenerateTexture();
    ReadDataToTexture(texturePath);
}