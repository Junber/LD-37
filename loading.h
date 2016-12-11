#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>
#include <deque>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

enum effects
{
    trails,random_bg_color,move_window,EFFECT_NUM
};

extern SDL_Window* renderwindow;
extern SDL_Renderer* renderer;
extern const int window[2];
extern const int renderzoom;
extern std::string level_to_load;
extern bool active_effects[EFFECT_NUM];

SDL_Texture* load_image(std::string s);
void load_level(std::string name);
void load_script(std::string file, std::deque<std::string>* result);
Mix_Chunk* load_sound(std::string s);

#endif // _LOADING
