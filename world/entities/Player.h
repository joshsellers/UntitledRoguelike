#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics.hpp>
#include "HairyEntity.h"
#include "../../core/gamepad/GamePad.h"
#include "../Chunk.h"

constexpr int PLAYER_WIDTH = 16;
constexpr int PLAYER_HEIGHT = 32;

constexpr bool GODMODE = false;
constexpr float BASE_PLAYER_SPEED = GODMODE ? 8 : 2;
constexpr bool NO_MOVEMENT_RESTRICIONS = GODMODE;

constexpr int INITIAL_MAX_STAMINA = 1000;
constexpr int SPRINT_STAMINA_COST = 0;
constexpr int DODGE_STAMINA_COST = 250;
constexpr int INITIAL_STAMINA_REFRESH_RATE = 5;

constexpr long double PI = 3.14159265358979L;

class Player : public HairyEntity, public GamePadListener {
public:
    Player(sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface);

    virtual void move(float xa, float ya);

    sf::Vector2f getPosition() const;

    bool isDodging() const;

    void knockBack(float amt, MOVING_DIRECTION dir);

    bool isInBoat();

    virtual int getStamina() const;
    virtual int getMaxStamina() const;
    int getStaminaRefreshRate() const;
    int& getStaminaRef();
    int& getMaxStaminaRef();
    
    virtual void setMaxStamina(int amount);
    virtual void restoreStamina(int amount);
    virtual void increaseStaminaRefreshRate(int amount);

    bool isUsingStamina();
    bool hasSufficientStamina(int cost);

    unsigned int getCoinMagnetCount() const;
    void addCoinMagnet();

    int& getMagazineContentsPercentage();
    void decrementMagazine();
    void emptyMagazine();

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    bool freeMove = false;

    void mouseButtonReleased(const int mx, const int my, const int button);
    void keyPressed(sf::Keyboard::Key& key);
    void keyReleased(sf::Keyboard::Key& key);

    void controllerButtonReleased(GAMEPAD_BUTTON button);
    void controllerButtonPressed(GAMEPAD_BUTTON button);
    void gamepadDisconnected();

    void toggleVisible();

    friend class Game;
    friend class RemotePlayer;
    friend class World;
    friend class SaveManager;
    friend class PlayerVisualEffectManager;
protected:
    virtual void damage(int damage);

private:
    bool _isVisible = true;

    bool& _gamePaused;

    sf::Sprite _wavesSprite;
    sf::Sprite _boatSprite;

    sf::Sprite _clothingHeadSprite;
    sf::Sprite _clothingBodySprite;
    sf::Sprite _clothingLegsSprite;
    sf::Sprite _clothingFeetSprite;

    sf::Sprite _armorHeadSprite;
    sf::Sprite _armorBodySprite;
    sf::Sprite _armorLegsSprite;
    sf::Sprite _armorFeetSprite;

    sf::Sprite _toolSprite;
    
    void drawEquipables(sf::RenderTexture& surface);
    void drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface);
    void drawTool(sf::RenderTexture& surface);

    // this might be the single worst thing i've ever done
    bool _isActuallyMoving = false;
    // this (and probably the above) were for multiplayer and can probably be removed
    bool _isHoldingWeapon = false;

    virtual TERRAIN_TYPE getCurrentTerrain();

    const float _slowMoveMultiplier = 0.5f;

    long long _lastTimeBoatBobbedUp = 0;

    float _dodgeMultiplier(int dodgeTimer) const {
        return (0.075f * (float)(std::pow((float)dodgeTimer, 2) - 4) + (float)2);
    }

    float getTotalArmorCoefficient();

    bool _isDodging = false;
    int _dodgeTimer = 0;
    int _maxDodgeTime = 10;
    float _dodgeSpeedMultiplier = 1.f;
    bool _dodgeKeyReleased = true;

    void fireWeapon();
    void fireAutomaticWeapon();
    bool reloadWeapon();
    void startReloadingWeapon();
    int _magContentsPercentage = 0.f;
    long long _lastAutoFireTimeMillis = 0;
    long long _reloadStartTimeMillis = 0;
    int _magContentsFilled = 0;

    int _automaticWeaponAnimationCounter = 0;
    bool _isFiringAutomaticWeapon = false;

    bool _inventoryMenuIsOpen = false;

    void meleeAttack(sf::FloatRect meleeHitBox, sf::Vector2f currentMousePos);
    sf::Vector2f _lastMousePos;
    unsigned int _meleeAttackDelayCounter = 0;

    int _stamina = 0;
    int _maxStamina = INITIAL_MAX_STAMINA;
    int _staminaRefreshRate = INITIAL_STAMINA_REFRESH_RATE;
    bool _isUsingStamina = false;

    unsigned int _coinMagnetCount = 0;

    sf::RenderWindow* _window;

    bool _verticalMovementKeyIsPressed = false;
    bool _horizontalMovementKeyIsPressed = false;
    long long _lastLeftOrRightPressTime = 0;
    long long _lastUpOrDownPressTime = 0;

    void blink();
    bool _isBlinking = false;
    long long _blinkStartTime = 0LL;
};

#endif