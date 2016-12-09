#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

const int window[2] = {500,500};

bool breakk = false;
SDL_Window* renderwindow;
SDL_Renderer* renderer;

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

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,("Data\\Graphics\\"+s+".png").c_str());

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
        SDL_Rect r={pos[0], pos[1], size[0], size[1]};

        SDL_RenderCopy(renderer, tex, nullptr, &r);
    }
};

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);

    renderwindow = SDL_CreateWindow("...", 50, 50, window[0], window[1], SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer,load_image("Test"),nullptr,nullptr);

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
