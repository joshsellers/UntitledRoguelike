#include "Compiler.h"
#include "../../core/Util.h"
#include <map>
#include <iostream>
#include "../../core/MessageManager.h"

std::string Compiler::compile(std::string assembly) {
    std::map<std::string, INSTRUCTION> instructions = {
        {"IF",                              INSTRUCTION::IF},
        {"WHILE",                           INSTRUCTION::WHILE},
        {"BREAK",                           INSTRUCTION::BRK},
        {"CONT",                            INSTRUCTION::CONT},
        {"ENDIF",                           INSTRUCTION::ENDIF},
        {"RETURN",                          INSTRUCTION::RET},
        {"+",                               INSTRUCTION::ADD},
        {"-",                               INSTRUCTION::SUB},
        {"*",                               INSTRUCTION::MUL},
        {"/",                               INSTRUCTION::DIV},
        {"%",                               INSTRUCTION::MOD},
        {">",                               INSTRUCTION::GRT},
        {"<",                               INSTRUCTION::LSS},
        {">=",                              INSTRUCTION::GRE},
        {"<=",                              INSTRUCTION::LSE},
        {"==",                              INSTRUCTION::EQL},
        {"and",                             INSTRUCTION::AND},
        {"or",                              INSTRUCTION::OR},
        {"STWH",                            INSTRUCTION::STWH},
        {"!=",                              INSTRUCTION::NEQ},
        {"print",                           INSTRUCTION::PRNT},
        {"println",                         INSTRUCTION::PRNTLN},
        {"numPrint",                        INSTRUCTION::NUMPRNT},
        {"numPrintln",                      INSTRUCTION::NUMPRNTLN},
        {"runFunction",                     INSTRUCTION::CALL},
        {"pop",                             INSTRUCTION::POP},
        {"random",                          INSTRUCTION::RAND},
        {"displayMessage",                  INSTRUCTION::DMSG},
        {"displayNumber",                   INSTRUCTION::NUMDMSG},
        {"displayDebugMessage",             INSTRUCTION::DMSGDEBUG},
        {"displayWarningMessage",           INSTRUCTION::DMSGWARN},
        {"displayErrorMessage",             INSTRUCTION::DMSGERR},
        {"player.setHp",                    INSTRUCTION::SPLHP},
        {"player.getHp",                    INSTRUCTION::GPLHP},
        {"player.setMaxHp",                 INSTRUCTION::SPLMAXHP},
        {"player.getMaxHp",                 INSTRUCTION::GPLMAXHP},
        {"playSound",                       INSTRUCTION::PLAYSOUND},
        {"player.fireProjectile",           INSTRUCTION::PLSHOOT},
        {"player.giveItem",                 INSTRUCTION::PLGIVEITEM},
        {"player.hasItem",                  INSTRUCTION::PLHASITEM},
        {"player.weaponIsLoaded",           INSTRUCTION::PLWEAPONLOADED},
        {"player.addEffect",                INSTRUCTION::PLADDEFFECT},
        {"player.addAbility",               INSTRUCTION::PLADDABILITY},
        {"player.setAbilityParameter",      INSTRUCTION::PLSETABILITYPARAM},
        {"player.getAbilityParameter",      INSTRUCTION::PLGETABILITYPARAM},
        {"player.hasAbility",               INSTRUCTION::PLHASABILITY},
        {"player.hasEffect",                INSTRUCTION::PLHASEFFECT},
        {"player.setDamageMultiplier",      INSTRUCTION::PLSETDAMAGE},
        {"player.getDamageMultiplier",      INSTRUCTION::PLGETDAMAGE},
        {"player.setMaxStamina",            INSTRUCTION::PLSETMAXSTAMINA},
        {"player.getMaxStamina",            INSTRUCTION::PLGETMAXSTAMINA},
        {"player.setStaminaRestoreRate",    INSTRUCTION::PLSETSTAMINARESTORE},
        {"player.getStaminaRestoreRate",    INSTRUCTION::PLGETSTAMINARESTORE},
        {"player.setSpeedMultiplier",       INSTRUCTION::PLSETSPEEDMULT},
        {"player.getSpeedMultiplier",       INSTRUCTION::PLGETSPEEDMULT},
        {"player.removeItem",               INSTRUCTION::PLRMITEM},
        {"entity.setPosition",              INSTRUCTION::ESETPOS},
        {"entity.getXPosition",             INSTRUCTION::EGETPOSX},
        {"entity.getYPosition",             INSTRUCTION::EGETPOSY},
        {"entity.rotate",                   INSTRUCTION::EROT},
        {"entity.setScale",                 INSTRUCTION::ESETSCALE},
        {"randomChance",                    INSTRUCTION::RANDPROB},
        {"pow",                             INSTRUCTION::POW},
        {"sqrt",                            INSTRUCTION::SQRT},
        {"sin",                             INSTRUCTION::SIN},
        {"cos",                             INSTRUCTION::COS},
        {"tan",                             INSTRUCTION::TAN},
        {"player.fireLaser",                INSTRUCTION::PLLASER},
        {"player.fireAnimatedLaser",        INSTRUCTION::PLLANIMLASER}
    };

    std::vector<int> bytecode;
    std::vector<std::string> parsedAssembly = splitString(assembly, ":");

    int index = 0;
    while (index < parsedAssembly.size()) {
        std::string instr = parsedAssembly.at(index);

        if (instr == "LIT") {
            std::string value = parsedAssembly.at(index + 1);
            if (stringStartsWith(value, "\"")) {
                bytecode.push_back((int)INSTRUCTION::STR);
                std::string str = value;
                replaceAll(str, "\"", "");

                if (stringContains(str, "{") && stringContains(str, "}")) {
                    const std::string varName = splitString(splitString(str, "{")[1], "}")[0];
                    int varIndex = 0;
                    findVar(varName, varIndex);
                    replaceAll(str, varName, std::to_string(varIndex));
                }

                int strSize = (int)str.size();
                if (strSize > 255) {
                    MessageManager::displayMessage("String should not be greater than 255 characters", 5, WARN);
                }
                bytecode.push_back((int)strSize);

                for (int i = 0; i < str.size(); i++) {
                    bytecode.push_back((int)str.at(i));
                }
            } else if (isNumber(value)) {
                bytecode.push_back((int)INSTRUCTION::LIT);
                bytecode.push_back(std::stoi(value));
            } else if (stringStartsWith(value, "0x")) {
                bytecode.push_back((int)INSTRUCTION::LIT);
                bytecode.push_back(std::stoul(value, nullptr, 16));
            } else if (value != "NULL") {
                bytecode.push_back((int)INSTRUCTION::LIT);
                int varIndex = 0;
                if (findStrVar(value, varIndex, false)) {
                    bytecode.push_back(varIndex);
                    bytecode.push_back((int)INSTRUCTION::READSTR);
                } else {
                    findVar(value, varIndex);
                    bytecode.push_back(varIndex);
                    bytecode.push_back((int)INSTRUCTION::READ);
                }
            }

            index += 2;
        } else if (instr == "ASSIGN") {
            std::string varName = parsedAssembly.at(index + 2);
            int varIndex = 0;
            bool isStr = false;
            if (stringEndsWith(parsedAssembly.at(index - 1), "\"")) {
                findStrVar(varName, varIndex);
                isStr = true;
            } else {
                findVar(varName, varIndex);
            }
            bytecode.push_back((int)INSTRUCTION::LIT);
            bytecode.push_back(varIndex);
            if (isStr) {
                bytecode.push_back((int)INSTRUCTION::ASSIGNSTR);
            } else {
                bytecode.push_back((int)INSTRUCTION::ASSIGN);
            }

            index += 3;
        } else if (instr == "UNARY") {
            std::string op = parsedAssembly.at(index + 1);
            if (op == "!" || op == "not") {
                bytecode.push_back((int)INSTRUCTION::NOT);
            } else if (op == "-") {
                bytecode.push_back((int)INSTRUCTION::SGN);
            }

            index += 2;
        } else {
            if (instructions.find(instr) == instructions.end()) {
                MessageManager::displayMessage("\"" + instr + "\" is not a valid instruction", 5, ERR);
                index++;
                return process(bytecode);
            }
            bytecode.push_back((int)instructions.at(instr));
            index++;
        }
    }

    return process(bytecode);
}

std::string Compiler::process(std::vector<int> bytecode) {
    std::string output = "";
    int index = 0;

    while (index < bytecode.size()) {
        int instr = bytecode.at(index);

        if ((INSTRUCTION)instr == INSTRUCTION::LIT) {
            output += (unsigned char)instr;
            int literal = bytecode.at(index + 1);

            for (int i = 0; i < 4; i++) {
                output += (unsigned char)(literal >> (i * 8));
            }

            index += 2;
        } else if ((INSTRUCTION)instr == INSTRUCTION::STR) {
            int strSize = bytecode.at(index + 1);
            for (int i = 0; i < strSize + 2; i++) {
                output += (unsigned char)(bytecode.at(index + i));
            }
            index += strSize + 2;
        } else {
            output += (unsigned char)instr;
            index++;
        }
    }

    return output;
}

bool Compiler::findVar(std::string varName, int& varIndex) {
    for (int index = 0; index < _variables.size(); index++) {
        if (_variables.at(index) == varName) {
            varIndex = index;
            return true;
        }
    }

    _variables.push_back(varName);
    varIndex = _variables.size() - 1;
    return false;
}

bool Compiler::findStrVar(std::string varName, int& varIndex, bool assignIfNotFound) {
    for (int index = 0; index < _strVariables.size(); index++) {
        if (_strVariables.at(index) == varName) {
            varIndex = index;
            return true;
        }
    }

    if (assignIfNotFound) {
        _strVariables.push_back(varName);
    }
    varIndex = _strVariables.size() - 1;
    return false;
}