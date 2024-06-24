#include "ShaderManager.h"
#include "Util.h"
#include "MessageManager.h"
#include "../world/PerlinNoise.hpp"
#include "../world/Chunk.h"

void ShaderManager::compileShaders() {
    for (auto& shaderName : _shaderNames) {
        sf::Shader::Type shaderType = (sf::Shader::Type)-1;

        if (splitString(shaderName, "_").size() != 2) {
            MessageManager::displayMessage("Shader names must be followed by shader type:\n" + (std::string)"\"" + shaderName + "\"", 5, WARN);
            continue;
        }
        std::string typeNameTag = splitString(shaderName, "_")[1];
        if (typeNameTag == "frag") shaderType = sf::Shader::Type::Fragment;
        else if (typeNameTag == "geo") shaderType = sf::Shader::Type::Geometry;
        else if (typeNameTag == "vert") shaderType = sf::Shader::Type::Vertex;
        else {
            MessageManager::displayMessage("Unknown shader type: " + shaderName, 5, WARN);
            continue;
        }

        if (!_shaders[shaderName].loadFromFile("res/shaders/" + shaderName + ".glsl", shaderType)) {
            MessageManager::displayMessage("Shader compilation failed for \"" + shaderName + "\"", 5, ERR);
        }
    }
}

void ShaderManager::configureShaders() {
    // waves_frag
    getShader("waves_frag")->setUniform("texture", sf::Shader::CurrentTexture);
    getShader("waves_frag")->setUniform("resolution", sf::Glsl::Vec2(CHUNK_SIZE, CHUNK_SIZE));

    const siv::PerlinNoise perlin{ (siv::PerlinNoise::seed_type)currentTimeMillis() };
    sf::Image* wavesNoiseImage = new sf::Image;
    constexpr int scale = 1;
    wavesNoiseImage->create(_wavesNoiseSize * scale, _wavesNoiseSize * scale);
    for (unsigned int y = 0; y < _wavesNoiseSize * scale; y++) {
        for (unsigned int x = 0; x < _wavesNoiseSize * scale; x++) {
            double noise = perlin.octave2D_01(x * 0.1, y * 0.1, 1);
            unsigned char color = (((float)noise) * (float)255);
            wavesNoiseImage->setPixel(x, y, sf::Color(color, color, color, 255));
        }
    }

    _wavesNoiseTexture.loadFromImage(*wavesNoiseImage);
    _wavesNoiseTexture.setRepeated(true);
    delete wavesNoiseImage;
    getShader("waves_frag")->setUniform("wavesNoiseTex", _wavesNoiseTexture);

    sf::Image* capsNoiseImage = new sf::Image;
    capsNoiseImage->create(_capsNoiseSize, _capsNoiseSize, sf::Color::Black);
    for (unsigned int y = 0; y < _capsNoiseSize; y++) {
        for (unsigned int x = 0; x < _capsNoiseSize; x++) {
            constexpr int capChance = 350;
            bool drawCap = randomInt(0, capChance) == 0;
            if (drawCap) capsNoiseImage->setPixel(x, y, sf::Color::White);
        }
    }
    _capsNoiseTexture.loadFromImage(*capsNoiseImage);
    _capsNoiseTexture.setRepeated(true);
    delete capsNoiseImage;
    getShader("waves_frag")->setUniform("capsNoiseTex", _capsNoiseTexture);
    //
}

void ShaderManager::updateShaders() {
    // waves_frag
    getShader("waves_frag")->setUniform("time", _wavesTimer / _wavesNoiseSize);
    _wavesTimer += 0.05f;

    getShader("waves_frag")->setUniform("capsTime", _capsTimer / _capsNoiseSize);
    _capsTimer += 0.05;
    //
}

sf::Shader* ShaderManager::getShader(std::string name) {
    return &_shaders[name];
}
