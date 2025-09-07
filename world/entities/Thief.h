#ifndef _THIEF_H
#define _THIEF_H

#include "Entity.h"

class Thief : public Entity {
public:
    Thief(sf::Vector2f pos, bool chasing = true);

    void update();
    void draw(sf::RenderTexture& surface);

    virtual std::string getSaveData() const;

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);
protected:
    virtual void damage(int damage);
private:
    sf::Sprite _wavesSprite;

    sf::Sprite _clothingHeadSprite;
    sf::Sprite _clothingBodySprite;
    sf::Sprite _clothingLegsSprite;
    sf::Sprite _clothingFeetSprite;

    bool _chasing = true;
    unsigned int _pennyAmt = 0;

    void drawEquipables(sf::RenderTexture& surface);
    void drawApparel(sf::Sprite& sprite, EQUIPMENT_TYPE equipType, sf::RenderTexture& surface);
};

#endif