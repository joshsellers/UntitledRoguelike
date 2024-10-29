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
        if (sounds.find(soundName) == sounds.end()) {
            MessageManager::displayMessage("No sound named \"" + soundName + "\"", 5, ERR);
            return;
        }
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
        
        const std::string dirName = "res/sounds";
        std::vector<std::string> soundFiles;
        for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
            if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
            else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "wav") continue;
            soundFiles.push_back(splitString(entry.path().string(), "\\")[1]);
        }

        for (const auto& soundFile : soundFiles) {
            std::string soundName = splitString(soundFile, ".")[0];
            soundNames.push_back(soundName);
        }

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
    inline static std::vector<std::string> soundNames;

    inline static std::map<std::string, SoLoud::Wav> music;
    inline static std::vector<std::string> musicNames = {
        "cooldown_0", "mainmenu"
    };
};

#endif