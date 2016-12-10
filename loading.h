#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <fstream>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

extern SDL_Window* renderwindow;
extern SDL_Renderer* renderer;
extern const int window[2];
extern const int renderzoom;

SDL_Texture* load_image(std::string s);

void load_level(std::string name);

#endif // _LOADING
