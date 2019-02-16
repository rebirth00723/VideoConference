#include "SDLBase.h"
bool CSDLBase::isInitializedSDL = false;
CSDLBase::CSDLBase()
{
	if (!isInitializedSDL)
	{
		if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
		{
			printf("Unable to initialize SDL library.\n");
			return;
		}
		memset(&info, 0, sizeof(info));
		SDL_VERSION(&info.version);
		printf("SDL has been initialized.\n");
		isInitializedSDL = true;
	}
}