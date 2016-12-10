#ifndef _SOUND
#define _SOUND

#include <SDL_mixer.h>

void sound_init();
void play_sound(Mix_Chunk* s);

#endif // _SOUND
