#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "GameController.h"
#include "Chunk.h"

constexpr int PLAYER_WIDTH = 16;
constexpr int PLAYER_HEIGHT = 32;

constexpr bool GODMODE = false;
constexpr float BASE_PLAYER_SPEED = GODMODE ? 8 : 1;
constexpr bool NO_MOVEMENT_RESTRICIONS = GODMODE;

constexpr long double PI = 3.14159265358979L;

class Player : public Entity, public GameControllerListener {
public:
    Player(sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused);

    virtual void update();

    virtual void draw(sf::RenderTexture& surface);

    virtual void move(float xa, float ya);

    sf::Vector2f getPosition() const;

    bool isDodging() const;

    void knockBack(float amt, MOVING_DIRECTION dir);
    virtual void damage(int damage);

    virtual void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    bool freeMove = false;

    void mouseButtonReleased(const int mx, const int my, const int button);
    void keyReleased(sf::Keyboard::Key& key);

    void controllerButtonReleased(CONTROLLER_BUTTON button);
    void controllerButtonPressed(CONTROLLER_BUTTON button);

    friend class Game;
    friend class RemotePlayer;

private:
    bool& _gamePaused;

    sf::Sprite _wavesSprite;

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

    float _multiplayerAimAngle;

    // this might be the single worst thing i've ever done
    bool _isActuallyMoving = false;
    bool _isHoldingWeapon = false;

    virtual TERRAIN_TYPE getCurrentTerrain();

    float _sprintMultiplier = 2;

    MOVING_DIRECTION _facingDir = (MOVING_DIRECTION)_movingDir;

    float _dodgeMultiplier(int dodgeTimer) const {
        return (0.075f * (float)(std::pow((float)dodgeTimer, 2) - 4) + (float)2);
    }

    bool _isDodging = false;
    int _dodgeTimer = 0;
    int _maxDodgeTime = 10;
    float _dodgeSpeedMultiplier = 1.f;
    bool _dodgeKeyReleased = true;

    void fireWeapon();
    void reloadWeapon();

    void meleeAttack(sf::FloatRect meleeHitBox, sf::Vector2f currentMousePos);
    sf::Vector2f _lastMousePos;
    unsigned int _meleeAttackDelayCounter = 0;

    sf::RenderWindow* _window;
};

#endif