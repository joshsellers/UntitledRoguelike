#include "HairyEntity.h"
#include "../../inventory/hair/Hairstyle.h"

HairyEntity::HairyEntity(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight) 
: Entity(saveId, pos, baseSpeed, spriteWidth, spriteHeight, false) {
}

void HairyEntity::setHairstyle(int hairstyleId) {
    _equippedHair[(int)Hairstyle::HAIRSTYLES[hairstyleId]->getHairType()] = hairstyleId;
}

void HairyEntity::shave(HAIR_TYPE hairType) {
    _equippedHair[(int)hairType] = SHAVED;
}

int HairyEntity::getHairstyle(HAIR_TYPE hairType) const {
    return _equippedHair[(int)hairType];
}

void HairyEntity::setHairColor(HAIR_TYPE hairType, int color) {
    _hairSprites[(int)hairType].setColor(sf::Color(color));
}

void HairyEntity::drawHair(sf::RenderTexture& surface) {
    for (int i = 0; i < HAIR_SLOT_COUNT; i++) {
        if (_equippedHair[i] != SHAVED) {
            auto& sprite = _hairSprites[i];
            sf::IntRect textRect = Hairstyle::HAIRSTYLES[_equippedHair[i]]->getTextureRect();

            constexpr int spriteHeight = 3;
            int yOffset = isMoving() || isSwimming() ? ((_numSteps >> _animSpeed) & 3) * TILE_SIZE * spriteHeight : 0; 
            int spriteY = textRect.top + TILE_SIZE;

            sprite.setTextureRect(sf::IntRect(
                textRect.left + TILE_SIZE * 3 * _facingDir, spriteY + yOffset, TILE_SIZE * 3, TILE_SIZE * spriteHeight)
            );
            sprite.setPosition(sf::Vector2f(_sprite.getPosition().x - TILE_SIZE, _sprite.getPosition().y - TILE_SIZE));
            surface.draw(sprite);
        }
    }
}

void HairyEntity::initHairSprites(std::shared_ptr<sf::Texture> spriteSheet) {
    for (auto& hairSprite : _hairSprites) {
        hairSprite.setTexture(*spriteSheet);
    }
}

MOVING_DIRECTION HairyEntity::getFacingDir() const {
    return _facingDir;
}
