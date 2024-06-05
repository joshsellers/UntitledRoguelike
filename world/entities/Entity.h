#ifndef _ENTITY_H
#define _ENTITY_H

#include <SFML/Graphics.hpp>
#include "../../inventory/Inventory.h"
#include <boost/random/mersenne_twister.hpp>
#include "EntityTypeId.h"

constexpr int SPRITE_SHEET_SHIFT = 4;
constexpr int TILE_SIZE = 16;

constexpr int DEFAULT_DORMANCY_TIMEOUT = 60 * 30;
constexpr int DEFAULT_MAX_TIME_OUT_OF_CHUNK = 60 * 15;

enum MOVING_DIRECTION {
    UP = 2,
    DOWN = 0,
    LEFT = 1,
    RIGHT = 3
};

class Entity {
public:
    Entity(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, const bool isProp);

    virtual void update() = 0;

    virtual void draw(sf::RenderTexture& surface) = 0;

    virtual void move(float xa, float ya);
    bool isMoving() const;
    MOVING_DIRECTION getMovingDir() const;

    bool isSwimming() const;

    void setBaseSpeed(float speed);
    float getBaseSpeed() const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet) = 0;

    bool isMob() const;
    bool isProp() const;
    bool isEnemy() const;
    bool canPickUpItems() const;
    
    bool usesDormancyRules() const;
    void shouldUseDormancyRules(bool usesDormancyRules);
    void setDormancyTimeout(int dormancyTimeout);
    void setMaxTimeOutOfChunk(int maxTimeOutOfChunk);
    
    bool isInitiallyDocile() const;
    bool isHostile() const;

    sf::Sprite getSprite() const;

    bool displayBottom() const;

    sf::Vector2i getSpriteSize() const;

    Inventory& getInventory();

    void setWorld(World* world);
    World* getWorld() const;

    bool isActive() const;
    void deactivate();
    void activate();

    bool isDormant() const;
    void setDormant(bool dormant);
    int getDormancyTimeout() const;
    int getDormancyTime() const;
    void resetDormancyTimer();
    void incrementDormancyTimer();

    int getMaxTimeOutOfChunk() const;
    int getTimeOutOfChunk() const;
    void resetOutOfChunkTimer();
    void incrementOutOfChunkTimer();

    bool isDamageable() const;
    
    virtual void knockBack(float amt, MOVING_DIRECTION dir);

    void setMaxHitPoints(int maxHitPoints);
    int getMaxHitPoints() const;
    int& getMaxHitPointsRef();
    virtual void takeDamage(int damage);
    void heal(int hitPoints);
    int getHitPoints() const;
    int& getHitPointsRef();

    virtual int getStamina() const;
    virtual int getMaxStamina() const;
    virtual void setMaxStamina(int amount);
    virtual void restoreStamina(int amount);
    virtual void increaseStaminaRefreshRate(int amount);

    float getDamageMultiplier() const;
    void increaseDamageMultiplier(float amount);

    sf::FloatRect getHitBox() const;

    bool hasColliders() const;
    std::vector<sf::FloatRect> getColliders() const;

    sf::Vector2f getCalculatedBarrelPos() const;
    sf::Vector2f getTargetPos() const;

    bool compare(Entity* entity) const;

    const std::string& getEntityType() const;

    unsigned int getMagazineAmmoType() const;
    int& getMagazineSize();
    int& getMagazineContents();
    void decrementMagazine();
    void emptyMagazine();
    bool isReloading() const;

    ENTITY_SAVE_ID getSaveId() const;
    virtual std::string getSaveData() const;

    std::string getUID() const;
    void setUID(std::string uuid);

    virtual void lowContextSubclassFunction(std::string args);

    friend class SaveManager;

protected:
    std::string _entityType = "";

    const int _spriteWidth, _spriteHeight;

    bool _displayBottom = false;

    sf::Sprite _sprite;

    float _baseSpeed = 0;

    sf::Vector2f _pos;
    sf::Vector2f _velocity;
    int _numSteps = 0;
    int _animSpeed = 3;

    bool _isMoving = false;
    unsigned int _movingDir = DOWN;

    bool _isSwimming = false;
    
    void hoardMove(float xa, float ya, bool sameTypeOnly = false, float minDist = 32.f, float visionRange = 100.f);
    void wander(sf::Vector2f feetPos, boost::random::mt19937& generator, int movementChance = 100);
    sf::Vector2f _wanderTargetPos;

    bool _isMob = false;
    const bool _isProp = false;
    bool _isEnemy = false;
    bool _canPickUpItems = false;
    bool _usesDormancyRules = false;

    // this is specifically for things like cactoids that aren't immediatly aggressive
    bool _isInitiallyDocile = false;
    bool _isHostile = false;

    void fireTargetedProjectile(sf::Vector2f targetPos, const ProjectileData projData, std::string soundName = "NONE", bool onlyDamagePlayer = false);

    Inventory _inventory = Inventory(this);

    World* _world = nullptr;

    bool _isActive = true;
    bool _isDormant = false;

    int _dormancyTimer = 0;
    int _dormancyTimeout = DEFAULT_DORMANCY_TIMEOUT;

    int _outOfChunkTimer = 0;
    int _maxTimeOutOfChunk = DEFAULT_MAX_TIME_OUT_OF_CHUNK;

    sf::Vector2f _calculatedBarrelPos;
    sf::Vector2f _targetPos;

    sf::FloatRect _hitBox;
    float _hitBoxXOffset = 0;
    float _hitBoxYOffset = 0;

    int _hitPoints = 0;

    float _damageMultiplier = 1.f;

    bool _hasColliders = false;
    std::vector<sf::FloatRect> _colliders;

    unsigned int _magazineAmmoType;
    int _magazineSize = 0;
    int _magazineContents = 0;

    bool _isReloading = false;

    virtual void damage(int damage);

private:
    const ENTITY_SAVE_ID _saveId;
    std::string _uid;

    int _maxHitPoints = 0;

    const sf::Vector2f separate(sf::Vector2f acceleration, bool sameTypeOnly, float minDist);
    const sf::Vector2f align(bool sameTypeOnly, float visionRange);
    const sf::Vector2f cohesion(sf::Vector2f acceleration, bool sameTypeOnly, float visionRange);
};
#endif // !_ENTITY_H
