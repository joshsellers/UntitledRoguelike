#ifndef _PLAYER_VISUAL_EFFECT_MANAGER_H
#define _PLAYER_VISUAL_EFFECT_MANAGER_H

#include <vector>
#include "PlayerVisualEffect.h"
#include "../../world/entities/Player.h"

class PlayerVisualEffectManager {
public:
    static void drawEffects(Player* player, sf::RenderTexture surface);

    static void addEffectToPlayer(std::string effectName);
    static std::vector<unsigned int> getPlayerEffects();

    static void addEffectType(PlayerVisualEffect effect);
private:
    static inline std::vector<PlayerVisualEffect> _effectTypes;
    static inline std::vector<unsigned int> _playerEffects;
};

#endif