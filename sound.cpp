#include "sound.h"
#include "loading.h"

void sound_init()
{
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    play_music(load_music("Shut-in"));
}

int looping_channel;
void play_sound(Mix_Chunk* s, bool loops)
{
    if (s!= nullptr)
    {
        if (loops) looping_channel = Mix_PlayChannel(-1, s, -1);
        else Mix_PlayChannel(-1, s, 0);
    }
}

void stop_sound()
{
    Mix_HaltChannel(looping_channel);
}

void play_music(Mix_Music* s)
{
    if (s!= nullptr) Mix_PlayMusic(s,-1);
}
