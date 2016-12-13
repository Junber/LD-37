#include "loading.h"
#include "base_functions.h"
#include "object.h"
#include <map>

const int renderzoom = 5;
bool breakk = false;
int sfx_volume=100;
const int window[2] = {320,180}; //{480,270};//{960,540};
SDL_Window* renderwindow = nullptr;
SDL_Renderer* renderer = nullptr;
std::string level_to_load = "", cur_level = "", cur_closeup = "";
bool active_effects[EFFECT_NUM] = {};
std::map<std::string,bool> script_variables;

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,(std::string("Data")+PATH_SEPARATOR+"Graphics"+PATH_SEPARATOR+s+".png").c_str());

    return loaded_textures[s];
}

void load_level(std::string name)
{
    std::fstream file;
    file.open(std::string("Data")+PATH_SEPARATOR+"Levels"+PATH_SEPARATOR+name+".txt");

    while (!file.eof())
    {
        std::string line;
        std::getline(file,line);

        auto presplitted = split(line,':');

        if (presplitted.size()>1)
        {
            if ((!script_variables.count(presplitted[0]) || !script_variables[presplitted[0]])) continue;

            line = presplitted[1];
        }

        auto splitted = split(line,',');

        if (line[0]!='#') new Object(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[0],std::stoi(splitted[4]),splitted[3],
                                     splitted.size()>5?splitted[5]=="1":false,splitted.size()>6?splitted[6]=="1":false,
                                     splitted.size()>7?splitted[7]=="1":false,splitted.size()>8?std::stoi(splitted[8]):0,
                                     splitted.size()>9?splitted[9]=="1":false);
    }

    cur_level = name;
}

void load_script(std::string name, std::deque<std::string>* result)
{
    if (!name.size()) return;

    std::fstream file;
    file.open(std::string("Data")+PATH_SEPARATOR+"Scripts"+PATH_SEPARATOR+name+".txt");

    while (!file.eof())
    {
        std::string line;
        std::getline(file,line);

        if (line[0]!='#') result->push_back(line);
    }
}

std::map<std::string,Mix_Chunk*> loaded_sounds;
Mix_Chunk* load_sound(std::string s)
{
    if (!loaded_sounds.count(s))
    {
        loaded_sounds[s] = Mix_LoadWAV((std::string("Data")+PATH_SEPARATOR+"Sounds"+PATH_SEPARATOR+s+".wav").c_str());
        Mix_VolumeChunk(loaded_sounds[s], sfx_volume);
    }
    return loaded_sounds[s];
}

std::map<std::string,Mix_Music*> loaded_music;
Mix_Music* load_music(std::string s)
{
    if (!loaded_music.count(s)) loaded_music[s] = Mix_LoadMUS((std::string("Data")+PATH_SEPARATOR+"Music"+PATH_SEPARATOR+s+".mp3").c_str());
    return loaded_music[s];
}
