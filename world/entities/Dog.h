#ifndef _DOG_H
#define _DOG_H

#include "Entity.h"

class Dog : public Entity {
public:
    Dog(sf::Vector2f pos);

    void update();

    void draw(sf::RenderTexture& surface);

    void setParent(Entity* parent);
    Entity* getParent() const;
    bool hasParent() const;

    void loadSprite(std::shared_ptr<sf::Texture> spriteSheet);

    virtual std::string getSaveData() const; 

    friend class SaveManager;
protected:
    virtual void damage(int damage);
private:
    bool _hasOwner = false;
    Entity* _parent;

    sf::Sprite _wavesSprite;

    boost::random::mt19937 _gen = boost::random::mt19937();

    long long _lastDamageTime = 0LL;
};

#endif