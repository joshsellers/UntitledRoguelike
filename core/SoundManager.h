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
            MessageManager::displayMessage("No sound named \"" + soundName + "\"", _failedInit ? 0 : 5, ERR);
            return;
        }
        soloud.play(sounds[soundName]);
    }

    static void setVolume(std::string soundName, float volume) {
        sounds[soundName].setVolume(volume);
    }

    static void playSong(std::string musicName) {
        stopMusic();
        _currentSong = soloud.play(music[musicName]);
        _songIsPlaying = true;
        music[musicName].setLooping(true);
    }

    static void stopSong(std::string songName) {
        music[songName].stop();
        _songIsPlaying = false;
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

        if (_songIsPlaying) {
            soloud.setVolume(_currentSong, volume);
        }
    }

    static void setSfxVolume(float volume) {
        for (auto& sound : sounds) {
            sound.second.setVolume(volume);
        }
    }

    static void loadSounds() {
        const SoLoud::result result = soloud.init();
        if (result != 0) {
            MessageManager::displayMessage("There was an error initiating the audio engine\nAborting audio init\n\nSounds will not be loaded.\nTry changing your audio device and restarting the game.", 20, ERR);
            MessageManager::displayMessage("Error code: " + std::to_string(result), 10, DEBUG);
            soloud.deinit();
            _failedInit = true;
            return;
        }

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

        const std::string musicDirName = "res/sounds/music";
        std::vector<std::string> musicFiles;
        for (const auto& entry : std::filesystem::directory_iterator(musicDirName)) {
            if (splitString(splitString(entry.path().string(), "\\")[1], ".").size() != 2) continue;
            else if (splitString(splitString(entry.path().string(), "\\")[1], ".")[1] != "wav") continue;
            musicFiles.push_back(splitString(entry.path().string(), "\\")[1]);
        }

        for (const auto& musicFile : musicFiles) {
            std::string songName = splitString(musicFile, ".")[0];
            musicNames.push_back(songName);
        }

        for (int i = 0; i < musicNames.size(); i++) {
            std::string filePath = "res/sounds/music/" + musicNames[i] + ".wav";
            if (music[musicNames[i]].load(filePath.c_str()) != 0) {
                MessageManager::displayMessage("Could not load " + musicNames[i] + ".wav", 10, WARN);
            } else {
                music[musicNames[i]].setSingleInstance(true);
                music[musicNames[i]].setLooping(true);
            }
        }
    }

    static void shutdown() {
        soloud.deinit();
    }

    friend class MusicManager;
private:
    inline static SoLoud::Soloud soloud;
    inline static bool _failedInit = false;

    inline static std::map<std::string, SoLoud::Wav> sounds;
    inline static std::vector<std::string> soundNames;

    inline static std::map<std::string, SoLoud::Wav> music;
    inline static std::vector<std::string> musicNames;

    inline static SoLoud::handle _currentSong;
    inline static bool _songIsPlaying = false;
};

#endif