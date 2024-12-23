#ifndef _ABILITY_MANAGER_H
#define _ABILITY_MANAGER_H

#include "../../world/entities/Player.h"

class AbilityManager {
public:
    static bool givePlayerAbility(unsigned int abilityId);
    static bool playerHasAbility(unsigned int abilityId);

    static bool setParameter(unsigned int abilityId, std::string key, float val);
    static float getParameter(unsigned int abilityId, std::string key);

    static void resetAbilities();

    static void updateAbilities(Player* player);
    static void drawAbilities(Player* player, sf::RenderTexture& surface);

    static void loadSprites(std::shared_ptr<sf::Texture> spriteSheet);
};

#endif