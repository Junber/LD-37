#include "object.h"
#include "loading.h"
#include "base_functions.h"

const Uint8* keystate;
std::deque<Object*> objects;

Player* player;

Object::Object(int x, int y, std::string s, bool b)
{
    pos[0] = x;
    pos[1] = y;

    tex = load_image(s);
    SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

    objects.push_back(this);

    blocks = b;
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
