#include "VideoPlayer.h"
using namespace Yeager;

VideoPlayer::VideoPlayer(const yg_string& name, const yg_string& video_path)
    : GameEntity(name), m_video_path(video_path)
{

  if (avformat_open_input(&m_format_ctx, video_path.c_str(), nullptr, nullptr) <
      0) {
    Yeager::Log(ERROR, kSystem,
                "VideoPlayer name {} ID {} cannot been created!", m_name, m_id);
    return;
  }

  if (avformat_find_stream_info(m_format_ctx, nullptr) < 0) {
    Yeager::Log(ERROR, kSystem,
                "VideoPlayer name {} ID {} cannot find stream info!", m_name,
                m_id);
    return;
  }

  int video_stream_idx = -1;
  for (yg_uint x = 0; x < m_format_ctx->nb_streams; x++) {
    if (m_format_ctx->streams[x]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_idx = x;
      break;
    }
  }
  if (video_stream_idx == -1) {
    Yeager::Log(ERROR, kSystem,
                "VideoPlayer name {} ID {} cannot find video stream!", m_name,
                m_id);
    return;
  }

  m_codec_par = m_format_ctx->streams[video_stream_idx]->codecpar;
  m_codec = const_cast<AVCodec*>(avcodec_find_decoder(m_codec_par->codec_id));
  m_codec_ctx = avcodec_alloc_context3(m_codec);
  avcodec_parameters_to_context(m_codec_ctx, m_codec_par);
  if (avcodec_open2(m_codec_ctx, m_codec, nullptr) < 0) {
    Yeager::Log(ERROR, kSystem, "VideoPlayer name {} ID {} cannot open codec!",
                m_name, m_id);
    return;
  }

  m_frame = av_frame_alloc();
  if (!m_frame) {
    Yeager::Log(ERROR, kSystem, "VideoPlayer name {} ID {} cannot alloc frame!",
                m_name, m_id);
    return;
  }
  m_packet = av_packet_alloc();
  if (!m_packet) {
    Yeager::Log(ERROR, kSystem,
                "VideoHandle name {} ID {} cannot alloc packet!", m_name, m_id);
    return;
  }
}
VideoPlayer::~VideoPlayer() {}
void VideoPlayer::Play() {}
void VideoPlayer::Pause() {}
void VideoPlayer::Resume() {}
void VideoPlayer::Stop() {}
Yeager::Texture2D* VideoPlayer::GetGLTexture()
{
  return nullptr;
}
bool VideoPlayer::ReadFrame() {}
