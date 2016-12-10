#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include "object.h"
#include "loading.h"
#include "font.h"
#include "sound.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

bool breakk = false;

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    sound_init();
    font_init();

    renderwindow = SDL_CreateWindow("LD 37", 50, 50, window[0]*renderzoom, window[1]*renderzoom, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    player = new Player();

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
                    case SDLK_e:
                        bool done_soemthing=false;
                        for (Object* o: objects)
                        {
                            if (o->interact(false))
                            {
                                done_soemthing = true;
                                break;
                            }
                        }
                        if (done_soemthing) break;

                        for (Object* o: objects)
                        {
                            if (o != player && o->collision(player->pos[0],player->pos[1],player->size[0]+2,player->size[1]+2))
                            {
                                o->interact(true);
                            }
                        }
                        break;
			    }
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
        }

        for (Object* o: objects)
        {
            o->render();
        }

        for (Object* o: to_delete)
        {
            delete o;
        }
        to_delete.clear();

        if (!level_to_load.empty())
        {
            load_level(level_to_load);
            level_to_load = "";
        }

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
