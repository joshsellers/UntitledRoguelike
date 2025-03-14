#include "MegaBombBoy.h"
#include "../World.h"
#include "Explosion.h"
#include "projectiles/ProjectileDataManager.h"
#include "projectiles/Projectile.h"

MegaBombBoy::MegaBombBoy(sf::Vector2f pos) : Entity(MEGA_BOMB_BOY, pos, 4.f, 3 * TILE_SIZE, 3 * TILE_SIZE, false) {
    setMaxHitPoints(275);
    heal(getMaxHitPoints());

    _hitBoxXOffset = -((float)TILE_SIZE * 3.f / 2.f) + 5.f;
    _hitBoxYOffset = 8;
    _hitBox.width = 40;
    _hitBox.height = 40;

    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

    _canPickUpItems = false;
    _isMob = true;
    _isEnemy = true;

    _entityType = "megabombboy";

    /*srand(currentTimeNano());
    unsigned int pennyAmount = randomInt(0, 2);
    if (pennyAmount > 0) getInventory().addItem(Item::PENNY.getId(), pennyAmount);*/
}

void MegaBombBoy::update() {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;

    sf::Vector2f goalPos((int)_world->getPlayer()->getPosition().x + PLAYER_WIDTH / 2, (int)_world->getPlayer()->getPosition().y + PLAYER_WIDTH * 2);
    sf::Vector2f cLoc(((int)getPosition().x), ((int)getPosition().y) + TILE_SIZE * 3);

    const float distSquared = std::pow(goalPos.x - cLoc.x, 2) + std::pow(goalPos.y - cLoc.y, 2);
    constexpr float explosionDistance = 128.f * 128.f;
    if (distSquared < explosionDistance) {
        explode();
        deactivate();
    }

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
        _hitBoxYOffset = 8 + TILE_SIZE;
        _hitBox.height = 40 - TILE_SIZE;
    } else {
        _hitBoxYOffset = 8;
        _hitBox.height = 40;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;
}

void MegaBombBoy::explode() {
    constexpr int damage = 15;
    const sf::Vector2f spawnPos(getPosition().x, getPosition().y + ((float)TILE_SIZE * 3.f / 2.f) - ((float)TILE_SIZE * 3 / 2));
    const auto explosion = std::shared_ptr<Explosion>(new Explosion(spawnPos, damage, true, true));
    explosion->setWorld(getWorld());
    explosion->loadSprite(getWorld()->getSpriteSheet());
    getWorld()->addEntity(explosion);
    
    constexpr int projectilesPerBlast = 30;
    float fireAngle = (float)randomInt(0, 360);
    for (int i = 0; i < projectilesPerBlast; i++) {
        fireAngle += 360.f / projectilesPerBlast;
        if (fireAngle >= 360.f) fireAngle -= 360.f;

        const float fireAngleRads = fireAngle * ((float)PI / 180.f);
        Projectile* proj = fireTargetedProjectile(fireAngleRads, ProjectileDataManager::getData("_PROJECTILE_BOMB"), "NONE", true, false, {0, 0}, false);
        if (proj == nullptr) continue;
        proj->optimizedExplosions = true;
    }
}

void MegaBombBoy::draw(sf::RenderTexture& surface) {
    sf::Vector2f feetPos = getPosition();
    feetPos.y += TILE_SIZE * 3;
    TERRAIN_TYPE terrainType = _world->getTerrainDataAt(
        feetPos
    );
    _isSwimming = terrainType == TERRAIN_TYPE::WATER;

    if (isSwimming()) {
        _sprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + PLAYER_HEIGHT / 2));

        int xOffset = ((_numSteps >> _animSpeed) & 1) * TILE_SIZE;

        _wavesSprite.setTextureRect(sf::IntRect(xOffset, 160, TILE_SIZE, TILE_SIZE));
        _wavesSprite.setPosition(sf::Vector2f(getPosition().x - (float)TILE_SIZE * 3.f / 2.f, getPosition().y + (TILE_SIZE*2)+9));
        surface.draw(_wavesSprite);
    }

    int xOffset = getMovingDir() == UP ? TILE_SIZE * 3 : 0;
    int yOffset = isMoving() ? ((_numSteps >> _animSpeed) & 7) * TILE_SIZE * 3 : 0;

    _sprite.setTextureRect(sf::IntRect(
        97 * TILE_SIZE + xOffset, 40 * TILE_SIZE + yOffset, TILE_SIZE * 3, TILE_SIZE * (isSwimming() ? 2 : 3)
    ));
    if (getMovingDir() == LEFT) _sprite.setScale(-1, 1);
    else _sprite.setScale(1, 1);

    surface.draw(_sprite);
}

void MegaBombBoy::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(97 * TILE_SIZE, 40 * TILE_SIZE, TILE_SIZE * 3, TILE_SIZE * 3));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 3.f / 2, 0);

    _wavesSprite.setTexture(*spriteSheet);
    _wavesSprite.setTextureRect(sf::IntRect(0, 160, TILE_SIZE, TILE_SIZE));
    _wavesSprite.setPosition(sf::Vector2f(getPosition().x, getPosition().y + TILE_SIZE));
}

void MegaBombBoy::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        explode();
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
        }
    }
}
