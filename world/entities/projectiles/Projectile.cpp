#include "Projectile.h"
#include "../../World.h"
#include "../../../statistics/StatManager.h"
#include "../DroppedItem.h"
#include "../../entities/Explosion.h"
#include "ProjectileDataManager.h"
#include "ProjectilePoolManager.h"
#include "../../../core/Viewport.h"

constexpr long long LIFETIME = 5000LL;

Projectile::Projectile(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data, bool onlyDamagePlayer, 
    int damageBoost, bool addParentVelocity, EXPLOSION_BEHAVIOR explosionBehavior) :
    Entity(PROJECTILE, pos, 0, 1, 1, false), _originalPos(pos), _parent(parent), _directionAngle(directionAngle), _velocity(velocity), _data(data), _damageBoost(damageBoost),
    _itemId(data.itemId), onlyDamagePlayer(onlyDamagePlayer), _explosionBehavior(explosionBehavior) {
    if (parent != nullptr) {
        sf::Vector2f shooterVelocity(parent->getVelocity().x, parent->getVelocity().y);

        _velocityComponents.x = _velocity * std::cos(directionAngle) + (addParentVelocity ? shooterVelocity.x : 0);
        _velocityComponents.y = _velocity * std::sin(directionAngle) + (addParentVelocity ? shooterVelocity.y : 0);

        _lifeTime = data.lifeTime;
        _spawnTime = currentTimeMillis();

        //setMaxHitPoints(1000000);
        //heal(getMaxHitPoints());

        _hitBoxXOffset = _data.hitBox.left;
        _hitBoxYOffset = _data.hitBox.top;
        _hitBox.width = _data.hitBox.width;
        _hitBox.height = _data.hitBox.height;

        if (_explosionBehavior == EXPLOSION_BEHAVIOR::DEFER_TO_DATA) _explosionBehavior = _data.explosionBehavior;
    } else {
        _lifeTime = 0;
        _spawnTime = 0;
    }
}

void Projectile::update() {
    if (currentTimeMillis() - _spawnTime >= _lifeTime) {
        if (_data.dropOnExpire) {
            std::shared_ptr<DroppedItem> droppedItem = 
                std::shared_ptr<DroppedItem>(new DroppedItem(getPosition(), 0, _data.itemId, 1, Item::ITEMS[_data.itemId]->getTextureRect()));
            droppedItem->setWorld(getWorld());
            droppedItem->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(droppedItem);
        }

        if (_explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT) {
            spawnExplosion();
        }

        if (_splitOnDecay) split();
        _isActive = false;

        return;
    }

    if (!onlyDamagePlayer && !_parent->isActive()) {
        deactivate();
        return;
    }

    if (!_data.noCollide) {
        if (onlyDamagePlayer) {
            if (_world->getPlayer()->getHitBox().intersects(getHitBox())) {
                _world->getPlayer()->takeDamage(Item::ITEMS[_itemId]->getDamage(), _criticalHit);
                if (_explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_IMPACT || _explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT) {
                    spawnExplosion();
                }

                if (_splitOnHit) split();

                _isActive = false;
                return;
            }
        } else if (_parent->getEntityType() != "player") {
            for (auto& entity : getWorld()->getEntities()) {
                if (!entity->compare(_parent) && entity->getHitBox() != getHitBox() && entity->isActive() && entity->isDamageable()
                    && (!_data.onlyHitEnemies || entity->isEnemy()) && !(_parent->getSaveId() == PLAYER && entity->getEntityType() == "dontblockplayershots")
                    && entity->getEntityType() != _parent->getEntityType()) {
                    if (entity->getHitBox().intersects(_hitBox)) {
                        entity->takeDamage((Item::ITEMS[_itemId]->getDamage() + _damageBoost) * _parent->getDamageMultiplier(), _criticalHit);
                        _entitiesPassedThrough++;
                        if (_entitiesPassedThrough >= passThroughCount) {
                            if (_explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_IMPACT || _explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT) {
                                spawnExplosion();
                            }

                            if (_splitOnHit) split();
                            _isActive = false;
                            return;
                        }
                        break;
                    }
                }
            }
        } else if (_data.onlyHitEnemies) {
            for (auto& entity : getWorld()->getEnemies()) {
                if (!entity->compare(_parent) && entity->getHitBox() != getHitBox() && entity->isActive() && entity->isDamageable()
                    && (!_data.onlyHitEnemies || entity->isEnemy()) && !(_parent->getSaveId() == PLAYER && entity->getEntityType() == "dontblockplayershots")
                    && entity->getEntityType() != _parent->getEntityType()) {
                    if (entity->getHitBox().intersects(_hitBox)) {
                        bool alreadyHitThisEntity = false;
                        for (const std::string& uid : _hitEntities) {
                            if (uid == entity->getUID()) {
                                alreadyHitThisEntity = true;
                                break;
                            }
                        }
                        if (alreadyHitThisEntity) continue;

                        int damage = (Item::ITEMS[_itemId]->getDamage() + _damageBoost) * (_data.useDamageMultiplier ? _parent->getDamageMultiplier() : 1);
                        entity->takeDamage(damage, _criticalHit);
                        StatManager::increaseStat(DAMAGE_DEALT, damage);

                        _entitiesPassedThrough++;
                        _hitEntities.push_back(entity->getUID());
                        if (_entitiesPassedThrough >= passThroughCount) {
                            if (_explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_IMPACT || _explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT) {
                                spawnExplosion();
                            }

                            if (_splitOnHit) split();
                            _isActive = false;
                            return;
                        }
                        break;
                    }
                }
            }
        } else {
            for (auto& entity : getWorld()->getEntities()) {
                if (!entity->compare(_parent) && entity->getHitBox() != getHitBox() && entity->isActive() && entity->isDamageable()
                    && (!_data.onlyHitEnemies || entity->isEnemy()) && !(_parent->getSaveId() == PLAYER && entity->getEntityType() == "dontblockplayershots")
                    && entity->getEntityType() != _parent->getEntityType()) {
                    if (entity->getHitBox().intersects(_hitBox)) {
                        bool alreadyHitThisEntity = false;
                        for (const std::string& uid : _hitEntities) {
                            if (uid == entity->getUID()) {
                                alreadyHitThisEntity = true;
                                break;
                            }
                        }
                        if (alreadyHitThisEntity) continue;

                        int damage = (Item::ITEMS[_itemId]->getDamage() + _damageBoost) * (_data.useDamageMultiplier ? _parent->getDamageMultiplier() : 1);
                        entity->takeDamage(damage, _criticalHit);
                        StatManager::increaseStat(DAMAGE_DEALT, damage);

                        _entitiesPassedThrough++;
                        _hitEntities.push_back(entity->getUID());
                        if (_entitiesPassedThrough >= passThroughCount) {
                            if (_explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_IMPACT || _explosionBehavior == EXPLOSION_BEHAVIOR::EXPLODE_ON_DECAY_AND_IMPACT) {
                                spawnExplosion();
                            }

                            if (_splitOnHit) split();
                            _isActive = false;
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }

    if (bounceOffViewport) {
        _baseSpeed = 1;
        move(_velocityComponents.x, _velocityComponents.y);
        sf::Vector2i spriteSize(_hitBox.width, _hitBox.height);

        const sf::FloatRect viewport = Viewport::getBounds();
        if (_pos.x <= viewport.left) {
            _pos.x = viewport.left;
            _velocityComponents.x = -_velocityComponents.x;
        } else if (_pos.x + spriteSize.x >= viewport.left + viewport.width) {
            _pos.x = viewport.left + viewport.width - spriteSize.x;
            _velocityComponents.x = -_velocityComponents.x;
        }

        const bool bounced =
            _pos.x <= viewport.left
            || _pos.x + spriteSize.x >= viewport.left + viewport.width
            || _pos.y - (float)spriteSize.y / 2 <= viewport.top
            || _pos.y + (float)spriteSize.y / 2 >= viewport.top + viewport.height;

        if (_pos.y - (float)spriteSize.y / 2 <= viewport.top) {
            _pos.y = viewport.top + (float)spriteSize.y / 2;
            _velocityComponents.y = -_velocityComponents.y;
        } else if (_pos.y + (float)spriteSize.y / 2 >= viewport.top + viewport.height) {
            _pos.y = viewport.top + viewport.height - (float)spriteSize.y / 2;
            _velocityComponents.y = -_velocityComponents.y;
        }

        if (bounced) {
            const float angle = radsToDeg(std::atan2(_velocityComponents.y, _velocityComponents.x));
            _sprite.setRotation(angle);
        }
    } else {
        _pos.x = _velocityComponents.x * (float)_currentTime + _originalPos.x;
        _pos.y = _velocityComponents.y * (float)_currentTime + _originalPos.y;
    }

    _sprite.setPosition(_pos);

    _hitBox.left = _sprite.getGlobalBounds().left + _hitBoxXOffset;
    _hitBox.top = _sprite.getGlobalBounds().top + _hitBoxYOffset;

    _currentTime++;
}

void Projectile::spawnExplosion() const {
    const sf::Vector2f spawnPos(_sprite.getGlobalBounds().left + _sprite.getGlobalBounds().width / 2, _sprite.getGlobalBounds().top - (3.f * (float)TILE_SIZE / 2.f));
    const auto& explosion = std::shared_ptr<Explosion>(new Explosion(spawnPos, Item::ITEMS[_itemId]->getDamage(), optimizedExplosions, optimizedExplosions));
    explosion->setWorld(getWorld());
    explosion->loadSprite(getWorld()->getSpriteSheet());
    getWorld()->addEntity(explosion);
}

void Projectile::draw(sf::RenderTexture& surface) {
    if (_data.isAnimated) {
        std::shared_ptr<const Item> item = Item::ITEMS[_itemId];
        sf::IntRect projRect = item->getTextureRect();

        int yOffset = ((((_animationTime) >> _data.animationSpeed) & (_data.animationFrames - 1))) * 16;
        if (_data.animationSpeed == 0) {
            srand(currentTimeMillis());
            yOffset = randomInt(0, _data.animationFrames - 1);
        }
        _sprite.setTextureRect(sf::IntRect(projRect.left, projRect.top + yOffset, projRect.width, projRect.height));

        _animationTime++;
    }


    surface.draw(_sprite);
}

void Projectile::setSplitInto(const std::string projectileDataIdentifier, const bool splitOnHit, const bool splitOnDecay, const unsigned int projectileCount) {
    _splitProjectileDataIdentifer = projectileDataIdentifier;
    _splitOnHit = splitOnHit;
    _splitOnDecay = splitOnDecay;
    _splitProjectileCount = projectileCount;
}

void Projectile::split() const {
    if (_splitProjectileDataIdentifer == "") {
        MessageManager::displayMessage("split() was called on a projectile with an undefined split projectile identifier", 5, WARN);
        return;
    }

    const float angleIncrement = 360.f / _splitProjectileCount;
    for (int i = 0; i < _splitProjectileCount; i++) {
        const auto& proj = ProjectilePoolManager::addProjectile(
            getPosition(), _parent, degToRads(angleIncrement * i), ProjectileDataManager::getData(_splitProjectileDataIdentifer).baseVelocity,
            ProjectileDataManager::getData(_splitProjectileDataIdentifer), onlyDamagePlayer, _damageBoost, false, passThroughCount, _explosionBehavior
        );

        proj->setCrit(_criticalHit);
        proj->bounceOffViewport = bounceOffViewport;
    }
}

void Projectile::setCrit(const bool crit) {
    _criticalHit = crit;
}

void Projectile::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    std::shared_ptr<const Item> item = Item::ITEMS[_itemId];
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(item->getTextureRect());
    _sprite.setOrigin(0, item->getTextureRect().height / 2);
    if (_data.rotateSprite) _sprite.setRotation(_directionAngle * (180.f / PI));
    else _sprite.setRotation(0);

    _hitBox.left = _sprite.getGlobalBounds().left + _hitBoxXOffset;
    _hitBox.top = _sprite.getGlobalBounds().top + _hitBoxYOffset;
}

std::string Projectile::getSaveData() const {
    if (!onlyDamagePlayer) {
        return _parent->getUID() + ":" + std::to_string(_directionAngle) + ":" + std::to_string(_velocity)
            + ":" + std::to_string(_data.itemId)
            + ":" + std::to_string(_data.baseVelocity)
            + ":" + std::to_string(_data.hitBox.left) + "," + std::to_string(_data.hitBox.left) + "," + std::to_string(_data.hitBox.width) + "," + std::to_string(_data.hitBox.height)
            + ":" + (_data.rotateSprite ? "1" : "0")
            + ":" + (_data.onlyHitEnemies ? "1" : "0")
            + ":" + std::to_string(_data.lifeTime)
            + ":" + (_data.isAnimated ? "1" : "0")
            + ":" + std::to_string(_data.animationFrames)
            + ":" + std::to_string(_data.animationSpeed)
            + ":" + (onlyDamagePlayer ? "1" : "0");
    } else return "NOSAVE";
}

void Projectile::reset(sf::Vector2f pos, Entity* parent, float directionAngle, float velocity, const ProjectileData data,
    bool onlyHitPlayer, int damageBoost, bool addParentVelocity, int passThroughCount, EXPLOSION_BEHAVIOR explosionBehavior) {
    _originalPos = pos;
    _parent = parent;
    _directionAngle = directionAngle;
    _velocity = velocity;
    _damageBoost = damageBoost;
    _itemId = data.itemId;
    this->onlyDamagePlayer = onlyHitPlayer;
    _data = data;
    if (explosionBehavior == EXPLOSION_BEHAVIOR::DEFER_TO_DATA) _explosionBehavior = data.explosionBehavior;
    else _explosionBehavior = explosionBehavior;

    _pos = pos;
    sf::Vector2f shooterVelocity(parent->getVelocity().x, parent->getVelocity().y);

    _velocityComponents.x = _velocity * std::cos(directionAngle) + (addParentVelocity ? shooterVelocity.x : 0);
    _velocityComponents.y = _velocity * std::sin(directionAngle) + (addParentVelocity ? shooterVelocity.y : 0);

    _lifeTime = data.lifeTime;
    _spawnTime = currentTimeMillis();

    loadSprite(getWorld()->getSpriteSheet());
    _sprite.setPosition(_pos);
    //setMaxHitPoints(1000000);
    //heal(getMaxHitPoints());

    _hitBoxXOffset = _data.hitBox.left;
    _hitBoxYOffset = _data.hitBox.top;
    _hitBox.width = _data.hitBox.width;
    _hitBox.height = _data.hitBox.height; 
    _hitBox.left = _sprite.getGlobalBounds().left + _hitBoxXOffset;
    _hitBox.top = _sprite.getGlobalBounds().top + _hitBoxYOffset;

    this->passThroughCount = passThroughCount;
    _animationTime = 0;
    _entitiesPassedThrough = 0;
    _hitEntities.clear();
    _currentTime = 0;

    _isActive = true;

    _splitProjectileDataIdentifer = "";
    _splitOnHit = false;
    _splitOnDecay = false;
    _splitProjectileCount = 0;

    optimizedExplosions = false;

    _criticalHit = false;

    bounceOffViewport = false;
    _baseSpeed = 0;
}
