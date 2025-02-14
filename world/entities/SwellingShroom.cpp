#include "SwellingShroom.h"
#include "../../core/MessageManager.h"
#include "../World.h"
#include "Spore.h"

SwellingShroom::SwellingShroom(sf::Vector2f pos) : Entity(NO_SAVE, pos, 0, 32, 32, false), _spawnTime(currentTimeMillis()) {
    _entityType = "swellingshroom";
}

void SwellingShroom::update() {
    constexpr int ticksPerFrame = 4;
    constexpr int frameCount = 13;
    const int frameNumber = ((_numSteps / ticksPerFrame) % frameCount);
    if (frameNumber <= 5) {
        _sprite.setTextureRect(sf::IntRect(158 << SPRITE_SHEET_SHIFT, (43 << SPRITE_SHEET_SHIFT) + frameNumber * TILE_SIZE * 2, TILE_SIZE * 2, TILE_SIZE * 2));
        _numSteps++;
    }

    constexpr long long popDelay = 1500LL;
    if (currentTimeMillis() - _spawnTime >= popDelay) {
        const int sporeCount = randomInt(500, 600);
        constexpr int cloudRadius = 32;

        for (int i = 0; i < sporeCount; i++) {
            const float r = cloudRadius * std::sqrt(static_cast<float>(rand()) / RAND_MAX);
            const float theta = static_cast<float>(rand()) / RAND_MAX * 2 * PI;
            const float x = r * std::cos(theta);
            const float y = r * std::sin(theta);

            auto& spore = std::shared_ptr<Spore>(new Spore(sf::Vector2f(_pos.x + x, _pos.y + y)));
            spore->setWorld(getWorld());
            spore->loadSprite(getWorld()->getSpriteSheet());
            getWorld()->addEntity(spore);
        }

        deactivate();
    }
}

void SwellingShroom::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void SwellingShroom::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(158 << SPRITE_SHEET_SHIFT, 43 << SPRITE_SHEET_SHIFT, TILE_SIZE * 2, TILE_SIZE * 2));
    _sprite.setPosition(getPosition());
    _sprite.setOrigin((float)TILE_SIZE * 2 / 2, 0);
}
