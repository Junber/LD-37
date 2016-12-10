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

SDL_Texture* overlay;

void render_shadows(int darkness_color)
{
    SDL_SetRenderTarget(renderer,overlay);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);//darkness_color,darkness_color,darkness_color,255);
    SDL_RenderClear(renderer);
    for (Object* o: objects)
    {
        o->render_shadow(darkness_color);
    }
    SDL_SetRenderTarget(renderer,nullptr);

    SDL_RenderCopy(renderer,overlay,nullptr,nullptr);

    SDL_SetRenderTarget(renderer,nullptr);

    SDL_RenderCopy(renderer,overlay,nullptr,nullptr);
}

bool comp(Object* a, Object* b)
{
    return (!a->foreground && (b->foreground || a->pos[1] < b->pos[1]));
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    sound_init();
    font_init();

    renderwindow = SDL_CreateWindow("LD 37", 50, 50, window[0]*renderzoom, window[1]*renderzoom, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    overlay = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,window[0],window[1]);
    SDL_SetTextureBlendMode(overlay,SDL_BLENDMODE_MOD);

    player = new Player();

    SDL_Texture* bg = load_image("background");
    int bg_size[2];
    SDL_QueryTexture(bg,nullptr,nullptr,&bg_size[0],&bg_size[1]);

    load_level("Test");

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
                            if (o != player && o->collision(player->pos[0],player->pos[1],player->size[0]+2,player->size[1]+2,player->non_hitbox_height))
                            {
                                o->interact(true);
                            }
                        }
                        break;
			    }
			}
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_Rect r = {8-camera_pos[0]+window[0]/2, 72-camera_pos[1]+window[1]/2, bg_size[0]*renderzoom, bg_size[1]*renderzoom};
        SDL_RenderCopy(renderer,bg,nullptr,&r);

        player->update();
        for (Object* o: objects)
        {
            if (o != player) o->update();
        }

        std::stable_sort(objects.begin(),objects.end(),comp);

        for (Object* o: objects)
        {
            o->render();
        }

        //render_shadows(100);

        for (Object* o: to_delete)
        {
            delete o;
        }
        to_delete.clear();

        if (!level_to_load.empty())
        {
            unsigned ignored=0;
            while (objects.size() > ignored)
            {
                if (objects[ignored] == player) ignored++;
                else delete objects[ignored];
            }

            load_level(level_to_load);
            level_to_load = "";
        }

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
