#include "Cache.h"
#include "Main/Core/Application.h"
using namespace Yeager;

size_t Yeager::CreateFileHash(const String& name)
{
  std::hash<String> hasher;
  return hasher(name);
}
TextureCache::TextureCache(Yeager::ApplicationCore* application) : m_Application(application) {}

std::optional<TextureCacheHeader*> TextureCache::GetDataHeader()
{
  if (!m_Allocated) {
    Yeager::LogDebug(WARNING, "Trying to get texture header with unallocated data!");
    return std::nullopt;
  }
  return reinterpret_cast<TextureCacheHeader*>(m_CurrentData.get());
}

void TextureCache::Free()
{
  if (m_Allocated) {
    m_CurrentData.reset();
    m_Allocated = false;
  }
}

void TextureCache::Create(const String& path) {}
void TextureCache::Create(unsigned char* data, uint16_t format, uint16_t width, uint16_t height) {}

void TextureCache::Create(Yeager::MaterialTexture2D& texture)
{
  if (!texture.IsGenerated()) {
    Yeager::LogDebug(ERROR, "Cannot generate cache from ungenerated texture! {}", texture.GetName());
    return;
  }

  if (!FormatToChannels(texture.GetFormat()).has_value()) {
    Yeager::LogDebug(ERROR, "Invalid format from texture {}, cannot create cache!", texture.GetName());
    return;
  }

  const String filename =
      std::to_string(CreateFileHash(String(texture.GetPath()))) + String(YEAGER_TEXTURE_CACHE_EXT_STR);
  const String path = m_Application->GetScene()->GetTextureCacheFolderPath() + YG_PS + filename;
  std::ofstream output(path, std::ios_base::binary);

  size_t alloc_size = sizeof(TextureCacheHeader) +
                      ((texture.GetWidth() * texture.GetHeight() * FormatToChannels(texture.GetFormat()).value()) *
                       sizeof(unsigned char));
  m_CurrentData =
      std::shared_ptr<unsigned char>(new unsigned char[alloc_size], [](unsigned char* mem) { delete[] mem; });

  m_Allocated = true;
  TextureCacheHeader* header = reinterpret_cast<TextureCacheHeader*>(m_CurrentData.get());

  std::memset(m_CurrentData.get(), NULL, sizeof(unsigned char) * alloc_size);

  header->Width = static_cast<uint16_t>(texture.GetWidth());
  header->Height = static_cast<uint16_t>(texture.GetHeight());
  header->Format = static_cast<uint16_t>(texture.GetFormat());
  header->FileSize = static_cast<uint64_t>(alloc_size - sizeof(TextureCacheHeader));

  std::memcpy(header->MagicConst, YEAGER_CACHE_MAGIC_CONST, sizeof(char) * 4);

  std::time_t epoch_timer = std::time(nullptr);
  header->Timestamp = static_cast<uint64_t>(epoch_timer);

  unsigned char* data_p = (m_CurrentData.get()) + sizeof(TextureCacheHeader);
  const size_t data_size = alloc_size - sizeof(TextureCacheHeader);

  const size_t size = texture.GetWidth() * texture.GetHeight() * FormatToChannels(texture.GetFormat()).value();
  GLuint* bytes = new GLuint[size];

  glBindTexture(texture.GetTextureDataHandle()->BindTarget, texture.GetTextureID());
  glGetTexImage(texture.GetTextureDataHandle()->BindTarget, NULL, texture.GetTextureDataHandle()->Format,
                GL_UNSIGNED_INT, bytes);
  glBindTexture(texture.GetTextureDataHandle()->BindTarget, NULL);

  Free();
  delete bytes;
}