#include "loading.h"
#include "base_functions.h"
#include "object.h"
#include <map>

const int renderzoom = 2;
const int window[2] = {480,270};
SDL_Window* renderwindow = nullptr;
SDL_Renderer* renderer = nullptr;

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

        auto splitted = split(line,',');

        new Object(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[0],true);
    }
}
