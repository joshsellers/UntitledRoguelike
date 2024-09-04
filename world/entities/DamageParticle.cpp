#include "DamageParticle.h"
#include "../../core/Util.h"

DamageParticle::DamageParticle(sf::Vector2f pos, unsigned int damageAmount) : Entity(NO_SAVE, pos, 0, 0, 0, false), _amount(damageAmount) {
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
    std::string damageString = std::to_string(_amount);
    for (int i = 0; i < damageString.size(); i++) {
        std::string character(1, damageString.at(i));

        sf::Sprite sprite;
        sprite.setTexture(*spriteSheet);
        sprite.setTextureRect(
            sf::IntRect(std::stoi(character) * TILE_SIZE, 30 * TILE_SIZE, TILE_SIZE, TILE_SIZE)
        );
        sprite.setPosition(getPosition().x + 12 * i - (damageString.length() * 12), getPosition().y - TILE_SIZE);
        
        sprite.setColor(sf::Color(0xF50000FF));

        _sprites.push_back(sprite);
    }

    // Fixes damage particles being invisible with frustum culling
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(0, 0, damageString.length() * 12, TILE_SIZE)
    );
    _sprite.setPosition(getPosition().x - (damageString.length() * 12), getPosition().y - TILE_SIZE);
}
