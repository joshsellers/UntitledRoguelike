#ifndef _TERRAIN_GEN_PARAMETERS_H
#define _TERRAIN_GEN_PARAMETERS_H

#include "../core/MessageManager.h"

struct TerrainGenParameters {
    float terrainScale = 0;

    int octaves = 0;
    double warpSize = 0;
    double warpStrength = 0;
    double sampleRate = 0;

    double seaLevel = 0;
    double oceanMidRange = 0;
    double oceanShallowRange = 0;
    double sandRange = 0;
    double dirtLowRange = 0;
    double dirtHighRange = 0;
    double mountainLowRange = 0;
    double mountainMidRange = 0;
    double mountainHighRange = 0;

    double biomeXOffset = 0;
    double biomeYOffset = 0;

    int biomeOctaves = 0;
    double biomeSampleRate = 0;

    float biomeEdgeMixing = 0;

    sf::Vector2f tundraTemp;
    sf::Vector2f tundraPrec;
    
    sf::Vector2f desertTemp;
    sf::Vector2f desertPrec;

    sf::Vector2f savannaTemp;
    sf::Vector2f savannaPrec;

    sf::Vector2f forestTemp;
    sf::Vector2f forestPrec;

    sf::Vector2f fleshTemp;
    sf::Vector2f fleshPrec;

    sf::Vector2f fungusTemp;
    sf::Vector2f fungusPrec;
};

class TerrainGenInitializer {
public:
    static bool loadParameters() {
        std::ifstream terrainGenIn(_terrainGenPath);
        if (!terrainGenIn.good()) {
            MessageManager::displayMessage("Could not find " + _terrainGenPath, 5, WARN);
            terrainGenIn.close();
            return false;
        } else {
            std::string line;
            while (getline(terrainGenIn, line)) {
                replaceAll(line, " ", "");
                try {
                    if (stringStartsWith(line, "#")) continue;

                    const std::vector<std::string> parsedLine = splitString(line, "=");
                    if (parsedLine.size() != 2) continue;

                    const std::string parameterString = parsedLine.at(0);
                    const std::string valueString = parsedLine.at(1);

                    if (parameterString == "terrainScale") {
                        _terrainGenParameters.terrainScale = std::stof(valueString);
                    } else if (parameterString == "octaves") {
                        _terrainGenParameters.octaves = std::stoi(valueString);
                    } else if (parameterString == "warpSize") {
                        _terrainGenParameters.warpSize = std::stod(valueString);
                    } else if (parameterString == "warpStrength") {
                        _terrainGenParameters.warpStrength = std::stod(valueString);
                    } else if (parameterString == "sampleRate") {
                        _terrainGenParameters.sampleRate = std::stod(valueString);
                    } else if (parameterString == "seaLevel") {
                        _terrainGenParameters.seaLevel = std::stod(valueString);
                    } else if (parameterString == "oceanMidRange") {
                        _terrainGenParameters.oceanMidRange = std::stod(valueString);
                    } else if (parameterString == "oceanShallowRange") {
                        _terrainGenParameters.oceanShallowRange = std::stod(valueString);
                    } else if (parameterString == "sandRange") {
                        _terrainGenParameters.sandRange = std::stod(valueString);
                    } else if (parameterString == "dirtLowRange") {
                        _terrainGenParameters.dirtLowRange = std::stod(valueString);
                    } else if (parameterString == "dirtHighRange") {
                        _terrainGenParameters.dirtHighRange = std::stod(valueString);
                    } else if (parameterString == "mountainLowRange") {
                        _terrainGenParameters.mountainLowRange = std::stod(valueString);
                    } else if (parameterString == "mountainMidRange") {
                        _terrainGenParameters.mountainMidRange = std::stod(valueString);
                    } else if (parameterString == "mountainHighRange") {
                        _terrainGenParameters.mountainHighRange = std::stod(valueString);
                    } else MessageManager::displayMessage("Unknown terrain gen parameter: " + parameterString, 5, DEBUG);
                } catch (std::exception ex) {
                    MessageManager::displayMessage("Error reading terraingen.ini:\n" + (std::string)ex.what(), 5, WARN);
                }
            }
        }
        terrainGenIn.close();

        std::ifstream biomeGenIn(_biomeGenPath);
        if (!biomeGenIn.good()) {
            MessageManager::displayMessage("Could not find " + _biomeGenPath, 5, WARN);
            biomeGenIn.close();
            return false;
        } else {
            std::string line;
            while (getline(biomeGenIn, line)) {
                replaceAll(line, " ", "");
                try {
                    if (stringStartsWith(line, "#")) continue;

                    const std::vector<std::string> parsedLine = splitString(line, "=");
                    if (parsedLine.size() != 2) continue;

                    const std::string parameterString = parsedLine.at(0);
                    const std::string valueString = parsedLine.at(1);

                    if (parameterString == "biomeXOffset") {
                        _terrainGenParameters.biomeXOffset = std::stod(valueString);
                    } else if (parameterString == "biomeYOffset") {
                        _terrainGenParameters.biomeYOffset = std::stod(valueString);
                    } else if (parameterString == "biomeOctaves") {
                        _terrainGenParameters.biomeOctaves = std::stoi(valueString);
                    } else if (parameterString == "biomeSampleRate") {
                        _terrainGenParameters.biomeSampleRate = std::stod(valueString);
                    } else if (parameterString == "biomeEdgeMixing") {
                        _terrainGenParameters.biomeEdgeMixing = std::stof(valueString);
                    } else MessageManager::displayMessage("Unknown biome gen parameter: " + parameterString, 5, DEBUG);
                } catch (std::exception ex) {
                    MessageManager::displayMessage("Error reading biomegen.ini:\n" + (std::string)ex.what(), 5, WARN);
                }
            }
        }
        biomeGenIn.close();

        std::ifstream biomeDistIn(_biomeDistPath);
        if (!biomeDistIn.good()) {
            MessageManager::displayMessage("Could not find " + _biomeDistPath, 5, WARN);
            biomeDistIn.close();
            return false;
        } else {
            std::string line;
            while (getline(biomeDistIn, line)) {
                replaceAll(line, " ", "");
                try {
                    if (stringStartsWith(line, "#")) continue;

                    const std::vector<std::string> parsedLine = splitString(line, "=");
                    if (parsedLine.size() != 2) continue;

                    const std::string parameterString = parsedLine.at(0);
                    const std::string valueString = parsedLine.at(1);
                    
                    const std::vector<std::string> parsedVector = splitString(valueString, ",");
                    if (parsedVector.size() != 2) {
                        MessageManager::displayMessage("Bad syntax in biomedist.ini:\n" + valueString, 5, WARN);
                        continue;
                    }

                    const sf::Vector2f vector(std::stof(parsedVector.at(0)), std::stof(parsedVector.at(1)));

                    if (parameterString == "tundraTemp") _terrainGenParameters.tundraTemp = vector;
                    else if (parameterString == "tundraPrec") _terrainGenParameters.tundraPrec = vector;
                    else if (parameterString == "desertTemp") _terrainGenParameters.desertTemp = vector;
                    else if (parameterString == "desertPrec") _terrainGenParameters.desertPrec = vector;
                    else if (parameterString == "savannaTemp") _terrainGenParameters.savannaTemp = vector;
                    else if (parameterString == "savannaPrec") _terrainGenParameters.savannaPrec = vector;
                    else if (parameterString == "forestTemp") _terrainGenParameters.forestTemp = vector;
                    else if (parameterString == "forestPrec") _terrainGenParameters.forestPrec = vector;
                    else if (parameterString == "fleshTemp") _terrainGenParameters.fleshTemp = vector;
                    else if (parameterString == "fleshPrec") _terrainGenParameters.fleshPrec = vector;
                    else if (parameterString == "fungusTemp") _terrainGenParameters.fungusTemp = vector;
                    else if (parameterString == "fungusPrec") _terrainGenParameters.fungusPrec = vector;
                    else {
                        MessageManager::displayMessage("Unknown biome dist parameter: " + parameterString, 5, DEBUG);
                    }
                } catch (std::exception ex) {
                    MessageManager::displayMessage("Error reading biomedist.ini:\n" + (std::string)ex.what(), 5, WARN);
                }
            }
        }
        biomeDistIn.close();

        return true;
    }

    static TerrainGenParameters* getParameters() {
        return &_terrainGenParameters;
    }
private:
    static inline TerrainGenParameters _terrainGenParameters;

    static const inline std::string _terrainGenPath = "res/terraingen/terraingen.ini";
    static const inline std::string _biomeGenPath = "res/terraingen/biomegen.ini";
    static const inline std::string _biomeDistPath = "res/terraingen/biomedist.ini";
};

#endif