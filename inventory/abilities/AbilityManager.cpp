#include "AbilityManager.h"
#include "Ability.h"

bool AbilityManager::givePlayerAbility(unsigned int abilityId) {
    if (!playerHasAbility(abilityId)) {
        Ability::ABILITIES[abilityId]->givePlayerAbility();
        return true;
    }
    return false;
}

bool AbilityManager::playerHasAbility(unsigned int abilityId) {
    return Ability::ABILITIES[abilityId]->playerHasAbility();
}

bool AbilityManager::setParameter(unsigned int abilityId, std::string key, float val) {
    if (playerHasAbility(abilityId)) {
        Ability::ABILITIES[abilityId]->setParameter(key, val);
        return true;
    }

    return false;
}

float AbilityManager::getParameter(unsigned int abilityId, std::string key) {
    return Ability::ABILITIES[abilityId]->getParameter(key);
}

void AbilityManager::resetAbilities() {
    for (const auto& ability : Ability::ABILITIES) {
        ability->reset();
    }
}

void AbilityManager::updateAbilities(Player* player) {
    if (player->isActive()) {
        for (const auto& ability : Ability::ABILITIES) {
            if (playerHasAbility(ability->getId())) ability->update(player, ability);
        }
    }
}

void AbilityManager::drawAbilities(Player* player, sf::RenderTexture& surface) {
    if (player->isActive()) {
        for (const auto& ability : Ability::ABILITIES) {
            if (playerHasAbility(ability->getId())) ability->draw(player, ability, surface);
        }
    }
}

void AbilityManager::loadSprites(std::shared_ptr<sf::Texture> spriteSheet) {
    for (const auto& ability : Ability::ABILITIES) {
        ability->loadSprite(spriteSheet);
    }
}
