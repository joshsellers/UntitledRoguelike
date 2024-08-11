#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

#include <map>
#include <iostream>
#include "MessageManager.h"
#include "soloud.h"
#include "soloud_wav.h"

class SoundManager {
public:
    static void playSound(std::string soundName) {
        soloud.play(sounds[soundName]);
    }

    static void setVolume(std::string soundName, float volume) {
        sounds[soundName].setVolume(volume);
    }

    static void loadSounds() {
        soloud.init();

        for (int i = 0; i < soundNames.size(); i++) {
            std::string filePath = "res/sounds/" + soundNames[i] + ".wav";
            if (sounds[soundNames[i]].load(filePath.c_str()) != 0) {
                MessageManager::displayMessage("Could not load " + soundNames[i] + ".wav", 10, WARN);
            } else sounds[soundNames[i]].setSingleInstance(true);
        }

        setVolume("revolver", .25f);
    }

    static void shutdown() {
        soloud.deinit();
    }
private:
    inline static SoLoud::Soloud soloud;

    inline static std::map<std::string, SoLoud::Wav> sounds;

    inline static std::vector<std::string> soundNames = {
        "revolver", "slip", "ar", "laser_pistol", "blowtorch", "coinpickup", "railgun", "chainsaw"
    };
};

#endif