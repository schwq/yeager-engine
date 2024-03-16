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
/*
#pragma once

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../Renderer/Entity.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class Yeager::GameEntity;

namespace Yeager {
class VideoPlayer : public GameEntity {
 public:
  VideoPlayer(const 
  
  & name, const YgString& video_path);
  ~VideoPlayer();

  void Play();
  void Pause();
  void Resume();
  void Stop();
  Yeager::Texture2D* GetGLTexture();

 private:
  bool ReadFrame();

  Yeager::Texture2D* m_gl_texture = YEAGER_NULLPTR;
  AVFormatContext* m_format_ctx = YEAGER_NULLPTR;
  AVCodec* m_codec = YEAGER_NULLPTR;
  AVCodecContext* m_codec_ctx = YEAGER_NULLPTR;
  AVCodecParameters* m_codec_par = YEAGER_NULLPTR;
  AVFrame* m_frame = YEAGER_NULLPTR;
  AVPacket* m_packet = YEAGER_NULLPTR;
  const YgString m_video_path;
};
}  // namespace Yeager
*/
