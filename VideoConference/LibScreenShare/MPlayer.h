#ifndef MPLAYER_H
#define MPLAYER_H
#include "FFmpegBase.h"
#include "ImageUtils.h"
#include "CPacketQueue.h"
#include <portaudio.h>
#include <thread>
#include "SDLBase.h"
//This specifier is no side effect.
//Purpose of use is to indicate that function is main thread only.
#define MAIN_THREAD_ONLY 
#ifdef _MSC_VER
	#include <Windows.h>
#elif __GNUC__
	#include <SDL2/SDL.h>
#endif

/*
	Issue 2:
	We need to maintance the FPS of the yuvplayer (not just using presenting synchronization)
*/ 

/*
	There are two working modes of YUVPlayer 
	
	1.Individual Mode:

		In this mode, player processes with 
			startWaitEventLoop()
			startPollEventLoop()
		Working at this mode, player is able to set callback function.
		And maintanening frame display automatically.

	2.Display Mode:
		
		In this mode player processes with
			updateFrame()
			handleEvent()

		Working at this mode, there is no callback function can be used.
		every time you invoke updateFrame() player consumes one frame 
		from the buffering queue and shows on the window.

		And if you want to handle the event on this mode,
		you can handle that by invoking handleEvent().
		First of all you shall read the comments on handleEvent :)

	Usage of player 
		There are some steps of initialization to a player

		1.setup()
			Setup player properties and ready to create.
		
		2.create()
			Create window of player and ready to play.
		
		3.play()
			After invoking play() function, you are able to pass data to player.
		
		4.appendYUVData()
			Start to pass yuv raw data to player.
		
		5.startPollEventLoop()  <Individual Mode>
			Start message loop for player,
			And calling this function at main thread.

		5.updateFrame()         <Display Mode>
		  handleEvent()
			Start to update frame to the surface of player,
			And handle the event by yourself.

*/
typedef enum
{
	//Append yuv data successfully
	AR_SUCCEED,
	//Try to append yuv data to player, but player haven't started.
	AR_PLAYER_NOT_START,
	//Try to append yuv data to player, but queue has been full already.
	AR_QUEUE_IS_FULL,
	//Try to append yuv data to player, but it has terminated.
	AR_PLAYER_TERMINATED
}AppendResult;

class CYUVPlayer : public CSDLBase
{
public:
	CYUVPlayer();
	~CYUVPlayer();
	CYUVPlayer(int width, int height, const char *title = "CYUVPlayer", bool v_sync = true, bool hasFullscreen = false,bool resizable = false, bool stretch = false,HWND hwnd = NULL);
	/*
		---Usage---
		Calling this function to set properties of player.
	*/
	bool setup(int width, int height, const char *title, int frame_buffer_size = 5, bool v_sync = true, bool hasFullscreen = false, bool resizable = false, bool stretch = false, HWND hwnd = NULL);
	/*
		---Usage---
		This function will setup presenting size and also create SDL Texture (for rendering YUV surface)
		so you need to note if passed argument (p_width and p_height) isn't to match real size of YUV picture
		it would make updateFrame() function broken.
	*/
	bool						setupPresentingSize(int p_width, int p_height);
	/*
		---Usage---
		After calling setup(), invoking this functino to create a player instance to work.
	*/
	SDL_Window*					create();
	/*
		---Usage---
		this function would return a win32 native window handle.
	*/
	HWND						getHWND();
	/*
		---Usage---
		Calling this function would consume one event from OS message queue,
		and determine there is any event or not,
		if it's, then return a non-zero value pointer 
		if not, then return a zero value.

		---Warning---
		This is a non-blocking type function,
		It may occur CPU busy-waiting.
	*/
	const SDL_Event*			handleEvent();
	/*
		---Usage---
		Calling this function would consume one frame from buffering queue,
		and show it on surface.
	*/
	void						updateFrame();
	/*
		---Usage---
		Appending the yuv raw data to buffering data.
		Note that, the yuv_raw data should be allocated with av_malloc() function,
		because playing handler would free the frame data by av_freep() and av_frame_free().

		return value
			refer to declaration of AppendResult
	*/
	AppendResult 				appendYUVData(uint8_t *yuv_raw);
	/*
		---Usage---
		Appending the AVFrame to buffering data.

		return value
			refer to declaration of AppendResult
	*/
	AppendResult				appendYUVData(AVFrame *pFrameYUV);
	/*
		---Usage---
		Using this function to show the window.
	*/
	bool	MAIN_THREAD_ONLY	show();
	/*
		---Usage---
		Using this function to hide the window.
	*/
	bool	MAIN_THREAD_ONLY	hide();
	/*
		---Usage---
		Using this function to start playing. (let event_handler start to consume yuv raw data)
	*/
	void						play();
	/*
		---Usage---
		Using this function to pause playing.
	*/
	void						pause();
	/*	
		---Usage---
		Using this function to finish playing.
		and restart player.
	*/
	void						reset();
	/*
		---Important---
		Using this function to finish the player.
		Note that, if you invoking this function the player will be no longer to use.
	*/
	void						destroy();
	/*		
		---Usage---
		Using this function to terminate player generally.	
	*/
	void						setStretch(bool enable,int srcW,int srcH);
	void						setViewPos(int x, int y);
	void						terminate();
	bool						terminated() const;
	bool MAIN_THREAD_ONLY	    move(int xOffset = 0, int yOffset = 0);
	bool MAIN_THREAD_ONLY		move(SDL_Point p);
	bool MAIN_THREAD_ONLY       getSize(int &w, int &h);
	void						getPresentingSize(int &w, int &h);
	bool MAIN_THREAD_ONLY       resize(int w, int h);
	/*
		---Usage---
		Calling this function let player enter fullscreen.
	*/
	bool MAIN_THREAD_ONLY		fullscreen();
	/*
		---Usage---
		Calling this function let player return to window.
	*/
	bool MAIN_THREAD_ONLY		desktopWindow(int width,int height);
	bool MAIN_THREAD_ONLY       setTitle(const char *title);
	bool MAIN_THREAD_ONLY		setTitle(const wchar_t *title);
	bool MAIN_THREAD_ONLY		setPos(int x, int y);
	bool MAIN_THREAD_ONLY       getPos(int *x, int *y);
	/*
		---Usage---
		This function is general use for message loop
		You can picture this as GetMessage while programming on Windows API 
	*/
	bool MAIN_THREAD_ONLY       startWaitEventLoop();
	/*
		---Usage---
		This function is general use for message loop
		You can picture this as PeekMessage while programming on Windows API
	*/
	bool MAIN_THREAD_ONLY		startPollEventLoop();
	void						showCursor(bool s);
	void						setMouseEvent(void(*mouse_func)(int x, int y, const char *type, void *userData),void *userData = NULL);
	void						setKeyBoardEvent(void(*keyboard_func)(const char *type, SDL_Keycode key, void *userData), void *userData = NULL);
	void						setWindowEvent(void(*window_func)(const char *type,void *userData),void *userData);
private:
	bool						internal_Init();
	void						event_handler(SDL_Event event);
	void					   *callback_mouse_userdata;
	void					   *callback_keyboard_userdata;
	void					   *callback_window_userdata;
	void					  (*callback_mouse_func)(int x, int y, const char *type, void *userData);
	void					  (*callback_keyboard_func)(const char *type, SDL_Keycode key, void *userData);
	void					  (*callback_window_func)(const char *type, void *userData);
	CPacketQueue<AVFrame*>      yuv_que;
	std::thread				   *event_thread;	
	std::mutex					var_mtx;
	SDL_Window				   *window;
	SDL_Renderer			   *renderer;
	SDL_Texture				   *yuvTexture;
	SDL_Rect				    presented_rect;
	HWND						hwnd;
	DWORD						mainThread;
	SDL_Event					displayModeEvent;
	char						title[1024];
	int							window_width;
	int							window_height;
	bool						hasFullscreen;
	bool						hasStretch;
	bool						hasResization;
	bool						hasVSync;
	bool						isPlaying;
	bool						isInitializedYUVPlayer;
	bool						isTerminated;
};
class CPCMPlayer
{
public:
	CPCMPlayer();
	~CPCMPlayer();
	CPCMPlayer(int channels, int sample_rate, int frames_per_buffer = 1024);
	bool			setupPCMPlayer(int channels, int sample_rate, int frames_per_buffer = 1024);
	void			appendPCMData(uint8_t *pcm_raw, int raw_length);
	bool			isStopped();
private:
	void			internal_Init();
	PaStream	   *stream;
	int				pcm_channels; 
	int				pcm_sample_rate;
	int				pcm_frames_per_buffer;
	bool			isInitializedPCMPlayer;
};
class CMediaPlayer : public CYUVPlayer, public CPCMPlayer
{
public:
	CMediaPlayer();
	CMediaPlayer(int video_width, int video_height, const char *title,
		int audio_channels, int audio_sample_rate, int audio_frames_per_buffer = 1024,
		int video_frames_buffer = 5, bool video_vertical_sync = true);
	~CMediaPlayer();
	void setupMediaPlayer(int video_width, int video_height, const char *title,
		int audio_channels, int audio_sample_rate, int audio_frames_per_buffer = 1024,
		int video_frames_buffer = 5,bool video_vertical_sync = true);
private:
};
#endif