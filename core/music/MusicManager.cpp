#include "MusicManager.h"
#include <thread>
#include "../SoundManager.h"

void MusicManager::start() {
    _isHalted = false; 
}

void MusicManager::shutdown() {
    _isHalted = true;
}

void MusicManager::setSituation(MUSIC_SITUTAION situation) {
    _situation = situation;
}

MUSIC_SITUTAION MusicManager::getSituation() {
    return _situation;
}

void MusicManager::run() {

}
