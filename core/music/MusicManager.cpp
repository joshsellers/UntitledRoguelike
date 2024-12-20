#include "MusicManager.h"
#include <thread>
#include "../SoundManager.h"

void MusicManager::start() {
    _isHalted = false;

    for (const auto& songName : SoundManager::musicNames) {
        if (stringStartsWith(songName, "mainmenu")) {
            _mainMenuSongs.push_back(songName);
        } else if (stringStartsWith(songName, "cooldown")) {
            _cooldownSongs.push_back(songName);
        } else if (stringStartsWith(songName, "wave")) {
            _waveSongs.push_back(songName);
        } else if (stringStartsWith(songName, "boss")) {
            _bossSongs.push_back(songName);
        } else if (stringStartsWith(songName, "shop")) {
            _shopSongs.push_back(songName);
        } else if (stringStartsWith(songName, "death")) {
            _deathSongs.push_back(songName);
        }
    }

    std::thread musicThread(MusicManager::run);
    musicThread.detach();
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
    while (!_isHalted) {
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(250.f)));

        if (_situation != _lastSituation) {
            _lastSituation = _situation;
            SoundManager::stopMusic();

            switch (_situation) {
                case MUSIC_SITUTAION::MAIN_MENU:
                    if (_mainMenuSongs.size() > 1) {
                        SoundManager::playSong(_mainMenuSongs.at(randomInt(0, _mainMenuSongs.size() - 1)));
                    } else if (_mainMenuSongs.size() == 1) {
                        SoundManager::playSong(_mainMenuSongs.at(0));
                    }
                    break;
                case MUSIC_SITUTAION::COOLDOWN:
                    if (_cooldownSongs.size() > 1) {
                        SoundManager::playSong(_cooldownSongs.at(randomInt(0, _cooldownSongs.size() - 1)));
                    } else if (_cooldownSongs.size() == 1) {
                        SoundManager::playSong(_cooldownSongs.at(0));
                    }
                    break;
                case MUSIC_SITUTAION::WAVE:
                    if (_waveSongs.size() > 1) {
                        SoundManager::playSong(_waveSongs.at(randomInt(0, _waveSongs.size() - 1)));
                    } else if (_waveSongs.size() == 1) {
                        SoundManager::playSong(_waveSongs.at(0));
                    }
                    break;
                case MUSIC_SITUTAION::BOSS:
                    if (_bossSongs.size() > 1) {
                        SoundManager::playSong(_bossSongs.at(randomInt(0, _bossSongs.size() - 1)));
                    } else if (_bossSongs.size() == 1) {
                        SoundManager::playSong(_bossSongs.at(0));
                    }
                    break;
                case MUSIC_SITUTAION::SHOP:
                    if (_shopSongs.size() > 1) {
                        SoundManager::playSong(_shopSongs.at(randomInt(0, _shopSongs.size() - 1)));
                    } else if (_shopSongs.size() == 1) {
                        SoundManager::playSong(_shopSongs.at(0));
                    }
                    break;
                case MUSIC_SITUTAION::DEATH:
                    if (_deathSongs.size() > 1) {
                        SoundManager::playSong(_deathSongs.at(randomInt(0, _deathSongs.size() - 1)));
                    } else if (_deathSongs.size() == 1) {
                        SoundManager::playSong(_deathSongs.at(0));
                    }
                    break;
            }
        }
    }
}