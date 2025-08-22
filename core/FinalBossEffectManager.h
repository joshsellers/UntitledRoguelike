#ifndef _FINAL_BOSS_EFFECT_MANAGER_H
#define _FINAL_BOSS_EFFECT_MANAGER_H

#include <vector>

enum FINAL_BOSS_EFFECT {
    SLOW_BULLETS,
    ANTIHOMING_BULLETS,
    INVERT_CONTROLS,
    MONOCHROME_TERRAIN
};

struct FinalBossEffect {
    FinalBossEffect(const FINAL_BOSS_EFFECT id, const long long duration, const long long startTime) : id(id), duration(duration), startTime(startTime) {}

    FINAL_BOSS_EFFECT id;
    long long duration;
    long long startTime;
};

class FinalBossEffectManager {
public:
    static void activateEffect(FINAL_BOSS_EFFECT effect, long long duration);

    static bool effectIsActive(FINAL_BOSS_EFFECT effect);

    static void reset();

    static inline bool devBossIsActive = false;
private:
    static inline std::vector<FinalBossEffect> _activeEffects;
};

#endif