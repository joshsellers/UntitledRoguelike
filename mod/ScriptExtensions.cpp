#include "ScriptExtensions.h"
#include "../core/MessageManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"
#include "../world/World.h"
#include "../world/entities/orbiters/Orbiter.h"
#include "../world/entities/Scythe.h"
#include "../core/Tutorial.h"
#include "../world/entities/LaserBeam.h"
#include "../inventory/effect/PlayerVisualEffectManager.h"

bool ScriptExtensions::execute(const std::string functionName, Entity* entity, Interpreter* interpreter) {
    if (_functions.find(functionName) != _functions.end()) {
        return _functions.at(functionName)(entity, interpreter);
    }

    MessageManager::displayMessage("No built-in function named \"" + functionName + "\"", 5, WARN);
    return false;
}

std::map<const std::string, const std::function<bool(Entity*, Interpreter*)>> ScriptExtensions::_functions = {
    {
        "increaseCritChance",
        [](Entity* parent, Interpreter* interpreter) {
            const float increaseAmount = interpreter->pop();
            const float currentChance = AbilityManager::getParameter(Ability::CRIT_CHANCE.getId(), "chance");
            AbilityManager::setParameter(Ability::CRIT_CHANCE.getId(), "chance", currentChance + increaseAmount);
            return false;
        }
    },

    {
        "increaseOrbiterDist",
        [](Entity* parent, Interpreter* interpreter) {
            constexpr float increaseAmount = 8.f;
            for (const auto& entity : parent->getWorld()->getEntities()) {
                if (entity->isOrbiter()) {
                    Orbiter* orbiter = dynamic_cast<Orbiter*>(entity.get());
                    if (orbiter->getParent()->getSaveId() == PLAYER) {
                        orbiter->setDistance(orbiter->getDistance() + increaseAmount);
                    }
                }
            }

            return true;
        }
    },

    {
        "addOrbiter",
        [](Entity* parent, Interpreter* interpreter) {
            const std::string orbiterName = interpreter->strPop();
            int orbiterId = -1;
            for (int i = 0; i < OrbiterType::ORBITER_TYPES.size(); i++) {
                if (OrbiterType::ORBITER_TYPES.at(i)->getName() == orbiterName) {
                    orbiterId = i;
                    break;
                }
            }

            if (orbiterId == -1) {
                MessageManager::displayMessage("No orbiter named \"" + orbiterName + "\"", 5, ERR);
                return false;
            }

            const auto& orbiter = std::shared_ptr<Orbiter>(new Orbiter(90.f, orbiterId, parent));
            orbiter->setWorld(parent->getWorld());
            orbiter->loadSprite(parent->getWorld()->getSpriteSheet());
            parent->getWorld()->addEntity(orbiter);

            return true;
        }
    },

    {
        "spawnScythe",
        [](Entity* parent, Interpreter* interpreter) {
            const auto& scythe = std::shared_ptr<Scythe>(new Scythe({
                parent->getWorld()->getPlayer()->getPosition().x + (float)TILE_SIZE / 2.f, parent->getWorld()->getPlayer()->getPosition().y + TILE_SIZE
            }));
            scythe->setWorld(parent->getWorld());
            scythe->loadSprite(parent->getWorld()->getSpriteSheet());
            parent->getWorld()->addEntity(scythe);
            return true;
        }
    },

    {
        "completeTutorialStep",
        [](Entity* parent, Interpreter* interpreter) {
            const int step = interpreter->pop();
            if (!Tutorial::isCompleted()) {
                Tutorial::completeStep((TUTORIAL_STEP)step);
            }
            return true;
        }
    },

    {
        "dicepowder",
        [](Entity* parent, Interpreter* interpreter) {
            parent->setMaxHitPoints(randomInt(5, 500));
            parent->heal(parent->getMaxHitPoints());
            parent->heal(-parent->getMaxHitPoints());
            parent->heal(randomInt(5, parent->getMaxHitPoints()));

            const auto& player = parent->getWorld()->getPlayer();
            const float speedMultiplierAbs = (float)randomInt(5, 200) / 100.f;
            player->setSpeedMultiplier(randomChance(0.5f) ? speedMultiplierAbs : -speedMultiplierAbs);
            player->setDamageMultiplier((float)randomInt(5, 500) / 100.f);
            player->setStaminaRefreshRate(randomInt(1, 100));
            player->setMaxStamina(randomInt(10, 3000));
            
            std::vector<unsigned int> possibleItems;
            for (int i = 0; i < Item::ITEMS.size(); i++) {
                const auto& item = Item::ITEMS.at(i);
                if (item->isBuyable() && item->isUnlocked(player->getWorld()->getCurrentWaveNumber()) && !stringStartsWith(item->getName(), "_") 
                    && item->getEquipmentType() != EQUIPMENT_TYPE::AMMO) {
                    possibleItems.push_back(item->getId());
                }
            }

            const int numItems = player->getInventory().getCurrentSize() - 1;
            while (player->getInventory().getCurrentSize() != 0) {
                player->getInventory().removeItemAt(0, player->getInventory().getItemAmountAt(0));
            }

            for (int i = 0; i < numItems; i++) {
                const auto& item = Item::ITEMS.at(possibleItems.at(randomInt(0, possibleItems.size() - 1)));
                player->getInventory().addItem(item->getId(), item->isStackable() ? randomInt(1, item->getStackLimit()) : 1);
            }

            AbilityManager::resetAbilities();
            const int abilityCount = randomInt(1, Ability::ABILITIES.size() - 1);
            for (int i = 0; i < abilityCount; i++) {
                const unsigned int abilityId = randomInt(0, Ability::ABILITIES.size() - 1);
                if (!AbilityManager::playerHasAbility(abilityId)) {
                    AbilityManager::givePlayerAbility(abilityId);
                    const auto& params = Ability::ABILITIES.at(abilityId)->getParameters();
                    for (const auto& parameter : params) {
                        AbilityManager::setParameter(abilityId, parameter.first, randomInt(0, 50000) / 100.f);
                    }
                }
            }

            PlayerVisualEffectManager::clearPlayerEffects();
            const int effectCount = randomInt(1, PlayerVisualEffectManager::getEffectCount() - 1);
            for (int i = 0; i < effectCount; i++) {
                PlayerVisualEffectManager::addEffectToPlayer(PlayerVisualEffectManager::getEffectTypes().at(randomInt(0, PlayerVisualEffectManager::getEffectCount() - 1)).name);
            }

            return true;
        }
    }
};