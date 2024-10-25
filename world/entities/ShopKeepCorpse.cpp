#include "ShopKeepCorpse.h"
#include "../World.h"

ShopKeepCorpse::ShopKeepCorpse(sf::Vector2f pos, std::shared_ptr<sf::Texture> spriteSheet) 
: Entity(NO_SAVE, pos, 0, 16, 32, false) {
    loadSprite(spriteSheet);
    _entityType = "shopkeep";
}

void ShopKeepCorpse::update() {
    if (!getWorld()->playerIsInShop()) deactivate();
}

void ShopKeepCorpse::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);
}

void ShopKeepCorpse::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(
        sf::IntRect(0, 45 * TILE_SIZE, 32, 16)
    );
    _sprite.setPosition(getPosition());
}
