#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>

class Sound {
public:
    Sound();
    ~Sound();

    bool init();
    void playHit();

private:
    Mix_Chunk* hitSound;
};

#endif

