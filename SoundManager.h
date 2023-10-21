#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

#include "SFML/Audio.hpp"
#include <map>
#include <iostream>

class SoundManager {
public:
    static void playSound(std::string soundName) {
        sounds[soundName].play();
    }

    static void loadSounds() {
        for (int i = 0; i < soundNames.size(); i++) {
            if (!buffer.loadFromFile("res/sounds/" + soundNames[i] + ".wav")) {
                std::cout << "Could not load " + soundNames[i] + ".wav" << std::endl;
            } else {
                sf::Sound sound;
                sound.setBuffer(buffer);
                sounds[soundNames[i]] = sound;
            }
        }
    }
private:
    inline static sf::SoundBuffer buffer;
    inline static std::map<std::string, sf::Sound> sounds;

    inline static std::vector<std::string> soundNames = {
        "revolver"
    };
};

#endif