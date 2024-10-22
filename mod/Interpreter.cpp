#include "Interpreter.h"
#include "Instructions.h"
#include <iostream>
#include "ModManager.h"
#include "../core/Util.h"
#include "../core/MessageManager.h"
#include "../core/SoundManager.h"
#include "../inventory/effect/PlayerVisualEffectManager.h"
#include "../inventory/abilities/AbilityManager.h"
#include "../inventory/abilities/Ability.h"

int Interpreter::interpret(std::vector<int> bytecode, Entity* entity) {
    int i = 0;
    while (i < bytecode.size()) {
        INSTRUCTION inst = (INSTRUCTION)bytecode.at(i);

        if (inst == INSTRUCTION::LIT) {
            int value = 0;
            for (int j = 3; j >= 0; j--) {
                int byte = bytecode.at((i + 1) + j);
                value += byte << (j * 8);
            }
            push((float)value);
            i += 5;
        } else if (inst == INSTRUCTION::STR) {
            std::string str = "";
            int strSize = bytecode.at(i + 1);
            for (int j = 0; j < strSize; j++) {
                str += (char)bytecode.at(i + 2 + j);
            }
            strPush(str);
            i += strSize + 2;
        } else if (inst == INSTRUCTION::IF) {
            if ((int)pop()) {
                callStackPush(-1);
                i++;
            } else {
                int ifWhileCount = 0;
                int endIfCount = 0;
                for (int j = i + 1; j < bytecode.size(); j++) {
                    INSTRUCTION subInst = (INSTRUCTION)bytecode.at(j);
                    if (subInst == INSTRUCTION::LIT) j += 4;
                    else if (subInst == INSTRUCTION::STR) skipStringLit(j, bytecode);
                    else if (subInst == INSTRUCTION::WHILE || subInst == INSTRUCTION::IF) ifWhileCount++;
                    else if (subInst == INSTRUCTION::ENDIF) endIfCount++;
                    if (endIfCount > ifWhileCount) {
                        i = j + 1;
                        break;
                    }
                }
            }
        } else if (inst == INSTRUCTION::ENDIF) {
            int addr = callStackPop();
            if (addr == -1) i++;
            else {
                i = addr;
            }
        } else if (inst == INSTRUCTION::WHILE) {
            int test = (int)pop();
            if (test) {
                i++;
            } else {
                int ifWhileCount = 0;
                int endIfCount = 0;
                for (int j = i + 1; j < bytecode.size(); j++) {
                    INSTRUCTION subInst = (INSTRUCTION)bytecode.at(j);
                    if (subInst == INSTRUCTION::LIT) j += 4;
                    else if (subInst == INSTRUCTION::STR) skipStringLit(j, bytecode);
                    else if (subInst == INSTRUCTION::WHILE || subInst == INSTRUCTION::IF) ifWhileCount++;
                    else if (subInst == INSTRUCTION::ENDIF) endIfCount++;
                    if (endIfCount > ifWhileCount) {
                        callStackPop();
                        i = j + 1;
                        break;
                    }
                }
            }
        } else if (inst == INSTRUCTION::STWH) {
            callStackPush(i);
            i++;
        } else if (inst == INSTRUCTION::BRK) {
            for (int j = i + 1; j < bytecode.size(); j++) {
                INSTRUCTION subInst = (INSTRUCTION)bytecode.at(j);

                int currentAddr = -1;
                if (subInst == INSTRUCTION::LIT) j += 4;
                else if (subInst == INSTRUCTION::STR) skipStringLit(j, bytecode);
                else if (subInst == INSTRUCTION::WHILE || subInst == INSTRUCTION::IF) callStackPush(-1);
                else if (subInst == INSTRUCTION::ENDIF) {
                    currentAddr = callStackPop();
                }

                if (currentAddr != -1) {
                    i = j + 1;
                    break;
                }
            }
        } else if (inst == INSTRUCTION::CONT) {
            i = callStackPop();
            while (i == -1) {
                i = callStackPop();
            }
        } else if (inst == INSTRUCTION::RET) {
            for (int j = i + 1; j < bytecode.size(); j++) {
                INSTRUCTION subInst = (INSTRUCTION)bytecode.at(j);

                if (subInst == INSTRUCTION::LIT) j += 4;
                else if (subInst == INSTRUCTION::STR) skipStringLit(j, bytecode);
                else if (subInst == INSTRUCTION::WHILE || subInst == INSTRUCTION::IF) callStackPush(-1);
                else if (subInst == INSTRUCTION::ENDIF) {
                    callStackPop();
                }
            }
            return pop();
        } else if (inst == INSTRUCTION::ASSIGN) {
            int regAddr = (int)pop();
            float value = pop();
            _register[regAddr] = value;
            i++;
        } else if (inst == INSTRUCTION::ASSIGNSTR) {
            int regAddr = (int)pop();
            std::string str = strPop();
            _strRegister[regAddr] = str;
            i++;
        } else if (inst == INSTRUCTION::READ) {
            int regAddr = (int)pop();
            push(_register[regAddr]);
            i++;
        } else if (inst == INSTRUCTION::READSTR) {
            int regAddr = (int)pop();
            strPush(_strRegister[regAddr]);
            i++;
        } else if (inst == INSTRUCTION::ADD) {
            float right = pop();
            float left = pop();
            push(left + right);
            i++;
        } else if (inst == INSTRUCTION::SUB) {
            float right = pop();
            float left = pop();
            push(left - right);
            i++;
        } else if (inst == INSTRUCTION::MUL) {
            float right = pop();
            float left = pop();
            push(left * right);
            i++;
        } else if (inst == INSTRUCTION::DIV) {
            float right = pop();
            float left = pop();
            push(left / right);
            i++;
        } else if (inst == INSTRUCTION::MOD) {
            int right = pop();
            int left = pop();
            push(left % right);
            i++;
        } else if (inst == INSTRUCTION::GRT) {
            float right = pop();
            float left = pop();
            push((int)(left > right));
            i++;
        } else if (inst == INSTRUCTION::LSS) {
            float right = pop();
            float left = pop();
            push((int)(left < right));
            i++;
        } else if (inst == INSTRUCTION::GRE) {
            float right = pop();
            float left = pop();
            push((int)(left >= right));
            i++;
        } else if (inst == INSTRUCTION::LSE) {
            float right = pop();
            float left = pop();
            push((int)(left <= right));
            i++;
        } else if (inst == INSTRUCTION::EQL) {
            float right = pop();
            float left = pop();
            push((int)(left == right));
            i++;
        } else if (inst == INSTRUCTION::NEQ) {
            float right = pop();
            float left = pop();
            push((int)(left != right));
            i++;
        } else if (inst == INSTRUCTION::AND) {
            int right = (int)pop();
            int left = (int)pop();
            push((int)(left && right));
            i++;
        } else if (inst == INSTRUCTION::OR) {
            int right = (int)pop();
            int left = (int)pop();
            push((int)(left || right));
            i++;
        } else if (inst == INSTRUCTION::SGN) {
            float val = pop();
            push(-val);
            i++;
        } else if (inst == INSTRUCTION::PRNT) {
            std::string val = strPop();
            std::cout << val;
            i++;
        } else if (inst == INSTRUCTION::PRNTLN) {
            std::string val = strPop();
            std::cout << val << std::endl;
            i++;
        } else if (inst == INSTRUCTION::NUMPRNT) {
            float val = pop();
            std::cout << val;
            i++;
        } else if (inst == INSTRUCTION::NUMPRNTLN) {
            float val = pop();
            std::cout << val << std::endl;
            i++;
        } else if (inst == INSTRUCTION::CALL) {
            std::string funcName = strPop();
            push(interpret(ModManager::getFunction(funcName), entity));
            i++;
        } else if (inst == INSTRUCTION::POP) {
            i++;
        } else if (inst == INSTRUCTION::RAND) {
            float max = pop();
            float min = pop();
            push(randomInt(min, max));
            i++;
        } else if (inst == INSTRUCTION::DMSG) {
            std::string msg = strPop();
            replaceAll(msg, "\\n", "\n");
            int dispTime = (int)pop();
            MessageManager::displayMessage(msg, dispTime);
            i++;
        } else if (inst == INSTRUCTION::NUMDMSG) {
            int dispTime = (int)pop();
            float msg = pop();
            MessageManager::displayMessage(msg, dispTime, DEBUG);
            i++;
        } else if (inst == INSTRUCTION::DMSGDEBUG) {
            std::string msg = strPop();
            replaceAll(msg, "\\n", "\n");
            int dispTime = (int)pop();
            MessageManager::displayMessage(msg, dispTime, DEBUG);
            i++;
        } else if (inst == INSTRUCTION::DMSGWARN) {
            std::string msg = strPop();
            replaceAll(msg, "\\n", "\n");
            int dispTime = (int)pop();
            MessageManager::displayMessage(msg, dispTime, WARN);
            i++;
        } else if (inst == INSTRUCTION::DMSGERR) {
            std::string msg = strPop();
            replaceAll(msg, "\\n", "\n");
            int dispTime = (int)pop();
            MessageManager::displayMessage(msg, dispTime, ERR);
            i++;
        } else if (inst == INSTRUCTION::SPLHP) {
            if (entity != nullptr) {
                int hp = pop();
                int currentHp = entity->getHitPoints();
                entity->heal(-(currentHp - hp));
            }
            i++;
        } else if (inst == INSTRUCTION::GPLHP) {
            if (entity != nullptr) {
                push(entity->getHitPoints());
            }
            i++;
        } else if (inst == INSTRUCTION::SPLMAXHP) {
            if (entity != nullptr) {
                int maxHp = pop();
                entity->setMaxHitPoints(maxHp);
            }
            i++;
        } else if (inst == INSTRUCTION::GPLMAXHP) {
            if (entity != nullptr) {
                push(entity->getMaxHitPoints());
            }
            i++;
        } else if (inst == INSTRUCTION::PLAYSOUND) {
            if (_strStackSize > 0) {
                std::string soundName = strPop();
                SoundManager::playSound(soundName);
            } else {
                MessageManager::displayMessage("playSound() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLSHOOT) {
            if (_strStackSize > 0) {
                std::string projItemName = strPop();
                int passThroughCount = pop();
                if (entity != nullptr) {
                    Item::fireTargetedProjectile(entity, ProjectileDataManager::getData(projItemName), "NONE", passThroughCount);
                }
            } else {
                MessageManager::displayMessage("player.fireProjectile() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGIVEITEM) {
            if (_strStackSize > 0) {
                if (entity != nullptr) {
                    const std::string itemName = strPop();
                    const int amount = pop();

                    bool foundItem = false;
                    for (const auto& item : Item::ITEMS) {
                        if (item->getName() == itemName) {
                            entity->getInventory().addItem(item->getId(), amount);
                            foundItem = true;
                            break;
                        }
                    }

                    if (!foundItem) MessageManager::displayMessage("No item named \"" + itemName + "\"", 5, WARN);
                }
            } else {
                MessageManager::displayMessage("player.giveItem() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLHASITEM) {
            if (_strStackSize > 0) {
                if (entity != nullptr) {
                    const std::string itemName = strPop();

                    int itemId = 0;
                    bool foundItem = false;
                    for (const auto& item : Item::ITEMS) {
                        if (item->getName() == itemName) {
                            itemId = item->getId();
                            foundItem = true;
                            break;
                        }
                    }

                    if (!foundItem) MessageManager::displayMessage("No item named \"" + itemName + "\"", 5, WARN);

                    if (entity->getInventory().hasItem(itemId)) {
                        push(1);
                    } else {
                        push(0);
                    }
                }
            } else {
                MessageManager::displayMessage("player.hasItem() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLWEAPONLOADED) {
            if (entity != nullptr) {
                if (entity->getMagazineContents() > 0) {
                    push(1);
                } else push(0);
            }
            i++;
        } else if (inst == INSTRUCTION::PLADDEFFECT) {
            if (_strStackSize > 0) {
                const std::string effectName = strPop();

                PlayerVisualEffectManager::addEffectToPlayer(effectName);
            } else {
                MessageManager::displayMessage("player.addEffect() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLADDABILITY) {
            if (_strStackSize > 0) {
                const std::string abilityName = strPop();

                bool foundAbility = false;
                for (const auto& ability : Ability::ABILITIES) {
                    if (ability->getName() == abilityName) {
                        const unsigned int abilityId = ability->getId();
                        AbilityManager::givePlayerAbility(abilityId);
                        foundAbility = true;
                        break;
                    }
                }

                if (!foundAbility) MessageManager::displayMessage("No ability named \"" + abilityName + "\"", 5, WARN);
            } else {
                MessageManager::displayMessage("player.addAbility() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLSETABILITYPARAM) {
            if (_strStackSize > 1) {
                const float paramValue = pop();
                const std::string paramName = strPop();
                const std::string abilityName = strPop();

                bool foundAbility = false;
                for (const auto& ability : Ability::ABILITIES) {
                    if (ability->getName() == abilityName) {
                        const unsigned int abilityId = ability->getId();
                        AbilityManager::setParameter(abilityId, paramName, paramValue);
                        foundAbility = true;
                        break;
                    }
                }

                if (!foundAbility) MessageManager::displayMessage("No ability named \"" + abilityName + "\"", 5, WARN);
            } else {
                MessageManager::displayMessage("player.setAbilityParameter() requires two string parameters", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGETABILITYPARAM) {
            if (_strStackSize > 1) {
                const std::string paramName = strPop();
                const std::string abilityName = strPop();

                bool foundAbility = false;
                for (const auto& ability : Ability::ABILITIES) {
                    if (ability->getName() == abilityName) {
                        const unsigned int abilityId = ability->getId();
                        push(AbilityManager::getParameter(abilityId, paramName));
                        foundAbility = true;
                        break;
                    }
                }

                if (!foundAbility) MessageManager::displayMessage("No ability named \"" + abilityName + "\"", 5, WARN);
            } else {
                MessageManager::displayMessage("player.getAbilityParameter() requires two string parameters", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLHASABILITY) {
            if (_strStackSize > 0) {
                const std::string abilityName = strPop();

                bool foundAbility = false;
                for (const auto& ability : Ability::ABILITIES) {
                    if (ability->getName() == abilityName) {
                        const unsigned int abilityId = ability->getId();
                        push(AbilityManager::playerHasAbility(abilityId));
                        foundAbility = true;
                        break;
                    }
                }

                if (!foundAbility) MessageManager::displayMessage("No ability named \"" + abilityName + "\"", 5, WARN);
            } else {
                MessageManager::displayMessage("player.hasAbility() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLHASEFFECT) {
            if (_strStackSize > 0) {
                const std::string effectName = strPop();

                push(PlayerVisualEffectManager::playerHasEffect(effectName));
            } else {
                MessageManager::displayMessage("player.hasEffect() requires a string parameter", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLSETDAMAGE) {
            if (entity != nullptr) {
                float newDamage = pop();
                entity->setDamageMultiplier(newDamage);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGETDAMAGE) {
            if (entity != nullptr) {
                push(entity->getDamageMultiplier());
            }
            i++;
        } else if (inst == INSTRUCTION::PLSETMAXSTAMINA) {
            if (entity != nullptr) {
                int newMaxStamina = pop();
                entity->setMaxStamina(newMaxStamina);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGETMAXSTAMINA) {
            if (entity != nullptr) {
                push(entity->getMaxStamina());
            }
            i++;
        } else if (inst == INSTRUCTION::PLSETSTAMINARESTORE) {
            if (entity != nullptr) {
                int newStaminaRestore = pop();
                entity->setStaminaRefreshRate(newStaminaRestore);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGETSTAMINARESTORE) {
            if (entity != nullptr) {
                push(entity->getStaminaRefreshRate());
            }
            i++;
        } else if (inst == INSTRUCTION::PLSETSPEEDMULT) {
            if (entity != nullptr && entity->getSaveId() == PLAYER) {
                Player* player = dynamic_cast<Player*>(entity);
                float newSpeedMult = pop();
                player->setSpeedMultiplier(newSpeedMult);
            } else if (entity != nullptr && entity->getSaveId() != PLAYER) {
                MessageManager::displayMessage("player.setSpeedMultiplier was called in a function that was not provided with a ref to the player", 5, ERR);
            }
            i++;
        } else if (inst == INSTRUCTION::PLGETSPEEDMULT) {
            if (entity != nullptr && entity->getSaveId() == PLAYER) {
                Player* player = dynamic_cast<Player*>(entity);
                push(player->getSpeedMultiplier());
            } else if (entity != nullptr && entity->getSaveId() != PLAYER) {
                MessageManager::displayMessage("player.getSpeedMultiplier was called in a function that was not provided with a ref to the player", 5, ERR);
            }
            i++;
        } else {
            MessageManager::displayMessage("Unknown instruction: " + std::to_string((int)inst), 5, ERR);
            i++;
            break;
        }
    }

    return 0;
}

void Interpreter::skipStringLit(int& index, std::vector<int> bytecode) {
    int strSize = bytecode.at(index + 1);
    index += strSize + 1;
}