#ifndef _OBJECT
#define _OBJECT

#include <deque>
#include <SDL.h>
#include <string>

class Object
{
public:
    int pos[2], size[2], light_size[2], non_hitbox_height, light_frames, cur_light_frame;
    bool blocks, foreground, throws_shadow, use_camera, is_zone, persistent;
    std::deque<std::string> script;
    SDL_Texture *tex, *light_tex[3]; //the 3 might need to be higher

    Object(int x, int y, std::string s, int hitbox_height, std::string script_file, bool shadow, bool in_foreground, bool zone=false, int light=0);
    virtual ~Object();
    bool collision(int x, int y, int sx, int sy, int nhh);
    virtual void update();
    virtual void render();
    void render_shadow(int darkness_color);
    virtual void render_light();
    virtual bool interact(bool touch);
};

extern std::deque<Object*> objects, to_delete, automatic_trigger;
extern int camera_pos[2];

class Dialog_box : public Object
{
public:
    std::string text;
    unsigned progress, type_speed;
    int portrait_size[2];
    SDL_Texture* portrait[2];

    Dialog_box(int x, int y, std::string t, int speed, std::string portrait_image);
    void update();
    void render();
    bool interact(bool touch);
};

class Player: public Object
{
public:
    int lastpos[2], anim_progress;

    Player();
    void update();
};
extern Player* player;

#endif // _OBEJECT
