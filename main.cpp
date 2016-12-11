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
#include "base_functions.h"

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
        if (o->throws_shadow) o->render_shadow(darkness_color);
    }

    SDL_SetRenderTarget(renderer,nullptr);
    SDL_RenderCopy(renderer,overlay,nullptr,nullptr);
}

bool comp(Object* a, Object* b)
{
    return (!a->foreground && (b->foreground || (a->pos[1]+a->size[1]/2 < b->pos[1]+b->size[1]/2)));
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    sound_init();
    font_init();
    random_init();

    renderwindow = SDL_CreateWindow("LD 37", 50, 50, window[0]*renderzoom, window[1]*renderzoom, SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    overlay = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,window[0],window[1]);
    SDL_SetTextureBlendMode(overlay,SDL_BLENDMODE_MOD);

    player = new Player();

    SDL_Texture* bg = load_image("background");
    int bg_size[2];
    SDL_QueryTexture(bg,nullptr,nullptr,&bg_size[0],&bg_size[1]);

    load_level("start");

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
        if (!active_effects[trails]) SDL_RenderClear(renderer);

        player->update();
        for (Object* o: objects)
        {
            if (o != player) o->update();
        }

        for (Object* o: automatic_trigger)
        {
            if (o->collision(player->pos[0], player->pos[1], player->size[0], player->size[1], player->non_hitbox_height))
            {
                o->interact(true);
            }
        }

        camera_pos[0] = player->pos[0];
        camera_pos[1] = player->pos[1];

        if (camera_pos[0] < window[0]/2-5) camera_pos[0] = window[0]/2-5;
        else if (camera_pos[0] > bg_size[0]-window[0]/2+20) camera_pos[0] = bg_size[0]-window[0]/2+20;
        if (camera_pos[1] < window[1]/2-5) camera_pos[1] = window[1]/2-5;
        else if (camera_pos[1] > bg_size[1]-window[1]/2+135) camera_pos[1] = bg_size[1]-window[1]/2+135;

        SDL_Rect r = {(8-camera_pos[0]+window[0]/2)*renderzoom, (72-camera_pos[1]+window[1]/2)*renderzoom, bg_size[0]*renderzoom, bg_size[1]*renderzoom};

        if (active_effects[trails])
        {
            SDL_SetTextureAlphaMod(bg,20);
            SDL_SetTextureBlendMode(bg,SDL_BLENDMODE_BLEND);
        }
        if (active_effects[random_bg_color]) SDL_SetTextureColorMod(bg,random(0,255),random(0,255),random(0,255));

        SDL_RenderCopy(renderer,bg,nullptr,&r);

        std::stable_sort(objects.begin(),objects.end(),comp);

        for (Object* o: objects)
        {
            o->render();
        }

        render_shadows(100);

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

        if (active_effects[move_window]) SDL_SetWindowPosition(renderwindow,camera_pos[0]*renderzoom,camera_pos[1]*renderzoom);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
