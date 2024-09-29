#include "Boss.h"
#include "../../core/Util.h"
#include "../World.h"

Boss::Boss(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight, std::vector<BossState> bossStates) :
Entity(saveId, pos, baseSpeed, spriteWidth, spriteHeight, false), _numBossStates(bossStates.size()), _currentState(bossStates.at(0)) {
    _isMob = true;
    _isEnemy = true;
    _isBoss = true;

    _bossStates = bossStates;

    _lastStateChangeTime = currentTimeMillis();
}

void Boss::update() {
    subUpdate();

    runCurrentState();
    if (currentTimeMillis() - _lastStateChangeTime >= _currentStateLength) changeState();
}

void Boss::deactivateBossMode() {
    _spawnedWithEnemies = true;
    _isBoss = false;
    _isMiniboss = true;
}

void Boss::changeState() {
    const BossState previousState = _currentState;

    std::vector<BossState> availableStates;
    for (int i = 0; i < _numBossStates; i++) {
        int stateIndex = i;
        if (stateIndex != _currentState.stateId) availableStates.push_back(_bossStates.at(stateIndex));
    }
    _currentState = availableStates.at(randomInt(0, (int)availableStates.size() - 1));

    _currentStateLength = (long long)randomInt(_currentState.minLength, _currentState.maxLength);
    _lastStateChangeTime = currentTimeMillis();

    onStateChange(previousState, _currentState);
}

void Boss::damage(int damage) {
    _hitPoints -= damage;
    if (_hitPoints <= 0) {
        _isActive = false;
        for (int i = 0; i < getInventory().getCurrentSize(); i++) {
            if (getInventory().getItemIdAt(i) == Item::PENNY.getId() && !_spawnedWithEnemies) {
                MessageManager::displayMessage("You defeated " + getDisplayName() + "\n\nYou won " + std::to_string(getInventory().getItemAmountAt(i)) + " pennies!\n\nCongrats", 10);
                getWorld()->getPlayer()->getInventory().addItem(Item::PENNY.getId(), getInventory().getItemAmountAt(i));
            } else {
                getInventory().dropItem(getInventory().getItemIdAt(i), getInventory().getItemAmountAt(i));
            }
        }

        if (!_spawnedWithEnemies) getWorld()->bossDefeated();
    }
}

std::string Boss::getSaveData() const {
    return _spawnedWithEnemies ? "1" : "0";
}