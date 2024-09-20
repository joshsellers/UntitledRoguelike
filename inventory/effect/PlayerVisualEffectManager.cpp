#include "PlayerVisualEffectManager.h"
#include "../../core/MessageManager.h"

void PlayerVisualEffectManager::drawEffects(Player* player, sf::RenderTexture surface) {
}

void PlayerVisualEffectManager::addEffectToPlayer(std::string effectName) {
    for (auto& effectType : _effectTypes) {
        if (effectType.name == effectName) {
            for (unsigned int& playerEffectId : _playerEffects) {
                if (effectType.id == playerEffectId) {
                    return;
                }
            }

            _playerEffects.push_back(effectType.id);
            return;
        }
    }

    MessageManager::displayMessage("No effect named \"" + effectName + "\"", 5, WARN);
}

std::vector<unsigned int> PlayerVisualEffectManager::getPlayerEffects() {
    return _playerEffects;
}

void PlayerVisualEffectManager::addEffectType(PlayerVisualEffect effect) {
    _effectTypes.push_back(effect);
}
