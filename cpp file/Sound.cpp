
#include "Sound.h"
#include <iostream>

Sound::Sound() : hitSound(nullptr) {}

Sound::~Sound() {
    Mix_FreeChunk(hitSound);
    Mix_CloseAudio();
}

bool Sound::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Không thể khởi tạo âm thanh! Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    hitSound = Mix_LoadWAV("Sounds/hit.wav");
    if (!hitSound) {
        std::cout << "Không thể tải âm thanh! Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void Sound::playHit() {
    Mix_PlayChannel(-1, hitSound, 0);
}
