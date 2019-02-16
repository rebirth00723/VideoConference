#include "MPlayer.h"
CYUVPlayer::CYUVPlayer()
{
	isInitializedYUVPlayer = false;
	internal_Init();
}
CYUVPlayer::~CYUVPlayer()
{
	if (window)
	{
		SDL_DestroyTexture(yuvTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
	if (event_thread)
		event_thread->join();
	SDL_Quit();
}
CYUVPlayer::CYUVPlayer(int width, int height, const char *title, bool v_sync, bool hasFullscreen, bool resizable, bool stretch, HWND hwnd)
{
	isInitializedYUVPlayer = false;
	internal_Init();
	setup(width, height, title, 5, v_sync, hasFullscreen, resizable, stretch, hwnd);
}
bool CYUVPlayer::setup(int width, int height, const char *title, int frame_buffer_size, bool v_sync, bool hasFullscreen, bool resizable, bool stretch, HWND hwnd)
{
	strcpy(this->title, title);
	window_width = width;
	window_height = height;
	this->hasFullscreen = hasFullscreen;
	this->hwnd = hwnd;
	this->hasVSync = v_sync;
	this->hasResization = resizable;
	this->hasStretch = stretch;
	yuv_que.setMaxSize(frame_buffer_size);
	return true;
}
bool CYUVPlayer::setupPresentingSize(int p_width, int p_height)
{
	std::unique_lock<std::mutex> lck(var_mtx);
	if (yuvTexture)
	{
		SDL_DestroyTexture(yuvTexture);
	}
	yuvTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, p_width, p_height);

	if (!yuvTexture)
	{
		av_log(0, AV_LOG_FATAL, "CYUVPlayer:Could not create SDL texture.");
		return false;
	}
	if (p_width <= window_width && p_height <= window_height)
	{
		presented_rect.x = window_width / 2 - p_width / 2;
		presented_rect.y = window_height / 2 - p_height / 2;
		presented_rect.w = p_width;
		presented_rect.h = p_height;
	}
	else if (hasStretch)
	{
		presented_rect.x = presented_rect.y = 0;
		presented_rect.w = window_width;
		presented_rect.h = window_height;
	}
	else
	{
		presented_rect.x = presented_rect.y = 0;
		presented_rect.w = p_width;
		presented_rect.h = p_height;
	}
	return true;
}

SDL_Window* CYUVPlayer::create()
{
	Uint32 flag;
	if (window_width == 0 || window_height == 0)
	{
		printf("CYUVPlayer:Haven't initialized yet so that function can't create SDL window!");
		return NULL;
	}
	if (!hwnd)
	{
		flag = hasFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
		flag |= hasResization ? SDL_WINDOW_RESIZABLE : 0;
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			window_width,
			window_height,
			flag);
	}
	else
	{
		window = SDL_CreateWindowFrom(hwnd);
	}
	if (!window)
	{
		av_log(0, AV_LOG_FATAL, "CYUVPlayer:Could not create SDL window.");
		return NULL;
	}

	flag = hasVSync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC :
		SDL_RENDERER_ACCELERATED;

	renderer = SDL_CreateRenderer(window, -1, flag);

	if (!renderer)
	{
		av_log(0, AV_LOG_FATAL, "CYUVPlayer:Could not create SDL renderer.");
		return NULL;
	}

	if (!setupPresentingSize(window_width, window_height))
		return NULL;

	SDL_GetWindowWMInfo(window, &info);
	if (!hwnd)
		hwnd = info.info.win.window;

	mainThread = GetCurrentThreadId();

	printf("CYUVPlayer is up to work!\n");
	return window;
}

HWND CYUVPlayer::getHWND()
{
	return hwnd;
}

const SDL_Event* CYUVPlayer::handleEvent()
{
	int ret;
	if (ret = SDL_PollEvent(&displayModeEvent))
		event_handler(displayModeEvent);
	return ret ? &displayModeEvent : nullptr;
}

void CYUVPlayer::updateFrame()
{
	CPacketList<AVFrame*> *item;
	AVFrame *pFrameYUV;
	std::unique_lock<std::mutex> lck(var_mtx);
	if (isPlaying)
	{
		item = yuv_que.getItemNonBlocking();
		if (item)
		{
			pFrameYUV = item->data;
			SDL_UpdateYUVTexture(yuvTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0],
				pFrameYUV->data[1], pFrameYUV->linesize[1],
				pFrameYUV->data[2], pFrameYUV->linesize[2]
				);

			av_freep(pFrameYUV->data);
			av_frame_free(&pFrameYUV);
			delete item;
		}
	}
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, yuvTexture, NULL, &presented_rect);
	SDL_RenderPresent(renderer);
}

AppendResult CYUVPlayer::appendYUVData(uint8_t *yuv_raw)
{
	AVFrame *pFrameYUV;
	if (isPlaying)
	{
		pFrameYUV = CImageFormatConverter::makePicture(presented_rect.w, presented_rect.h, AV_PIX_FMT_YUV420P, yuv_raw);
		if (yuv_que.pushNonBlocking(pFrameYUV))
			return AR_SUCCEED;
		else
			return AR_QUEUE_IS_FULL;
	}
	else if (isTerminated)
		return AR_PLAYER_TERMINATED;
	else
		return AR_PLAYER_NOT_START;
}

AppendResult CYUVPlayer::appendYUVData(AVFrame *pFrameYUV)
{
	if (isPlaying)
	{
		if (yuv_que.pushNonBlocking(pFrameYUV))
			return AR_SUCCEED;
		else
			return AR_QUEUE_IS_FULL;
	}
	else if (isTerminated)
		return AR_PLAYER_TERMINATED;
	else
		return AR_PLAYER_NOT_START;
}

bool MAIN_THREAD_ONLY CYUVPlayer::show()
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	if (window)
		SDL_ShowWindow(window);
	return true;
}
bool MAIN_THREAD_ONLY CYUVPlayer::hide()
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	if (window)
		SDL_HideWindow(window);
	return true;
}

void CYUVPlayer::play()
{
	std::unique_lock<std::mutex> lck(var_mtx);
	isPlaying = true;
}

void CYUVPlayer::pause()
{
	std::unique_lock<std::mutex> lck(var_mtx);
	isPlaying = false;
}

void CYUVPlayer::reset()
{
	CPacketList<AVFrame*> *item;
	std::unique_lock<std::mutex> lck(var_mtx);
	isPlaying = false;
	isTerminated = false;
	yuv_que.break_getItem();
	yuv_que.reset();
	while (1)
	{
		item = yuv_que.flushOutItem();
		if (!item)
			break;
		av_frame_free(&(item->data));
		delete item;
	}
}
bool MAIN_THREAD_ONLY CYUVPlayer::move(int xOffset, int yOffset)
{
	SDL_Point p;
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SDL_GetWindowPosition(window, &p.x, &p.y);
	SDL_SetWindowPosition(window, p.x + xOffset, p.y + yOffset);
	return true;
}
bool MAIN_THREAD_ONLY CYUVPlayer::move(SDL_Point p)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SDL_SetWindowPosition(window, p.x, p.y);
	return true;
}

bool MAIN_THREAD_ONLY CYUVPlayer::getSize(int &w, int &h)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SDL_GetWindowSize(window, &w, &h);
	return true;
}

void CYUVPlayer::getPresentingSize(int &w, int &h)
{
	w = presented_rect.w;
	h = presented_rect.h;
}

bool MAIN_THREAD_ONLY CYUVPlayer::resize(int w, int h)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	window_width = w;
	window_height = h;
	presented_rect.w = w;
	presented_rect.h = h;
	SDL_SetWindowSize(window, w, h);
	return true;
}
bool MAIN_THREAD_ONLY CYUVPlayer::fullscreen()
{
	SIZE s = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	resize(s.cx, s.cy);
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	return true;
}

bool MAIN_THREAD_ONLY CYUVPlayer::desktopWindow(int width, int height)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SDL_SetWindowFullscreen(window, 0);
	resize(width, height);
	return true;
}
bool CYUVPlayer::setTitle(const char *title)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SDL_SetWindowTitle(window, title);
	return true;
}
bool CYUVPlayer::setTitle(const wchar_t *title)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	SetWindowText(hwnd, title);
	return true;
}
void CYUVPlayer::destroy()
{
	if (window)
	{
		SDL_DestroyTexture(yuvTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		window = NULL;
	}
}
void CYUVPlayer::setStretch(bool enable, int srcW, int srcH)
{
	hasStretch = enable;
	setupPresentingSize(srcW, srcH);
}
void CYUVPlayer::setViewPos(int x, int y)
{
	presented_rect.x = x;
	presented_rect.y = y;
}
void CYUVPlayer::terminate()
{
	CPacketList<AVFrame*> *item;
	AVFrame *pFrameYUV;
	std::unique_lock<std::mutex> lck(var_mtx);
	isTerminated = true;
	isPlaying = false;
	while (item = yuv_que.getItemNonBlocking())
	{
		pFrameYUV = item->data;
		av_freep(pFrameYUV->data);
		av_frame_free(&pFrameYUV);
		delete item;
	}
}
bool  CYUVPlayer::terminated() const
{
	return isTerminated;
}
bool CYUVPlayer::setPos(int x, int y)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	if (window)
		SDL_SetWindowPosition(window, x, y);
	return true;
}
bool CYUVPlayer::getPos(int *x, int *y)
{
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	if (window)
		SDL_GetWindowPosition(window, x, y);
	return true;
}
bool CYUVPlayer::startWaitEventLoop()
{
	SDL_Event event;
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	while (!isTerminated)
	{
		if (SDL_WaitEvent(&event))
		{
			event_handler(event);
		}
		updateFrame();
	}
	printf("CYUVPlayer:startWaitEventLoop finished.");
	return true;
}
bool CYUVPlayer::startPollEventLoop()
{
	SDL_Event event;
	if (GetCurrentThreadId() != mainThread)
	{
		printf("CYUVPlayer:You invoked a function that is main thread only.");
		return false;
	}
	while (!isTerminated)
	{
		if (SDL_PollEvent(&event))
			event_handler(event);
		updateFrame();
	}
	printf("CYUVPlayer:startPollEventLoop finished.");
	return true;
}

void CYUVPlayer::showCursor(bool s)
{
	SDL_ShowCursor(s);
}

void CYUVPlayer::setMouseEvent(void(*mouse_func)(int x, int y, const char *type, void *userData), void *userData)
{
	this->callback_mouse_func = mouse_func;
	this->callback_mouse_userdata = userData;
}

void CYUVPlayer::setKeyBoardEvent(void(*keyboard_func)(const char *type, SDL_Keycode key, void *userData), void *userData)
{
	this->callback_keyboard_func = keyboard_func;
	this->callback_keyboard_userdata = userData;
}

void CYUVPlayer::setWindowEvent(void(*window_func)(const char *type, void *userData), void *userData)
{
	this->callback_window_func = window_func;
	this->callback_window_userdata = userData;
}

bool CYUVPlayer::internal_Init()
{
	if (!isInitializedYUVPlayer)
	{
		window_width = window_height = 0;
		window = NULL;
		renderer = NULL;
		yuvTexture = NULL;
		event_thread = NULL;
		callback_mouse_func = NULL;
		callback_window_func = NULL;
		callback_keyboard_func = NULL;
		callback_mouse_userdata = NULL;
		callback_keyboard_userdata = NULL;
		callback_window_userdata = NULL;
		isTerminated = false;
		isPlaying = false;
		hasFullscreen = false;
		hasVSync = false;
		hwnd = 0;
		mainThread = 0;
		memset(&presented_rect, 0, sizeof(presented_rect));
		memset(title, 0, sizeof(title));
	}
	isInitializedYUVPlayer = true;
	return true;
}
void CYUVPlayer::event_handler(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		if (callback_mouse_func)
			callback_mouse_func(event.button.x, event.button.y, "SDL_MOUSEWHEEL", callback_mouse_userdata);
		break;
	case SDL_MOUSEMOTION:
		if (callback_mouse_func)
			callback_mouse_func(event.button.x, event.button.y, "SDL_MOUSEMOTION", callback_mouse_userdata);
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (callback_mouse_func)
			callback_mouse_func(event.button.x, event.button.y, "SDL_MOUSEBUTTONDOWN", callback_mouse_userdata);
		break;
	case SDL_MOUSEBUTTONUP:
		if (callback_mouse_func)
			callback_mouse_func(event.button.x, event.button.y, "SDL_MOUSEBUTTONUP", callback_mouse_userdata);
		break;
	case SDL_KEYDOWN:
		if (callback_keyboard_func)
			callback_keyboard_func("SDL_KEYDOWN", event.key.keysym.sym, callback_keyboard_userdata);
		break;
	case SDL_KEYUP:
		if (callback_keyboard_func)
			callback_keyboard_func("SDL_KEYUP", event.key.keysym.sym, callback_keyboard_userdata);
		break;
	case SDL_WINDOWEVENT:
		switch (event.window.event)
		{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_SIZE_CHANGED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_EXPOSED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_ENTER:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_ENTER", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_LEAVE:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_LEAVE", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_FOCUS_GAINED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_FOCUS_LOST", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_MINIMIZED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_MAXIMIZED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_RESTORED:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_RESTORED", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_HIDDEN", callback_window_userdata);
			break;
		case SDL_WINDOWEVENT_SHOWN:
			if (callback_window_func)
				callback_window_func("SDL_WINDOWEVENT_SHOWN", callback_window_userdata);
			break;
		}
		break;
	case SDL_QUIT:
		isTerminated = true;
		pause();
		reset();
		break;
	}
}
//PCM player
CPCMPlayer::CPCMPlayer()
{
	isInitializedPCMPlayer = false;
	internal_Init();
}
CPCMPlayer::~CPCMPlayer()
{
	if (stream)
		Pa_CloseStream(stream);
	Pa_Terminate();
}
CPCMPlayer::CPCMPlayer(int channels, int sample_rate, int frames_per_buffer)
{
	isInitializedPCMPlayer = false;
	internal_Init();
	setupPCMPlayer(channels, sample_rate, frames_per_buffer);
}

void CPCMPlayer::appendPCMData(uint8_t *pcm_raw, int raw_length)
{
	int nb_samples = (raw_length >> 1) / pcm_channels;
	Pa_WriteStream(stream, pcm_raw, nb_samples);
}

bool CPCMPlayer::setupPCMPlayer(int channels, int sample_rate, int frames_per_buffer)
{
	PaError err;
	err = Pa_Initialize();
	pcm_channels = channels;
	pcm_sample_rate = sample_rate;
	pcm_frames_per_buffer = frames_per_buffer;
	if (err != paNoError)
	{
		printf("CPCMPlayer:Could not initialize PortAudio library.\n");
		return false;
	}
	err = Pa_OpenDefaultStream(&stream, 0, channels, paInt16, sample_rate,
		frames_per_buffer, NULL, NULL);
	if (err != paNoError)
	{
		printf("CPCMPlayer:Could not create stream.\n");
		return false;
	}
	err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		printf("CPCMPlayer:Could not start stream.\n");
		return false;
	}
	printf("CPCMPlayer is up to work!\n");
	return true;
}
bool CPCMPlayer::isStopped()
{
	PaError err;
	err = Pa_IsStreamStopped(stream);
	if (err == 1)
		return true;
	else if (err == 0)
		return false;
	else
	{
		printf("CPCMPlayer:Could not determine stream status.");
		return false;
	}
}
void CPCMPlayer::internal_Init()
{
	if (!isInitializedPCMPlayer)
	{
		stream = NULL;
		pcm_channels = pcm_sample_rate = pcm_frames_per_buffer = 0;
	}
	isInitializedPCMPlayer = true;
}
CMediaPlayer::CMediaPlayer()
{

}
CMediaPlayer::CMediaPlayer(int video_width, int video_height, const char *title,
	int audio_channels, int audio_sample_rate, int audio_frames_per_buffer,
	int video_frames_buffer, bool video_vertical_sync)
{
	setupMediaPlayer(video_width, video_height, title, audio_channels,
		audio_sample_rate, audio_frames_per_buffer, video_frames_buffer,
		video_vertical_sync);
}
CMediaPlayer::~CMediaPlayer()
{

}
void CMediaPlayer::setupMediaPlayer(int video_width, int video_height, const char *title,
	int audio_channels, int audio_sample_rate, int audio_frames_per_buffer,
	int video_frames_buffer, bool video_vertical_sync)
{
	this->setup(video_width, video_height, title, video_frames_buffer, video_vertical_sync);
	this->setupPCMPlayer(audio_channels, audio_sample_rate, audio_frames_per_buffer);
}