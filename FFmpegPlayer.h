#pragma once

#include "resource.h"
#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
};

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)
int thread_exit = 0;

int sfp_refresh_thread(void *opaque){
	thread_exit = 0;
	while (!thread_exit) {
		SDL_Event event;
		event.type = SFM_REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	thread_exit = 0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}

//结构体
class StreamState
{
public:
	AVFormatContext* pFormatCtx = NULL;
	AVCodecContext  *pCodecCtx;
	AVFrame         *pFrame ;
	AVStream        *st;
	AVFrame        *pFrameYUV ;
	AVCodec         *pCodec;
	int             videoIndex;
	uint8_t         *out_buffer;
	AVPacket        *packet;
	int             got_picture;
	struct SwsContext *img_convert_ctx;
	int             ret;
} ;

enum VideoStatus{
	PLAY,
	PAUSE,
	QUIT
};

class SDLPARM{
public:
	int           screen_w, screen_h;
	SDL_Window    *screen;
	SDL_Renderer  *sdlRenderer;
	SDL_Texture   *sdlTexture;
	SDL_Rect      sdlRect;
	SDL_Thread    *video_tid;
	SDL_Event     event;
};

StreamState *m_streamstate = new StreamState();
SDLPARM     *m_sdl = new SDLPARM();

class VideoProcess{
public:
	TCHAR      *szFileName;
	HWND       hVideo;
	VideoStatus  status = PAUSE;	//设置初始状态为暂停
	TCHAR     TotalTime[10]; //用于显示视频文件的总时长
	TCHAR     CurrentTime[10];  //用于显示当前播放时间
	//与视屏播放器相关的几个操作

	void Init();
	void OpenVideoFile(HWND hWnd, StreamState *m_streamstate, SDLPARM *m_sdl);
	void EventProc(StreamState *m_streamstate, SDLPARM *m_sdl);
	void GetVideoTime();
	void GetcurrentTime();
};

void VideoProcess::Init(){
	wsprintf(TotalTime, TEXT("%s"), TEXT("00:00:00"));
	wsprintf(CurrentTime, TEXT("%s"), TEXT("00:00:00"));
}

void VideoProcess::OpenVideoFile(HWND hWnd, StreamState *m_streamstate, SDLPARM *m_sdl) {
	//初始化
	av_register_all();
	avformat_network_init();
	m_streamstate->pFormatCtx = avformat_alloc_context();

	int iLength = WideCharToMultiByte(CP_ACP, 0, szFileName, -1, NULL, 0, NULL, NULL);
	char *filename = new char[iLength + 1];
	WideCharToMultiByte(CP_ACP, 0, szFileName, -1, filename, iLength, NULL, NULL);

	avformat_open_input(&m_streamstate->pFormatCtx, filename, NULL, NULL);
	avformat_find_stream_info(m_streamstate->pFormatCtx, NULL);

	m_streamstate->videoIndex = -1;
	int i;
	for (i = 0; i < m_streamstate->pFormatCtx->nb_streams; i++) {
		if (m_streamstate->pFormatCtx->streams[i]->codec->codec_type
			== AVMEDIA_TYPE_VIDEO) {
			m_streamstate->videoIndex = i;
			break;
		}
	}
	m_streamstate->pCodecCtx =
		m_streamstate->pFormatCtx->streams[m_streamstate->videoIndex]->codec;
	m_streamstate->pCodec = avcodec_find_decoder(m_streamstate->pCodecCtx->codec_id);
	avcodec_open2(m_streamstate->pCodecCtx, m_streamstate->pCodec, NULL);

	m_streamstate->pFrame = av_frame_alloc();
	m_streamstate->pFrameYUV = av_frame_alloc();

	m_streamstate->out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P,
		m_streamstate->pCodecCtx->width, m_streamstate->pCodecCtx->height));
	avpicture_fill((AVPicture *)m_streamstate->pFrameYUV, m_streamstate->out_buffer, PIX_FMT_YUV420P,
		m_streamstate->pCodecCtx->width, m_streamstate->pCodecCtx->height);

	m_streamstate->img_convert_ctx = sws_getContext(m_streamstate->pCodecCtx->width,
		m_streamstate->pCodecCtx->height, m_streamstate->pCodecCtx->pix_fmt,
		m_streamstate->pCodecCtx->width, m_streamstate->pCodecCtx->height,
		PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	m_sdl->screen_w = m_streamstate->pCodecCtx->width;
	m_sdl->screen_h = m_streamstate->pCodecCtx->height;

	m_sdl->screen = SDL_CreateWindowFrom(hVideo);

	m_sdl->sdlRenderer = SDL_CreateRenderer(m_sdl->screen, -1, 0);
	m_sdl->sdlTexture = SDL_CreateTexture(m_sdl->sdlRenderer, SDL_PIXELFORMAT_IYUV
		, SDL_TEXTUREACCESS_STREAMING, m_streamstate->pCodecCtx->width, m_streamstate->pCodecCtx->height);
	m_sdl->sdlRect.x = 0;
	m_sdl->sdlRect.y = 0;
	m_sdl->sdlRect.w = m_sdl->screen_w;
	m_sdl->sdlRect.h = m_sdl->screen_h;

	m_streamstate->packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	m_sdl->video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
}

void VideoProcess::EventProc(StreamState *m_streamstate, SDLPARM *m_sdl) {
	while (1) {
		SDL_WaitEvent(&m_sdl->event);
		if (m_sdl->event.type == SFM_REFRESH_EVENT && status == PLAY) {
			if (av_read_frame(m_streamstate->pFormatCtx, m_streamstate->packet) >= 0) {
				if (m_streamstate->packet->stream_index == m_streamstate->videoIndex) {
					m_streamstate->ret = avcodec_decode_video2(m_streamstate->pCodecCtx,
						m_streamstate->pFrame, &m_streamstate->got_picture, m_streamstate->packet);
					if (m_streamstate->ret < 0) {
						MessageBox(0, TEXT("error"), 0, MB_OK);
						break;
					}
					if (m_streamstate->got_picture) {
						sws_scale(m_streamstate->img_convert_ctx,
							(const uint8_t* const*)m_streamstate->pFrame->data, m_streamstate->pFrame->linesize,
							0, m_streamstate->pCodecCtx->height, m_streamstate->pFrameYUV->data, m_streamstate->pFrameYUV->linesize);
						SDL_UpdateTexture(m_sdl->sdlTexture, NULL, m_streamstate->pFrameYUV->data[0],
							m_streamstate->pFrameYUV->linesize[0]);
						SDL_RenderClear(m_sdl->sdlRenderer);
						SDL_RenderCopy(m_sdl->sdlRenderer, m_sdl->sdlTexture, NULL, NULL);
						SDL_RenderPresent(m_sdl->sdlRenderer);
						//GetcurrentTime();
					}
				}
			}
			av_free_packet(m_streamstate->packet);
		}
		else if (status == QUIT) {
			thread_exit = 1;
		}
		else if (status == PAUSE) {
			continue;
		}
	}

	sws_freeContext(m_streamstate->img_convert_ctx);
	SDL_Quit();
	av_frame_free(&m_streamstate->pFrameYUV);
	av_frame_free(&m_streamstate->pFrame);
	avcodec_close(m_streamstate->pCodecCtx);
	avformat_close_input(&m_streamstate->pFormatCtx);
}

void VideoProcess::GetVideoTime(){
	if (m_streamstate->pFormatCtx->duration != AV_NOPTS_VALUE){
		int hour, mins, secs, us;
		int  duration = m_streamstate->pFormatCtx->duration + 5000;
		secs = duration / AV_TIME_BASE;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hour = mins / 60;
		mins %= 60;
		TCHAR szBuffer[10] = TEXT("");

		wsprintf(TotalTime, TEXT("%d"), hour);
		lstrcat(TotalTime, TEXT(":"));
		wsprintf(szBuffer, TEXT("%d"), mins);
		lstrcat(TotalTime, szBuffer);
		lstrcat(TotalTime, TEXT(":"));
		wsprintf(szBuffer, TEXT("%d"), secs);
		lstrcat(TotalTime, szBuffer);
	}
}

void VideoProcess::GetcurrentTime(){
	int currenttime = m_streamstate->packet->pts;
	int hour, mins, secs, us;
	secs = currenttime / AV_TIME_BASE;
	us = currenttime % AV_TIME_BASE;
	mins = secs / 60;
	secs %= 60;
	hour = mins / 60;
	mins %= 60;
	TCHAR szBuffer[10] = TEXT("");

	wsprintf(CurrentTime, TEXT("%d"), hour);
	lstrcat(CurrentTime, TEXT(":"));
	wsprintf(szBuffer, TEXT("%d"), mins);
	lstrcat(CurrentTime, szBuffer);
	lstrcat(CurrentTime, TEXT(":"));
	wsprintf(szBuffer, TEXT("%d"), secs);
	lstrcat(CurrentTime, szBuffer);
}