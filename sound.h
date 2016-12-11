#ifndef _SOUND
#define _SOUND

#include <SDL_mixer.h>

void sound_init();
void play_sound(Mix_Chunk* s);
void play_music(Mix_Music* s);

#endif // _SOUND
