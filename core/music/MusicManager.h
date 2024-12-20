#ifndef _MUSIC_MANAGER_H
#define _MUSIC_MANAGER_H

#include "MusicSituation.h"
#include <vector>
#include <string>

class MusicManager {
public:
    static void start();
    static void shutdown();

    static void setSituation(MUSIC_SITUTAION situation);
    static MUSIC_SITUTAION getSituation();

private:
    static inline MUSIC_SITUTAION _lastSituation = MUSIC_SITUTAION::NONE;
    static inline MUSIC_SITUTAION _situation = MUSIC_SITUTAION::NONE;

    static void run();
    static inline bool _isHalted = false;

    static inline std::vector<std::string> _mainMenuSongs;
    static inline std::vector<std::string> _cooldownSongs;
    static inline std::vector<std::string> _waveSongs;
    static inline std::vector<std::string> _bossSongs;
    static inline std::vector<std::string> _shopSongs;
    static inline std::vector<std::string> _deathSongs;
};

#endif