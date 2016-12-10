#include "object.h"
#include "loading.h"
#include "base_functions.h"
#include "font.h"
#include "sound.h"
#include "loading.h"
#include <iostream>

const Uint8* keystate;
std::deque<Object*> objects,  to_delete;

Player* player;

Object::Object(int x, int y, std::string s, bool b, std::string script_file)
{
    pos[0] = x;
    pos[1] = y;

    tex = load_image(s);
    SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

    objects.push_back(this);

    blocks = b;
    load_script(script_file, &script);
}

Object::~Object()
{
    remove_it(&objects, this);
}

bool Object::collision(int x, int y, int sx, int sy)
{
    return (pos[0]-size[0]/2 < x+sx/2+sx%2 && pos[0]+size[0]/2+size[0]%2 > x-sx/2 && pos[1]-size[1]/2 < y+sy/2+sy%2 && pos[1]+size[1]/2+size[1]%2 > y-sy/2);
}

void Object::update()
{
    if (blocks)
    {
        if (collision(player->pos[0],player->pos[1],player->size[0],player->size[1]))
        {
            if (collision(player->lastpos[0],player->lastpos[1],player->size[0],player->size[1]))
            {
                while (collision(player->pos[0],player->pos[1],player->size[0],player->size[1]))
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

                if (player->lastpos[1]+player->size[1]/2+player->size[1]%2 <= pos[1]-size[1]/2) player->pos[1] = pos[1]-player->size[1]/2-size[1]/2-player->size[1]%2;
                if (player->lastpos[1]-player->size[1]/2 >= pos[1]+size[1]/2+size[1]%2) player->pos[1] = pos[1]+player->size[1]/2+size[1]/2+size[1]%2;
            }
        }
    }
}

bool Object::interact(bool touch)
{
    if (touch)
    {
        if (script.empty()) return false;

        for (std::string line: script)
        {
            auto splitted = split(line,',');
            if (splitted[0] == "play") //I hate string comparisons as much as the next guy but this is a lot easier
            {
                play_sound(load_sound(splitted[1]));
            }
            else if (splitted[0] == "dialog")
            {
                new Dialog_box(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[3],std::stoi(splitted[4]))
            }
            else if (splitted[0] == "level")
            {
                level_to_load = splitted[1];
            }
        }
    }

    return false;
}

void Object::render()
{
    SDL_Rect r={pos[0]*renderzoom, pos[1]*renderzoom, size[0]*renderzoom, size[1]*renderzoom};

    SDL_RenderCopy(renderer, tex, nullptr, &r);
}

void Player::update()
{
    lastpos[0]=pos[0]; lastpos[1]=pos[1];

    keystate = SDL_GetKeyboardState(nullptr);

    if (keystate[SDL_SCANCODE_D]) pos[0]++;
    if (keystate[SDL_SCANCODE_A]) pos[0]--;
    if (keystate[SDL_SCANCODE_S]) pos[1]++;
    if (keystate[SDL_SCANCODE_W]) pos[1]--;
}

Dialog_box::Dialog_box(int x, int y, std::string t, int speed) : Object(x,y,"Dialog_Box",false,"")
{
    text = t;
    progress = 0;
    type_speed = speed;
}

void Dialog_box::update()
{
    if (progress < text.size()*type_speed) progress++;
}

bool Dialog_box::interact(bool touch)
{
    if (!touch)
    {
        if (progress < text.size()*type_speed) progress = text.size()*type_speed;
        else to_delete.push_back(this);

        return true;
    }

    return false;
}

void Dialog_box::render()
{
    Object::render();

    render_text(pos[0]+10, pos[1]+10, text.substr(0,progress/type_speed),255);
}
