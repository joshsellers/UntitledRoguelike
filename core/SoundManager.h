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

    static void playSong(std::string musicName) {
        stopMusic();
        soloud.play(music[musicName]);
        music[musicName].setLooping(true);
    }

    static void stopSong(std::string songName) {
        music[songName].stop();
    }

    static void stopMusic() {
        for (const auto& songName : musicNames) {
            stopSong(songName);
        }
    }

    static void setSongVolume(std::string songName, float volume) {
        music[songName].setVolume(volume);
    }

    static void setMusicVolume(float volume) {
        for (auto& song : music) {
            song.second.setVolume(volume);
        }
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

        for (int i = 0; i < musicNames.size(); i++) {
            std::string filePath = "res/sounds/music/" + musicNames[i] + ".wav";
            if (music[musicNames[i]].load(filePath.c_str()) != 0) {
                MessageManager::displayMessage("Could not load " + musicNames[i] + ".wav", 10, WARN);
            } else music[musicNames[i]].setSingleInstance(true);
        }
    }

    static void shutdown() {
        soloud.deinit();
    }

    friend class MusicManager;
private:
    inline static SoLoud::Soloud soloud;

    inline static std::map<std::string, SoLoud::Wav> sounds;
    inline static std::vector<std::string> soundNames = {
        "revolver", "slip", "ar", "laser_pistol", "blowtorch", "coinpickup", "railgun", "chainsaw"
    };

    inline static std::map<std::string, SoLoud::Wav> music;
    inline static std::vector<std::string> musicNames = {
        "cooldown_0", "mainmenu"
    };
};

#endif