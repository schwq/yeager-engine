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
#include "Common/Utils/Utilities.h"
#include "Components/Renderer/Texture/TextureHandle.h"

namespace Yeager {

class ApplicationCore;
class MaterialTexture2D;

#define YEAGER_TEXTURE_CACHE_EXT_STR ".ygen_tex_cache"
#define YEAGER_CACHE_MAGIC_CONST "YGEN"
/**
 * Texture cache header file (name_hash).ygen_tex_cache  
 * 4 Bytes - Magic const YGEN 
 * 8 Bytes - Timestamp 
 * 2 Bytes = Format 
 * 2 Bytes - Width
 * 2 Bytes - Height
 * 8 Byte - File size
 * Total size : 26 bytes 
 */

struct TextureCacheHeader {
  char MagicConst[4] = {0};
  uint64_t Timestamp = 0;
  uint16_t Format = 0;
  uint16_t Width = 0;
  uint16_t Height = 0;
  uint64_t FileSize = 0;
};

extern size_t CreateFileHash(const String& name);

class TextureCache {
 public:
  TextureCache(Yeager::ApplicationCore* application);
  ~TextureCache() {}

  virtual void Create(const String& path);
  virtual void Create(unsigned char* data, uint16_t format, uint16_t width, uint16_t height);
  virtual void Create(Yeager::MaterialTexture2D& texture);
  unsigned char* Load(const String& path);

  std::optional<TextureCacheHeader*> GetDataHeader();

  void Free();

 protected:
  bool m_Allocated = true;
  YEAGER_USING_SHARED_PTR
  SharedPtr<TextureCacheHeader> m_Header = YEAGER_NULLPTR;
  SharedPtr<unsigned char> m_CurrentData = YEAGER_NULLPTR;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
};

}  // namespace Yeager