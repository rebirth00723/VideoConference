#ifndef SDL_BASE_H
#include <SDL.h>
#include <SDL_syswm.h>
#include <cstdio>
#include <cstring>
class CSDLBase
{
public:
	CSDLBase();
protected:
	bool virtual		 internal_Init() = 0;
	SDL_SysWMinfo		 info;
private:
	static bool			 isInitializedSDL;
};
#endif