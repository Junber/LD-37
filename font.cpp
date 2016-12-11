#include "font.h"
#include "loading.h"
#include "base_functions.h"
#include <map>
#include <deque>

TTF_Font* font;
void font_init()
{
    TTF_Init();
    font = TTF_OpenFont((std::string("Data")+PATH_SEPARATOR+"Fonts"+PATH_SEPARATOR+"Yantramanav-Medium.ttf").c_str(),12);
}

std::map<std::pair<std::string,Uint8>,SDL_Texture*> loaded_texts;
SDL_Texture* text_to_texture(std::string s, Uint8 brightness)
{
    std::pair<std::string,Uint8> p = {s, brightness};
    if (!loaded_texts.count(p)) loaded_texts[p] = SDL_CreateTextureFromSurface(renderer,TTF_RenderText_Solid(font, s.c_str(), {brightness,brightness,brightness,255}));
    return loaded_texts[p];
}

void render_text(int posx, int posy, std::string s, Uint8 brightness)
{
    int offset = 0;
    std::deque<std::string> splitt = split(s,'*');
    for (auto st: splitt)
    {
        SDL_Texture* tex =text_to_texture(st, brightness);
        int size[2];
        SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);
        SDL_Rect r = {posx*renderzoom, (posy+offset)*renderzoom, size[0]*renderzoom, size[1]*renderzoom};
        SDL_RenderCopy(renderer, tex, nullptr, &r);

        offset += size[1]+5;
    }
}
