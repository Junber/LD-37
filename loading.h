#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>
#include <deque>
#include <map>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

enum effects
{
    trails,random_bg_color,move_window,EFFECT_NUM
};

extern bool breakk;
extern SDL_Window* renderwindow;
extern SDL_Renderer* renderer;
extern const int window[2];
extern int renderzoom;
extern int sfx_volume;
extern std::string level_to_load, cur_level, cur_closeup;
extern bool active_effects[EFFECT_NUM];
extern std::map<std::string,bool> script_variables;

SDL_Texture* load_image(std::string s);
void load_level(std::string name);
void load_script(std::string file, std::deque<std::string>* result);
Mix_Chunk* load_sound(std::string s);
Mix_Music* load_music(std::string s);

#endif // _LOADING
