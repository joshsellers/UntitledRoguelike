#include "Funguy.h"
#include "../World.h"
#include "Spore.h"

Funguy::Funguy(sf::Vector2f pos) : Entity(FUNGUY, pos, 2.f, TILE_SIZE, TILE_SIZE, false) {
    setMaxHitPoints(45);
    heal(getMaxHitPoints());

    _entityType = "funguy";

    _hitBoxXOffset = -TILE_SIZE / 2;
    _hitBox.left = _pos.x + _hitBoxXOffset;
    _hitBox.top = _pos.y;
    _hitBox.width = TILE_SIZE;
    _hitBox.height = TILE_SIZE;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 5);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);
}

void Funguy::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;

    if (getWorld()->getPlayer()->isActive() && getWorld()->getPlayer()->getHitBox().intersects(getHitBox())) {
        getWorld()->getPlayer()->takeDamage(5);
        getWorld()->getPlayer()->knockBack(15.f, getMovingDir());
    }

    sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE);

    float xa = 0.f, ya = 0.f;
    if (goalPos.y < cLoc.y) {
        ya--;
    } else if (goalPos.y > cLoc.y) {
        ya++;
    }

    if (goalPos.x < cLoc.x) {
        xa--;
    } else if (goalPos.x > cLoc.x) {
        xa++;
    }

    if (xa && ya) {
        xa *= 0.785398;
        ya *= 0.785398;
    }

    hoardMove(xa, ya, true);

    _sprite.setPosition(getPosition());

    if (isSwimming()) {
        _hitBoxYOffset = 8;
        _hitBox.height = TILE_SIZE / 2;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void Funguy::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, TILE_SIZE, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - TILE_SIZE / 2, getPosition().y + (TILE_SIZE / 2)));
        surface.draw(_wavesSprite);
    }

    int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE : 0;

    _sprite.setTextureRect(sf::IntRect(
        89 * TILE_SIZE, 76 * TILE_SIZE + yOffset, TILE_SIZE, isSwimming() ? TILE_SIZE / 2 : TILE_SIZE
    ));

    surface.draw(_sprite);
}

void Funguy::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(89 * TILE_SIZE, 76 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE / 2.f, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, 16, 16));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE * 3.f / 2.f));
}

void Funguy::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }

        const int sporeCount = randomInt(50, 100);
        constexpr int cloudRadius = 16;

        for (int i = 0; i < sporeCount; i++) {
            const float r = cloudRadius * std::sqrt(static_cast<float>(rand()) / RAND_MAX);
            const float theta = static_cast<float>(rand()) / RAND_MAX * 2 * PI;
            const float x = r * std::cos(theta);
            const float y = r * std::sin(theta);

            auto& spore = std::shared_ptr<Spore>(new Spore(sf::Vector2f(_pos.x + x, _pos.y + y)));
            spore->setWorld(getWorld());
            spore->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(spore);
        }
    }
}
