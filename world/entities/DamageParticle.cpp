#include "DamageParticle.h"
#include "../../core/Util.h"

DamageParticle::DamageParticle(sf::Vector2f pos, unsigned int damageAmount, bool crit) : Entity(NO_SAVE, pos, 0, 0, 0, false), 
_amount(damageAmount), _crit(crit) {
    _entityType = "damageparticle";
    _spawnTimeMillis = currentTimeMillis();
}

void DamageParticle::update() {
    for (auto& sprite : _sprites) sprite.move(0, -1);

    if (currentTimeMillis() - _spawnTimeMillis >= DAMAGE_PARTICLE_LIFETIME) deactivate();
}

void DamageParticle::draw(sf::RenderTexture& surface) {
    for (auto& sprite : _sprites) {
        surface.draw(sprite);
    }
}

void DamageParticle::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    constexpr int numeralTileSize = 10;
    constexpr int charWidth = 7;
    std::string damageString = std::to_string(_amount);
    for (int i = 0; i < damageString.size(); i++) {
        std::string character(1, damageString.at(i));
        
        sf::Sprite sprite;
        sprite.setTexture(*spriteSheet);
        sprite.setTextureRect(
            sf::IntRect(std::stoi(character) * numeralTileSize, 486, numeralTileSize, numeralTileSize)
        );
        sprite.setPosition(getPosition().x + charWidth * i - (damageString.length() * charWidth) / 2.f, getPosition().y - numeralTileSize);
        
        sprite.setColor(sf::Color((_crit ? 0xA700E0FF : 0xF50000FF)));

        _sprites.push_back(sprite);
    }

    // Fixes damage particles being invisible with frustum culling
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(0, 0, damageString.length() * charWidth, numeralTileSize)
    );
    _sprite.setPosition(getPosition().x - (damageString.length() * charWidth), getPosition().y - numeralTileSize);
}
