#ifndef _OBJECT
#define _OBJECT

#include <deque>
#include <SDL.h>
#include <string>

class Object
{
public:
    int pos[2], size[2];
    bool blocks;
    SDL_Texture* tex;

    Object(int x, int y, std::string s, bool b);
    virtual ~Object();
    bool collision(int x, int y, int sx, int sy);
    virtual void update();
    virtual void render();
    virtual void interact();
};

extern std::deque<Object*> objects;

class Dialog_box : public Object
{
public:
    std::string text;
    int progress;

    Dialog_box(int x, int y, std::string t);
    void update();
    void render();
};

class Player: public Object
{
public:
    int lastpos[2];

    Player() : Object(10,10,"Test",false) {}
    void update();
};
extern Player* player;

#endif // _OBEJECT
