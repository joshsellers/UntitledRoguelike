#include "Altar.h"
#include "../World.h"
#include "../../core/SoundManager.h"
#include "../../core/MessageManager.h"

Altar::Altar(sf::Vector2f pos, bool isActivated, std::shared_ptr<sf::Texture> spriteSheet) : 
Entity(NO_SAVE, pos, 0, 96, 112, true), _isActivated(isActivated) {
    _pos = pos;
    loadSprite(spriteSheet);

    _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x + 3;
    collider.top = _pos.y + 52;
    collider.width = 96 - 6;
    collider.height = 36;
    _colliders.push_back(collider);

    _entityType = "altar";
}

void Altar::update() {
    if (!_isActivated && !_isActivating) {
        sf::Vector2f playerPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH);
        sf::Vector2f altarCenter(getPosition().x + 96 / 2, getPosition().y + 72);

        constexpr float triggerDistSquared = 10000.f;
        const float distSquared = std::pow(playerPos.x - altarCenter.x, 2) + std::pow(playerPos.y - altarCenter.y, 2);

        if (distSquared <= triggerDistSquared) {
            _isActivating = true;
            SoundManager::playSound("altaractivation");
        }
    } else if (_isActivating) {
        constexpr int ticksPerFrame = 4;
        constexpr int frameCount = 8;
        _activationStep = ((_animationCounter / ticksPerFrame) % frameCount);

        if (_activationStep == frameCount - 1) {
            _isActivating = false;
            _isActivated = true;
            onActivation();
        }
    }

    if (_isActivated || _isActivating) _animationCounter++;
}

void Altar::draw(sf::RenderTexture& surface) {
    if (_isActivating) {
        const sf::Vector2i textureCoords(64 * TILE_SIZE, 8 * TILE_SIZE);
        int xOffset = _activationStep * TILE_SIZE * 6;

        _sprite.setTextureRect(sf::IntRect(
            textureCoords.x + xOffset,
            textureCoords.y,
            96, 112
        ));
    } else if (_isActivated) {
        const sf::Vector2i textureCoords(64 * TILE_SIZE, 16 * TILE_SIZE);
        constexpr int ticksPerFrame = 3;
        constexpr int frameCount = 6;
        int xOffset = ((_animationCounter / ticksPerFrame) % frameCount) * TILE_SIZE * 6;

        _sprite.setTextureRect(sf::IntRect(
            textureCoords.x + xOffset,
            textureCoords.y,
            96, 112
        ));
    }

    surface.draw(_sprite);
}

void Altar::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(64 * TILE_SIZE, 8 * TILE_SIZE, 96, 112)
    );
    _sprite.setPosition(getPosition());
}

void Altar::onActivation() {
    unsigned int seed = _pos.x + _pos.y * (_pos.x - _pos.y);
    srand(seed);

    enum REWARD {
        ARMOR,
        DAMAGE,
        HP,
        STAMINA,
        MUSHROOM,
        BAD_VIBES,
        LIGHTNING
    };

    const auto& player = _world->getPlayer();

    REWARD reward = (REWARD)randomInt(0, 6);
    switch (reward) {
        case ARMOR:
        {
            player->getInventory().addItem(Item::MATMURA_HELMET.getId(), 1);
            player->getInventory().addItem(Item::MATMURA_CHESTPLATE.getId(), 1);
            player->getInventory().addItem(Item::MATMURA_LEGGINGS.getId(), 1);
            player->getInventory().addItem(Item::MATMURA_BOOTS.getId(), 1);

            MessageManager::displayMessage("A powerful armor has been bestowed upon you", 5);
            break;
        }
        case DAMAGE:
        {
            const float damageIncrease = (float)randomInt(100, 200) / 100.f;
            player->increaseDamageMultiplier(damageIncrease);

            MessageManager::displayMessage("You have been blessed with power", 5);
            break;
        }
        case HP:
        {
            const int maxHpIncrease = randomInt(50, 100);
            player->setMaxHitPoints(player->getMaxHitPoints() + maxHpIncrease);
            player->heal(player->getMaxHitPoints());

            MessageManager::displayMessage("You have been granted the gift of good health", 5);
            break;
        } 
        case STAMINA:
        {
            const int maxStaminaIncrease = randomInt(500, 1000);
            const int restoreIncrease = randomInt(20, 50);
            player->setMaxStamina(player->getMaxStamina() + maxStaminaIncrease);
            player->increaseStaminaRefreshRate(restoreIncrease);

            MessageManager::displayMessage("You have been honored with endurance", 5);
            break;
        }
        case MUSHROOM:
        {
            const int mushroomAmount = randomInt(20, 50);
            player->getInventory().addItem(Item::getIdFromName("Funny Mushroom"), mushroomAmount);

            MessageManager::displayMessage("You have been granted " + std::to_string(mushroomAmount) + " samples of a powerful fungus", 5);
            break;
        }
        case BAD_VIBES:
        {
            const int badVibesAmount = randomInt(15, 25);
            player->getInventory().addItem(Item::BAD_VIBES_POTION.getId(), badVibesAmount);

            MessageManager::displayMessage(std::to_string(badVibesAmount) + " bottles of a powerful potion have been imparted unto you", 5);
            break;
        }
        case LIGHTNING:
        {
            const int lightningAmount = randomInt(5, 10);
            player->getInventory().addItem(Item::getIdFromName("Thor's phone"), lightningAmount);

            MessageManager::displayMessage("You have been gifted implements used by the gods", 5);
            break;
        }
    }

    getWorld()->altarActivatedAt(getPosition());
}
