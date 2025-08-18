#include "FinalBossEffectManager.h"
#include "Util.h"

void FinalBossEffectManager::activateEffect(FINAL_BOSS_EFFECT effect, long long duration) {
    if (effectIsActive(effect)) return;
    _activeEffects.push_back({ effect, duration, currentTimeMillis() });
}

bool FinalBossEffectManager::effectIsActive(FINAL_BOSS_EFFECT effect) {
    _activeEffects.erase(std::remove_if(_activeEffects.begin(), _activeEffects.end(),
        [](FinalBossEffect effect) {
            return currentTimeMillis() - effect.startTime >= effect.duration;
        }), _activeEffects.end());

    for (const auto& activeEffect : _activeEffects) 
        if (activeEffect.id == effect) return true;

    return false;
}

void FinalBossEffectManager::reset() {
    _activeEffects.clear();
}
