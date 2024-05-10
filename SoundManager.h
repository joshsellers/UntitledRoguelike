#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

#include "SFML/Audio.hpp"
#include <map>
#include <iostream>
#include "MessageManager.h"

class SoundManager {
public:
    static void playSound(std::string soundName) {
        sounds[soundName].play();
    }

    static void setVolume(std::string soundName, float volume) {
        sounds[soundName].setVolume(volume);
    }

    static sf::SoundSource::Status getStatus(std::string soundName) {
        return sounds[soundName].getStatus();
    }

    static void loadSounds() {
        for (int i = 0; i < soundNames.size(); i++) {
            buffers[soundNames[i]] = sf::SoundBuffer();
            if (!buffers[soundNames[i]].loadFromFile("res/sounds/" + soundNames[i] + ".wav")) {
                MessageManager::displayMessage("Could not load " + soundNames[i] + ".wav", 10, WARN);
            } else {
                sf::Sound sound;
                sound.setBuffer(buffers[soundNames[i]]);
                sounds[soundNames[i]] = sound;
            }
        }

        setVolume("revolver", 25.f);
    }
private:
    inline static std::map<std::string, sf::SoundBuffer> buffers;
    inline static std::map<std::string, sf::Sound> sounds;

    inline static std::vector<std::string> soundNames = {
        "revolver", "slip", "ar", "laser_pistol", "blowtorch", "coinpickup"
    };
};

#endif