#include "TextureCache.h"
#include "Common/FS/FileUtils.h"
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
  return reinterpret_cast<TextureCacheHeader*>(m_CurrentData);
}

void TextureCache::Free()
{
  if (m_Allocated) {
    BaseAllocator::Deallocate(m_CurrentData);
    m_Allocated = false;
  }
}

bool TextureCache::Load(const String& path, MaterialTexture2D* texture)
{
  if (!Yeager::ValidatesPath(path)) {
    Yeager::LogDebug(ERROR, "Given path {} is not valid!", path);
    return false;
  }

  BaseAllocator::AddIntervalOfMemChecking("Cache");

  FileHandle fp = Yeager::OpenFileR(path, std::ios::out | std::ios::binary);

  if (!fp.bValid) {
    Yeager::LogDebug(ERROR, "Cannot open file {} for texture caching!", path);
    return false;
  }

  unsigned char* data = BaseAllocator::Allocate<unsigned char>(fp.mSize);

  fp.mFile.read(reinterpret_cast<char*>(data), fp.mSize);

  TextureCacheHeader* header = (TextureCacheHeader*)data;

  if (BaseAllocator::Memcmp(header->MagicConst, YEAGER_CACHE_MAGIC_CONST, sizeof(char) * 4) == 0) {
    Yeager::LogDebug(INFO, "Is a cache file! {}", path);
  } else {
    Yeager::LogDebug(ERROR, "Given file {}, is not a valid texture cache file!", path);
    Yeager::CloseFile(fp);
    BaseAllocator::Deallocate(data);
  }

  GLuint* image = BaseAllocator::Allocate<GLuint>(header->FileSize);

  BaseAllocator::Memcpy(image, data + sizeof(TextureCacheHeader), header->FileSize);

  TextureCacheData cdata = {.mHeader = header, .mData = (unsigned char*)image, .mPath = path};

  texture->GenerateFromCacheData(&cdata);

  Yeager::CloseFile(fp);
  BaseAllocator::Deallocate(data);
  BaseAllocator::Deallocate(image);

  BaseAllocator::EndIntervalOfMemChecking("Cache");

  return true;
}

bool TextureCache::Create(const String& folder, Yeager::MaterialTexture2D& texture)
{
  if (!ValidatesPath(folder)) {
    Yeager::LogDebug(ERROR, "Path to folder given is invalid for texture caching! Path: {}", folder);
    return false;
  }

  if (!texture.IsGenerated()) {
    Yeager::LogDebug(ERROR, "Cannot generate cache from ungenerated texture! {}", texture.GetName());
    return false;
  }

  if (!FormatToChannels(texture.GetFormat()).has_value()) {
    Yeager::LogDebug(ERROR, "Invalid format from texture {}, cannot create cache!", texture.GetName());
    return false;
  }

  const String filename =
      std::to_string(CreateFileHash(String(texture.GetPath()))) + String(YEAGER_TEXTURE_CACHE_EXT_STR);

  const String path = folder + YG_PS + filename;

  Yeager::LogDebug(INFO, "Writing texture cache {} to path {}", texture.GetName(), path);

  size_t alloc_size =
      sizeof(TextureCacheHeader) +
      ((texture.GetWidth() * texture.GetHeight() * FormatToChannels(texture.GetFormat()).value()) * sizeof(GLuint));

  m_CurrentData = BaseAllocator::Allocate<unsigned char>(alloc_size);

  m_Allocated = true;

  TextureCacheHeader* header = (TextureCacheHeader*)(m_CurrentData);

  header->Width = static_cast<uint16_t>(texture.GetWidth());
  header->Height = static_cast<uint16_t>(texture.GetHeight());
  header->Format = static_cast<uint16_t>(texture.GetFormat());
  header->FileSize = static_cast<uint64_t>(alloc_size - sizeof(TextureCacheHeader));

  BaseAllocator::Memcpy(header->MagicConst, YEAGER_CACHE_MAGIC_CONST, sizeof(char) * 4);

  std::time_t epoch = static_cast<uint64_t>(std::time(nullptr));
  BaseAllocator::Memcpy(&header->Timestamp, &epoch, sizeof(uint64_t));

  unsigned char* data_p = (m_CurrentData) + sizeof(TextureCacheHeader);

  const size_t data_size = alloc_size - sizeof(TextureCacheHeader);

  GLuint* bytes = BaseAllocator::Allocate<GLuint>(data_size);

  glBindTexture(texture.GetTextureDataHandle()->BindTarget, texture.GetTextureID());

  glGetTexImage(texture.GetTextureDataHandle()->BindTarget, NULL, texture.GetTextureDataHandle()->Format,
                GL_UNSIGNED_INT, bytes);

  glBindTexture(texture.GetTextureDataHandle()->BindTarget, NULL);

  if (!bytes) {
    Yeager::LogDebug(ERROR, "Bytes pointer retrieved from glGetTexImage is empty!");
  }

  BaseAllocator::Memcpy(data_p, bytes, data_size);

  FileHandle output = Yeager::OpenFileW(path, std::ios::out | std::ios::binary);

  output.mFile.write((char*)m_CurrentData, alloc_size);

  Yeager::CloseFile(output);

  Free();
  delete bytes;

  Yeager::LogDebug(INFO, "Done caching texture {}", texture.GetName());

  return true;
}