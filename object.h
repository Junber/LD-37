#ifndef _OBJECT
#define _OBJECT

#include <deque>
#include <SDL.h>
#include <string>

class Object
{
public:
    int pos[2], size[2], non_hitbox_height;
    bool blocks, foreground, throws_shadow;
    std::deque<std::string> script;
    SDL_Texture* tex;

    Object(int x, int y, std::string s, int hitbox_height, std::string script_file, bool shadow);
    virtual ~Object();
    bool collision(int x, int y, int sx, int sy, int nhh);
    virtual void update();
    virtual void render();
    void render_shadow(int darkness_color);
    virtual bool interact(bool touch);
};

extern std::deque<Object*> objects, to_delete;
extern int camera_pos[2];

class Dialog_box : public Object
{
public:
    std::string text;
    unsigned progress, type_speed;

    Dialog_box(int x, int y, std::string t, int speed);
    void update();
    void render();
    bool interact(bool touch);
};

class Player: public Object
{
public:
    int lastpos[2];

    Player() : Object(20,20,"player",4,"",false) {}
    void update();
};
extern Player* player;

#endif // _OBEJECT
