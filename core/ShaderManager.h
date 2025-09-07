#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H

#include <vector>
#include <string>
#include <map>
#include "SFML/Graphics.hpp"

class ShaderManager {
public:
    static void compileShaders();
    static void configureShaders();

    static void updateShaders();

    static sf::Shader* getShader(std::string name);
private:
    inline static std::map<std::string, sf::Shader> _shaders;

    inline static std::vector<std::string> _shaderNames = {
        "waves_frag", "damage_frag", "genericwaves_frag", "bossterrain_frag"
    };

    inline static const int _wavesNoiseSize = 500;
    inline static sf::Texture _wavesNoiseTexture;
    inline static float _wavesTimer = 0.f;

    inline static const int _capsNoiseSize = 500;
    inline static sf::Texture _capsNoiseTexture;
    inline static float _capsTimer = 0.f;
};

#endif