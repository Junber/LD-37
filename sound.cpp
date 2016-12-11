#include "sound.h"
#include "loading.h"
#include <iostream>

void sound_init()
{
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    play_music(load_music("Shut-in"));
}

void play_sound(Mix_Chunk* s)
{
    if (s!= nullptr) Mix_PlayChannel(-1, s, 0);
}

void play_music(Mix_Music* s)
{
    if (s!= nullptr) Mix_PlayMusic(s,-1);
}
