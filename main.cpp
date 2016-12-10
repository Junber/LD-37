#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

const int window[2] = {480,270};

const int zoom = 2;

bool breakk = false;
SDL_Window* renderwindow;
SDL_Renderer* renderer;

const Uint8* keystate;

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

std::deque<std::string> split(std::string s, char seperator)
{
    std::deque<std::string> ret;
    ret.push_back("");
    for (char c: s)
    {
        if (c == seperator)
        {
            ret.push_back("");
        }
        else
        {
            ret[ret.size()-1] += c;
        }
    }

    return ret;
}


std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,(std::string("Data")+PATH_SEPARATOR+"Graphics"+PATH_SEPARATOR+s+".png").c_str());

    return loaded_textures[s];
}

class Object;
std::deque<Object*> objects;
class Object
{
public:
    int pos[2], size[2];
    SDL_Texture* tex;

    Object(int x, int y, std::string s)
    {
        pos[0] = x;
        pos[1] = y;

        tex = load_image(s);
        SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

        objects.push_back(this);
    }

    virtual ~Object()
    {
        remove_it(&objects, this);
    }

    virtual void update()
    {

    }

    void render()
    {
        SDL_Rect r={pos[0]*zoom, pos[1]*zoom, size[0]*zoom, size[1]*zoom};

        SDL_RenderCopy(renderer, tex, nullptr, &r);
    }
};

class Player: public Object
{
public:
    Player() : Object(10,10,"Test") {}

    void update()
    {
        if (keystate[SDL_SCANCODE_D]) pos[0]++;
        if (keystate[SDL_SCANCODE_A]) pos[0]--;
        if (keystate[SDL_SCANCODE_S]) pos[1]++;
        if (keystate[SDL_SCANCODE_W]) pos[1]--;
    }
};

void load_level(std::string name)
{
    std::fstream file;
    file.open(std::string("Data")+PATH_SEPARATOR+"Levels"+PATH_SEPARATOR+name+".txt");

    while (!file.eof())
    {
        std::string line;
        std::getline(file,line);

        auto splitted = split(line,',');

        new Object(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[0]);
    }
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);

    renderwindow = SDL_CreateWindow("LD 37", 50, 50, window[0]*zoom, window[1]*zoom, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player* player = new Player();

    load_level("Test");

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    switch (e.key.keysym.sym)
			    {
                    case SDLK_ESCAPE:
                        breakk = true;
                        break;
			    }
			}
        }

        keystate = SDL_GetKeyboardState(nullptr);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
