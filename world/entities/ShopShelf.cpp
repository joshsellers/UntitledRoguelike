#include "ShopShelf.h"
#include "../../core/Util.h"
#include "../World.h"

constexpr int SHELF_SHEET_SIZE = 4;
constexpr int SHELF_TILE_SIZE = 7;
constexpr int SHELF_SHEET_X = 912 + TILE_SIZE;
constexpr int SHELF_SHEET_Y = 244;
constexpr int ITEMS_PER_SHELF = 7;
constexpr int SHELF_COUNT = 4;

ShopShelf::ShopShelf(sf::Vector2f pos) : Entity(NO_SAVE, pos, 0, 64, 43, false) {
    _entityType = "shopshelf";

    _hasColliders = true;
    sf::FloatRect collider;
    collider.left = _pos.x;
    collider.top = _pos.y + 15;
    collider.width = 58;
    collider.height = 3;
    _colliders.push_back(collider);

    setMaxHitPoints(10000000);
    heal(getMaxHitPoints());
}

void ShopShelf::update() {
    if (!getWorld()->playerIsInShop()) deactivate();
}

void ShopShelf::draw(sf::RenderTexture& surface) {
    surface.draw(_sprite);

    constexpr float itemSpacing = 1.f;
    for (int y = 0; y < SHELF_COUNT; y++) {
        for (int x = 0; x < ITEMS_PER_SHELF; x++) {
            const int item = _items.at(x + y * ITEMS_PER_SHELF);
            if (item == NO_ITEM) continue;
            _itemSprite.setTextureRect(sf::IntRect(
                SHELF_SHEET_X + (item % SHELF_SHEET_SIZE) * SHELF_TILE_SIZE, SHELF_SHEET_Y + (item / SHELF_SHEET_SIZE) * SHELF_TILE_SIZE,
                SHELF_TILE_SIZE, SHELF_TILE_SIZE
            ));
            _itemSprite.setPosition(_pos.x + 1 + (SHELF_TILE_SIZE + itemSpacing) * x, _pos.y + 11 + (SHELF_TILE_SIZE + 1.f) * y);
            surface.draw(_itemSprite);
        }
    }
}

void ShopShelf::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {
    _sprite.setTexture(*spriteSheet);
    _sprite.setTextureRect(sf::IntRect(
        912 + TILE_SIZE, 201, 58, 43
    ));
    _sprite.setPosition(getPosition());

    _itemSprite.setTexture(*spriteSheet);
}

void ShopShelf::setWorld(World* world) {
    _world = world;

    for (int i = 0; i < ITEMS_PER_SHELF * SHELF_COUNT; i++) {
        const float hasItemChance = 1.f * getWorld()->getCurrentWaveNumber() / 50.f;
        _items.push_back(randomChance(hasItemChance) ? randomInt(0, (SHELF_SHEET_SIZE * SHELF_SHEET_SIZE) - 1) : NO_ITEM);
    }
}
