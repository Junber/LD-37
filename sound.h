#ifndef _SOUND
#define _SOUND

#include <SDL_mixer.h>

void sound_init();
void play_sound(Mix_Chunk* s, bool loops=false);
void play_music(Mix_Music* s);
void stop_sound();

#endif // _SOUND
