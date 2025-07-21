#include "PlayerVisualEffectManager.h"
#include "../../core/MessageManager.h"
#include "../../core/ShaderManager.h"

void PlayerVisualEffectManager::drawEffects(Player* player, sf::RenderTexture& surface) {
    if (!player->isDodging() || !player->isMoving()) {
        for (const unsigned int effectId : _playerEffects) {
            const PlayerVisualEffect& effect = _effectTypes.at(effectId);

            sf::Vector2f playerPos = player->getPosition();

            constexpr int spriteHeight = 3;
            int yOffset = player->isMoving() || player->isSwimming() || (playerHasEffect("Dev's Blessing") && effect.name == "Dev's Blessing") ? ((player->_numSteps >> player->_animSpeed) & 3) * TILE_SIZE * spriteHeight : 0;
            if (playerHasEffect("Dev's Blessing") && effect.name != "Dev's Blessing") yOffset = 0;

            sf::IntRect effectTextureRect(effect.texturePos.x, effect.texturePos.y, spriteHeight * TILE_SIZE, (player->isSwimming() ? TILE_SIZE * 2 : spriteHeight * TILE_SIZE));
            int spriteY = effectTextureRect.top;

            _sprite.setTextureRect(sf::IntRect(
                effectTextureRect.left + TILE_SIZE * 3 * player->_facingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );

            _sprite.setPosition(sf::Vector2f(player->getSprite().getPosition().x - TILE_SIZE, player->getSprite().getPosition().y - TILE_SIZE));
            surface.draw(_sprite, player->isTakingDamage() ? ShaderManager::getShader("damage_frag") : sf::RenderStates::Default);
        }
    }
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

bool PlayerVisualEffectManager::playerHasEffect(std::string effectName) {
    for (auto& effectType : _effectTypes) {
        if (effectType.name == effectName) {
            for (unsigned int& playerEffectId : _playerEffects) {
                if (effectType.id == playerEffectId) {
                    return true;
                }
            }
        }
    }
    return false;
}

void PlayerVisualEffectManager::clearPlayerEffects() {
    _playerEffects.clear();
}

void PlayerVisualEffectManager::addEffectType(PlayerVisualEffect effect) {
    _effectTypes.push_back(effect);
}

unsigned int PlayerVisualEffectManager::getEffectCount() {
    return _effectTypes.size();
}

void PlayerVisualEffectManager::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
}
