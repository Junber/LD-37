#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include "object.h"
#include "loading.h"
#include "font.h"
#include "sound.h"
#include "base_functions.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

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

    if (script_variables["dynamic_light"])
    {
        SDL_SetRenderDrawColor(renderer,darkness_color,darkness_color,darkness_color,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            if (o->light_frames) o->render_light();
        }
    }
    else
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);
    }

    for (Object* o: objects)
    {
        if (o->throws_shadow) o->render_shadow(0);
    }

    SDL_SetRenderTarget(renderer,nullptr);
    SDL_RenderCopy(renderer,overlay,nullptr,nullptr);
}

bool comp(Object* a, Object* b)
{
    return (!a->foreground && !b->background && (b->foreground || a->background || (a->pos[1]+a->size[1]/2 < b->pos[1]+b->size[1]/2)));
}

void options()
{
    Mix_VolumeMusic(100);
    int cursor_pos=0, music_volume=100;
    SDL_Event e;
    bool breakkk = false;
	while (!breakkk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = breakkk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    switch (e.key.keysym.sym)
			    {
                    case SDLK_ESCAPE:
                        breakkk = true;
                        break;
                    case SDLK_s:
                        cursor_pos++;
                        break;
                    case SDLK_w:
                        cursor_pos--;
                        break;
                    case SDLK_d:
                        if (cursor_pos == 0) sfx_volume+=10;
                        if (cursor_pos == 1)
                        {
                            music_volume+=10;
                            if (music_volume>100)music_volume=100;
                            Mix_VolumeMusic(music_volume);
                        }
                        if (cursor_pos == 2)
                        {
                            renderzoom++;
                            SDL_SetWindowFullscreen(renderwindow,0);
                            SDL_SetWindowSize(renderwindow,window[0]*renderzoom, window[1]*renderzoom);
                            SDL_RenderSetScale(renderer,renderzoom,renderzoom);
                        }
                        break;
                    case SDLK_a:
                        if (cursor_pos == 0) sfx_volume-=10;
                        if (cursor_pos == 1)
                        {
                            music_volume-=10;
                            if (music_volume<0)music_volume=0;
                            Mix_VolumeMusic(music_volume);
                        }
                        if (cursor_pos == 2)
                        {
                            renderzoom--;
                            if (renderzoom<0) renderzoom=0;

                            if (renderzoom)
                            {
                                SDL_SetWindowSize(renderwindow,window[0]*renderzoom, window[1]*renderzoom);
                                SDL_RenderSetScale(renderer,renderzoom,renderzoom);
                            }
                            else
                            {
                                SDL_DisplayMode mode;
                                SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(renderwindow),&mode);
                                int zoom = std::min(mode.w/window[0],mode.h/window[1]);
                                SDL_SetWindowSize(renderwindow,window[0]*zoom, window[1]*zoom);
                                SDL_RenderSetScale(renderer,zoom,zoom);
                                SDL_SetWindowFullscreen(renderwindow,(!renderzoom)*SDL_WINDOW_FULLSCREEN);
                            }
                        }
                        break;
                }
			}
        }

        cursor_pos %= 3;
        while (cursor_pos < 0) cursor_pos += 3;

        if (sfx_volume>100)sfx_volume=100;
        if (sfx_volume<0)sfx_volume=0;

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer,load_image("menu3options"+std::to_string(cursor_pos+1)),nullptr, nullptr);

        render_text(130,87,std::to_string(sfx_volume)+" %",0);
        render_text(130,102,std::to_string(music_volume)+" %",0);
        render_text(130,117,renderzoom?std::to_string(renderzoom)+"x":"Fullscreen",0);

        SDL_RenderPresent(renderer);
        limit_fps();
    }
}

void main_menu()
{
    int cursor_pos=2;
    SDL_Event e;
    bool breakkk = false;
	while (!breakkk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = breakkk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    switch (e.key.keysym.sym)
			    {
                    case SDLK_ESCAPE:
                        breakk = breakkk = true;
                        break;
                    case SDLK_d:
                    case SDLK_s:
                        cursor_pos++;
                        break;
                    case SDLK_a:
                    case SDLK_w:
                        cursor_pos--;
                        break;
                    case SDLK_e:
                        if (cursor_pos == 0) options();
                        if (cursor_pos == 1) breakk = breakkk = true;
                        if (cursor_pos == 2) breakkk=true;
                        break;
                }
			}
        }

        cursor_pos %= 3;
        while (cursor_pos < 0) cursor_pos += 3;

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer,load_image("menu"+std::to_string(cursor_pos+1)),nullptr, nullptr);
        SDL_RenderPresent(renderer);

        limit_fps();
    }
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    sound_init();
    font_init();
    random_init();

    renderwindow = SDL_CreateWindow("LD 37", 50, 50, window[0]*renderzoom, window[1]*renderzoom, SDL_WINDOW_SHOWN);//|SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetScale(renderer,renderzoom,renderzoom);

    overlay = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,window[0],window[1]);
    SDL_SetTextureBlendMode(overlay,SDL_BLENDMODE_MOD);

    main_menu();
    if (breakk) return 0;

    player = new Player();

    int bg_size[2];
    SDL_QueryTexture(load_image("background"),nullptr,nullptr,&bg_size[0],&bg_size[1]);

    load_script("start",&player->script);//("start",&player->script);
    execute_script(player->script);
    player->script.clear();

    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    bool done_something=false;

			    switch (e.key.keysym.sym)
			    {
                    case SDLK_ESCAPE:
                        breakk = true;
                        break;
                    case SDLK_e:
                        for (Object* o: objects)
                        {
                            if (o->interact(false))
                            {
                                done_something = true;
                                break;
                            }
                        }
                        if (done_something) break;

                        for (Object* o: objects)
                        {
                            if (o != player && o->collision(player->pos[0],player->pos[1],player->size[0]+2,player->size[1]+2,player->non_hitbox_height))
                            {
                                if (o->interact(true))
                                {
                                    done_something = true;
                                    break;
                                }
                            }
                        }
                        break;
			    }

			    if (done_something) break;
			}
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        if (!active_effects[trails]) SDL_RenderClear(renderer);

        if (cur_closeup != "")
        {
            SDL_RenderCopy(renderer,load_image(cur_closeup),nullptr,nullptr);
            for (Object* o: objects) if (!o->use_camera) o->update();
        }
        else
        {
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

            if ((script_variables.count("hallway") && script_variables["hallway"]))
            {
                if (camera_pos[0] < window[0]/2-5-308-54) camera_pos[0] = window[0]/2-5-308-54;
            }
            else if (camera_pos[0] < window[0]/2-5) camera_pos[0] = window[0]/2-5;

            if (camera_pos[0] > bg_size[0]-window[0]/2+20) camera_pos[0] = bg_size[0]-window[0]/2+20;
            if (camera_pos[1] < window[1]/2-5) camera_pos[1] = window[1]/2-5;
            else if (camera_pos[1] > bg_size[1]-window[1]/2+135) camera_pos[1] = bg_size[1]-window[1]/2+135;

            SDL_Rect r = {8-camera_pos[0]+window[0]/2, 72-camera_pos[1]+window[1]/2, bg_size[0], bg_size[1]};

            SDL_Texture* bg = (script_variables.count("dirty") && script_variables["dirty"])?load_image("background_dirty"):load_image("background");

            if (active_effects[trails])
            {
                SDL_SetTextureAlphaMod(bg,20);
                SDL_SetTextureBlendMode(bg,SDL_BLENDMODE_BLEND);
            }
            if (active_effects[random_bg_color]) SDL_SetTextureColorMod(bg,random(0,255),random(0,255),random(0,255));

            SDL_RenderCopy(renderer,bg,nullptr,&r);

            std::stable_sort(objects.begin(),objects.end(),comp);

            for (Object* o: objects) if (o->use_camera) o->render();
            render_shadows((script_variables.count("pitch_black") && script_variables["pitch_black"])?10:((script_variables.count("not_that_dark") && script_variables["not_that_dark"])?40:20));
        }

        for (Object* o: objects) if (!o->use_camera) o->render();

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
                if (objects[ignored]->persistent) ignored++;
                else delete objects[ignored];
            }

            load_level(level_to_load);
            level_to_load = "";
        }

        if (active_effects[move_window]) SDL_SetWindowPosition(renderwindow,camera_pos[0],camera_pos[1]);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
