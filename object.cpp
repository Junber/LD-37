#include "object.h"
#include "loading.h"
#include "base_functions.h"
#include "font.h"
#include "sound.h"
#include "loading.h"
#include <iostream>
#include "SDL2_gfx-1.0.1/SDL2_gfxPrimitives.h"

const Uint8* keystate;
std::deque<Object*> objects, to_delete;

int camera_pos[2] = {0,0};

Player* player;

void execute_script(std::deque<std::string> script)
{
    Dialog_box* d = nullptr;
    for (std::string line: script)
    {
        if (d) d->script.push_back(line);
        else
        {
            auto splitted = split(line,',');
            if (splitted[0] == "sound") //I hate string comparisons as much as the next guy but this is a lot easier
            {
                play_sound(load_sound(splitted[1]));
            }
            else if (splitted[0] == "music")
            {
                Mix_FadeOutMusic(1000);
                play_music(load_music(splitted[1]));
            }
            else if (splitted[0] == "dialog")
            {
                d = new Dialog_box(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[3],std::stoi(splitted[4]));
            }
            else if (splitted[0] == "level")
            {
                level_to_load = splitted[1];
            }
        }
    }
}

Object::Object(int x, int y, std::string s, int hitbox_height, std::string script_file, bool shadow, bool in_foreground)
{
    pos[0] = x;
    pos[1] = y;

    tex = load_image(s);
    SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    objects.push_back(this);

    load_script(script_file, &script);

    blocks = bool(hitbox_height);
    non_hitbox_height = size[1]-hitbox_height;

    foreground=in_foreground;
    use_camera=true;

    throws_shadow=shadow;
}

Object::~Object()
{
    remove_it(&objects, this);
}

bool Object::collision(int x, int y, int sx, int sy, int nhh)
{
    return (pos[0]-size[0]/2 < x+sx/2+sx%2 && pos[0]+size[0]/2+size[0]%2 > x-sx/2 && pos[1]-size[1]/2+non_hitbox_height < y+sy/2+sy%2 && pos[1]+size[1]/2+size[1]%2 > y-sy/2+nhh);
}

void Object::update()
{
    if (blocks)
    {
        if (collision(player->pos[0],player->pos[1],player->size[0],player->size[1],player->non_hitbox_height))
        {
            if (collision(player->lastpos[0],player->lastpos[1],player->size[0],player->size[1],player->non_hitbox_height))
            {
                while (collision(player->pos[0],player->pos[1],player->size[0],player->size[1],player->non_hitbox_height))
                {
                    if (pos[0]-player->pos[0] || pos[1]-player->pos[1])
                    {
                        player->pos[0] += sign(player->pos[0]-pos[0]);
                        player->pos[1] += sign(player->pos[1]-pos[1]);
                    }
                    else
                    {
                        player->pos[0]++; player->pos[1]++;
                    }
                }
            }
            else
            {
                if (player->lastpos[0]+player->size[0]/2+player->size[0]%2 <= pos[0]-size[0]/2) player->pos[0] = pos[0]-player->size[0]/2-size[0]/2-player->size[0]%2;
                if (player->lastpos[0]-player->size[0]/2 >= pos[0]+size[0]/2+size[0]%2) player->pos[0] = pos[0]+player->size[0]/2+size[0]/2+size[0]%2;

                if (player->lastpos[1]+player->size[1]/2+player->size[1]%2 <= pos[1]-size[1]/2+non_hitbox_height) player->pos[1] = pos[1]-player->size[1]/2-size[1]/2-player->size[1]%2+non_hitbox_height;
                if (player->lastpos[1]-player->size[1]/2+player->non_hitbox_height >= pos[1]+size[1]/2+size[1]%2) player->pos[1] = pos[1]+player->size[1]/2+size[1]/2+size[1]%2-player->non_hitbox_height;
            }
        }
    }
}

bool Object::interact(bool touch)
{
    if (touch)
    {
        if (script.empty()) return false;

        execute_script(script);
    }

    return false;
}

void Object::render()
{
    SDL_Rect r={(pos[0]+use_camera*(window[0]/2-size[0]/2-camera_pos[0]))*renderzoom, (pos[1]+use_camera*(window[1]/2-size[1]/2-camera_pos[1]))*renderzoom, size[0]*renderzoom, size[1]*renderzoom};

    if (active_effects[trails]) SDL_SetTextureAlphaMod(tex,50);
    SDL_RenderCopy(renderer, tex, nullptr, &r);
}

void Object::render_shadow(int darkness_color)
{
    Sint16 vx[7], vy[7];
    bool pentagon = false;
    int add_corners=0;
    SDL_SetRenderDrawColor(renderer,darkness_color,darkness_color,darkness_color,255);

    if (player->pos[0] <= pos[0]+size[0]/2 && player->pos[0] >= pos[0]-size[0]/2)
    {
        if (player->pos[1]<pos[1])
        {
            vx[0] = pos[0]+size[0]/2+size[0]%2;
            vy[0] = pos[1]-size[1]/2+non_hitbox_height;
            vx[1] = pos[0]-size[0]/2;
            vy[1] = pos[1]-size[1]/2+non_hitbox_height;
        }
        else
        {
            vx[0] = pos[0]-size[0]/2;
            vy[0] = pos[1]+size[1]/2+size[1]%2;
            vx[1] = pos[0]+size[0]/2+size[0]%2;
            vy[1] = pos[1]+size[1]/2+size[1]%2;
        }
    }
    else if (player->pos[1] <= pos[1]+size[1]/2 && player->pos[1] >= pos[1]-size[1]/2+non_hitbox_height)
    {
        if (player->pos[0]<pos[0])
        {
            vx[0] = pos[0]-size[0]/2;
            vy[0] = pos[1]-size[1]/2+non_hitbox_height;
            vx[1] = pos[0]-size[0]/2;
            vy[1] = pos[1]+size[1]/2+size[1]%2;
        }
        else
        {
            vx[0] = pos[0]+size[0]/2+size[0]%2;
            vy[0] = pos[1]+size[1]/2+size[1]%2;
            vx[1] = pos[0]+size[0]/2+size[0]%2;
            vy[1] = pos[1]-size[1]/2+non_hitbox_height;
        }
    }
    else
    {
        pentagon = true;

        if ((player->pos[0] < pos[0]) == (player->pos[1] < pos[1]))
        {
            int m = (player->pos[0] < pos[0])?-1:1;

            vx[0] = pos[0]-size[0]/2;
            vy[0] = pos[1]+size[1]/2+size[1]%2;
            vx[1] = pos[0]+m*size[0]/2+(m==1)*size[0]%2;
            vy[1] = pos[1]+m*size[1]/2+(m==1)*size[0]%2+(m!=1)*non_hitbox_height;
            vx[2] = pos[0]+size[0]/2+size[0]%2;
            vy[2] = pos[1]-size[1]/2+non_hitbox_height;
        }
        else
        {
            int m = (player->pos[0] < pos[0])?-1:1;

            vx[0] = pos[0]-size[0]/2;
            vy[0] = pos[1]-size[1]/2+non_hitbox_height;
            vx[1] = pos[0]+m*size[0]/2+(m==1)*size[0]%2;
            vy[1] = pos[1]-m*size[1]/2+(m!=1)*size[0]%2+(m==1)*non_hitbox_height;
            vx[2] = pos[0]+size[0]/2+size[0]%2;
            vy[2] = pos[1]+size[1]/2+size[1]%2;
        }
    }

    int wall[2];
    for (int i=0; i<=(pentagon?2:1); i+=(pentagon?2:1))
    {
        int dx = vx[i]-player->pos[0], dy = vy[i]-player->pos[1];
        float dratio = float_abs(dy?float(dx)/dy:1);
        if (dratio < 0.0001) dratio=0;

        int lengthx = abs(vx[i]-(camera_pos[0]+sign(dx)*window[0]/2)), lengthy = abs(vy[i]-(camera_pos[1]+sign(dy)*window[1]/2));

        bool choose_x = (!dy || (dx && lengthy>(lengthx/dratio)));
        float length = choose_x?(float(lengthx)/dratio):lengthy;
        vx[i] += -camera_pos[0]+window[0]/2;
        vy[i] += -camera_pos[1]+window[1]/2;

        if (!i)
        {
            wall[0] = choose_x?(dx>0?window[0]:0):-1;
            wall[1] = choose_x?-1:(dy>0?window[1]:0);
        }
        else
        {
            if (choose_x == (wall[0]==-1))
            {
                add_corners = 1;
                vx[4+pentagon] = vx[3+pentagon];
                vy[4+pentagon] = vy[3+pentagon];

                vx[3+pentagon] = choose_x?(dx>0?window[0]:0):wall[0];
                vy[3+pentagon] = choose_x?wall[1]:(dy>0?window[1]:0);
            }
            else if (choose_x && wall[0] != (dx>0?window[0]:0))
            {
                add_corners = 2;
                vx[5+pentagon] = vx[3+pentagon];
                vy[5+pentagon] = vy[3+pentagon];

                vx[3+pentagon] = dx>0?window[0]:0;
                vx[4+pentagon] = dx>0?0:window[0];
                vy[3+pentagon] = vy[4+pentagon] = dy>0?window[1]:0;
            }
            else if (!choose_x && wall[1] != (dy>0?window[1]:0))
            {
                add_corners = 2;
                vx[5+pentagon] = vx[3+pentagon];
                vy[5+pentagon] = vy[3+pentagon];

                vx[3+pentagon] = vx[4+pentagon] = dx>0?window[0]:0;
                vy[3+pentagon] = dy>0?window[1]:0;
                vy[4+pentagon] = dy>0?0:window[1];
            }
        }

        vx[(pentagon?(4-i/2):(3-i))] = vx[i]+(length*sign(dx)*dratio);
        vy[(pentagon?(4-i/2):(3-i))] = vy[i]+(dy?(length*sign(dy)):0);
    }
    if (pentagon)
    {
        vx[1] += -camera_pos[0]+window[0]/2;
        vy[1] += -camera_pos[1]+window[1]/2;
    }

    filledPolygonRGBA(renderer,vx,vy,(pentagon?5:4)+add_corners,darkness_color,darkness_color,darkness_color,255);
}

void Player::update()
{
    lastpos[0]=pos[0]; lastpos[1]=pos[1];

    keystate = SDL_GetKeyboardState(nullptr);

    int move_speed = 3;

    if (keystate[SDL_SCANCODE_D]) pos[0]+=move_speed;
    if (keystate[SDL_SCANCODE_A]) pos[0]-=move_speed;
    if (keystate[SDL_SCANCODE_S]) pos[1]+=move_speed;
    if (keystate[SDL_SCANCODE_W]) pos[1]-=move_speed;
}

Dialog_box::Dialog_box(int x, int y, std::string t, int speed) : Object(x,y,"Dialog_Box",0,"",false,true)
{
    text = t;
    progress = 0;
    type_speed = speed;

    use_camera = false;
}

void Dialog_box::update()
{
    if (progress < text.size()*type_speed)
    {
        progress++;
        if (!(progress%type_speed)) play_sound(load_sound("text"));
    }
}

bool Dialog_box::interact(bool touch)
{
    if (!touch)
    {
        if (progress < text.size()*type_speed) progress = text.size()*type_speed;
        else
        {
            to_delete.push_back(this);
            execute_script(script);
        }

        return true;
    }

    return false;
}

void Dialog_box::render()
{
    Object::render();

    render_text(pos[0]+10, pos[1]+10, text.substr(0,progress/type_speed),255);
}
