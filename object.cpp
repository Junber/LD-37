#include "object.h"
#include "loading.h"
#include "base_functions.h"
#include "font.h"
#include "sound.h"
#include "loading.h"
#include "SDL2_gfx-1.0.1/SDL2_gfxPrimitives.h"

const Uint8* keystate;
std::deque<Object*> objects, to_delete, automatic_trigger;

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
            auto presplitted = split(line,':');

            if (presplitted.size()>1)
            {
                if ((!script_variables.count(presplitted[0]) || !script_variables[presplitted[0]])) continue;

                line = presplitted[1];
            }

            auto splitted = split(line,';');
            if (splitted[0] == "sound") //I hate string comparisons as much as the next guy but this is a lot easier
            {
                play_sound(load_sound(splitted[1]),(splitted.size()>2 && splitted[2]=="1"));
            }
            else if (splitted[0] == "stop_sound")
            {
                stop_sound();
            }
            else if (splitted[0] == "music")
            {
                Mix_FadeOutMusic(1000);
                play_music(load_music(splitted[1]));
            }
            else if (splitted[0] == "dialog")
            {
                d = new Dialog_box(std::stoi(splitted[1]),std::stoi(splitted[2]),splitted[3],std::stoi(splitted[4]),splitted[5]);
            }
            else if (splitted[0] == "level")
            {
                if (splitted.size()>1) level_to_load = splitted[1];
                else level_to_load = cur_level;
            }
            else if (splitted[0] == "teleport")
            {
                player->pos[0] = std::stoi(splitted[1]);
                player->pos[1] = std::stoi(splitted[2]);
            }
            else if (splitted[0] == "closeup")
            {
                if (splitted[1]=="1") cur_closeup = splitted[2];
                else cur_closeup = "";
            }
            else if (splitted[0] == "set")
            {
                script_variables[splitted[1]] = std::stoi(splitted[2]);
            }
            else if (splitted[0] == "end_game")
            {
                breakk=true;
                return;
            }
        }
    }
}

Object::Object(int x, int y, std::string s, int hitbox_height, std::string script_file, bool shadow, bool in_foreground, bool zone, int light, bool in_background)
{
    pos[0] = x;
    pos[1] = y;

    tex = load_image(s);

    light_frames = light;
    cur_light_frame=0;
    if (light_frames==1)
    {
        light_tex[0] = load_image("light_"+s);
        SDL_SetTextureBlendMode(light_tex[0],SDL_BLENDMODE_ADD);
    }
    else if (light_frames)
    {
        for (int i=1; i<=light_frames; i++)
        {
            light_tex[i-1] = load_image("light_"+s+std::to_string(i));
            SDL_SetTextureBlendMode(light_tex[i-1],SDL_BLENDMODE_ADD);
        }
    }
    else
    {
        light_tex[0] = nullptr;
    }

    SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);
    SDL_QueryTexture(light_tex[0], nullptr, nullptr, &light_size[0], &light_size[1]);

    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    load_script(script_file, &script);

    non_hitbox_height = size[1]-hitbox_height;

    foreground=in_foreground;
    background=in_background;
    use_camera=true;
    persistent = false;
    is_zone = zone;
    throws_shadow=shadow;
    blocks = !is_zone&&bool(hitbox_height);

    objects.push_back(this);
    if (is_zone) automatic_trigger.push_back(this);
}

Object::~Object()
{
    remove_it(&objects, this);
    if (is_zone) remove_it(&automatic_trigger, this);
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

    if (light_frames)
    {
        if (!random(0,4))
        {
            cur_light_frame = random(0,light_frames-1);
        }
    }
}

bool Object::interact(bool touch)
{
    if (touch)
    {
        if (script.empty()) return false;

        execute_script(script);

        return true;
    }

    return false;
}

void Object::render()
{
    SDL_Rect r={pos[0]-size[0]/2+use_camera*(window[0]/2-camera_pos[0]), pos[1]-size[1]/2+use_camera*(window[1]/2-camera_pos[1]), size[0], size[1]};

    if (active_effects[trails]) SDL_SetTextureAlphaMod(tex,50);
    SDL_RenderCopy(renderer, tex, nullptr, &r);
}

void Object::render_shadow(int darkness_color)
{
    const int lightsource[2] = {player->pos[0], player->pos[1]-player->size[1]/2+8};

    Sint16 vx[7], vy[7];
    bool pentagon = false;
    int add_corners=0;
    SDL_SetRenderDrawColor(renderer,darkness_color,darkness_color,darkness_color,255);

    if (lightsource[0] <= pos[0]+size[0]/2 && lightsource[0] >= pos[0]-size[0]/2)
    {
        if (lightsource[1]<pos[1])
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
    else if (lightsource[1] <= pos[1]+size[1]/2 && lightsource[1] >= pos[1]-size[1]/2+non_hitbox_height)
    {
        if (lightsource[0]<pos[0])
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

        if ((lightsource[0] < pos[0]) == (lightsource[1] < pos[1]))
        {
            int m = (lightsource[0] < pos[0])?-1:1;

            vx[0] = pos[0]-size[0]/2;
            vy[0] = pos[1]+size[1]/2+size[1]%2;
            vx[1] = pos[0]+m*size[0]/2+(m==1)*size[0]%2;
            vy[1] = pos[1]+m*size[1]/2+(m==1)*size[0]%2+(m!=1)*non_hitbox_height;
            vx[2] = pos[0]+size[0]/2+size[0]%2;
            vy[2] = pos[1]-size[1]/2+non_hitbox_height;
        }
        else
        {
            int m = (lightsource[0] < pos[0])?-1:1;

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
        int dx = vx[i]-lightsource[0], dy = vy[i]-lightsource[1];
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

void Object::render_light()
{
    SDL_Rect r={pos[0]-light_size[0]/2+window[0]/2-camera_pos[0], pos[1]-light_size[1]/2+window[1]/2-camera_pos[1], light_size[0], light_size[1]};

    SDL_RenderCopy(renderer, light_tex[cur_light_frame], nullptr, &r);
}

Player::Player() : Object(20,20,"front1",4,"",false,false,false,1)
{
    anim_progress = 0;
    persistent = true;

    SDL_SetTextureBlendMode(light_tex[0],SDL_BLENDMODE_BLEND);
}

void Player::update()
{
    lastpos[0]=pos[0]; lastpos[1]=pos[1];

    keystate = SDL_GetKeyboardState(nullptr);

    const int anim_speed=5, move_speed = 2;

    if (keystate[SDL_SCANCODE_D]) pos[0]+=move_speed;
    if (keystate[SDL_SCANCODE_A]) pos[0]-=move_speed;
    if (keystate[SDL_SCANCODE_S]) pos[1]+=move_speed;
    if (keystate[SDL_SCANCODE_W]) pos[1]-=move_speed;

    if (keystate[SDL_SCANCODE_S] && !keystate[SDL_SCANCODE_W])
    {
        anim_progress++;
        anim_progress%=6*anim_speed;
        tex = load_image("front"+std::to_string(anim_progress/anim_speed+1));
        light_tex[0] = load_image("light_front"+std::to_string(anim_progress/anim_speed+1));
    }
    else if (keystate[SDL_SCANCODE_W] && !keystate[SDL_SCANCODE_S])
    {
        anim_progress++;
        anim_progress%=6*anim_speed;
        tex = load_image("back"+std::to_string(anim_progress/anim_speed+1));
        light_tex[0] = load_image("light_back"+std::to_string(anim_progress/anim_speed+1));
    }
    else if (keystate[SDL_SCANCODE_A] && !keystate[SDL_SCANCODE_D])
    {
        anim_progress++;
        anim_progress%=6*anim_speed;
        tex = load_image("left"+std::to_string(anim_progress/anim_speed+1));
        light_tex[0] = load_image("light_left"+std::to_string(anim_progress/anim_speed+1));
    }
    else if (keystate[SDL_SCANCODE_D] && !keystate[SDL_SCANCODE_A])
    {
        anim_progress++;
        anim_progress%=6*anim_speed;
        tex = load_image("right"+std::to_string(anim_progress/anim_speed+1));
        light_tex[0] = load_image("light_right"+std::to_string(anim_progress/anim_speed+1));
    }

    int color_mod = script_variables["pitch_black"]?25:((script_variables.count("not_that_dark") && script_variables["not_that_dark"])?102:51);
    SDL_SetTextureColorMod(light_tex[0],color_mod,color_mod,color_mod);
}

Dialog_box::Dialog_box(int x, int y, std::string t, int speed, std::string portrait_image) : Object(x,y,"Dialog_Box",0,"",false,true)
{
    text = t;
    progress = 0;
    type_speed = speed;

    use_camera = false;
    persistent = true;

    portrait[0] = load_image(portrait_image+"1");
    portrait[1] = load_image(portrait_image+"2");

    SDL_QueryTexture(portrait[0], nullptr, nullptr, &portrait_size[0], &portrait_size[1]);
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

    SDL_Rect r = {pos[0]-size[0]/2+15, pos[1]+size[1]/2-portrait_size[0]-6, portrait_size[0], portrait_size[1]};

    SDL_RenderCopy(renderer,portrait[(progress < text.size()*type_speed)*(progress/8)%2],nullptr, &r);

    render_text(pos[0]-size[0]/2+20+portrait_size[0], pos[1]-size[1]/2+10, text.substr(0,progress/type_speed), 0);
}
