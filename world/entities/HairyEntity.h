#ifndef _HAIRY_ENTITY_H
#define _HAIRY_ENTITY_H

#include "Entity.h"
#include "../../inventory/hair/HairType.h"

constexpr int SHAVED = -1;
constexpr unsigned int HAIR_SLOT_COUNT = 3;

class HairyEntity : public Entity {
public:
    HairyEntity(ENTITY_SAVE_ID saveId, sf::Vector2f pos, float baseSpeed, const int spriteWidth, const int spriteHeight);

    void setHairstyle(int hairstyleId);
    void shave(HAIR_TYPE hairType);
    int getHairstyle(HAIR_TYPE hairType) const;

    void setHairColor(HAIR_TYPE hairType, int color);

    void drawHair(sf::RenderTexture& surface);

    void initHairSprites(std::shared_ptr<sf::Texture> spriteSheet);

protected:
    MOVING_DIRECTION _facingDir = (MOVING_DIRECTION)_movingDir;

private:
    int _equippedHair[HAIR_SLOT_COUNT] = {
        SHAVED, SHAVED, SHAVED
    };

    sf::Sprite _hairSprites[HAIR_SLOT_COUNT];
};

#endif