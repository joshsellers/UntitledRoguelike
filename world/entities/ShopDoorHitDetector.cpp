#include "ShopDoorHitDetector.h"

ShopDoorHitDetector::ShopDoorHitDetector(sf::Vector2f pos, ShopExterior* shopExt) : Entity(NO_SAVE, pos, 0, 0, 0, false), _shopExt(shopExt) {
    setMaxHitPoints(1);
    heal(getMaxHitPoints());

    _hitBox.left = pos.x;
    _hitBox.top = pos.y;
    _hitBox.width = 32;
    _hitBox.height = 31;
}

void ShopDoorHitDetector::update() {

}

void ShopDoorHitDetector::draw(sf::RenderTexture& surface) {

}

void ShopDoorHitDetector::takeDamage(int damage, bool crit) {
    if (damage > 0) {
        _shopExt->blowOpenDoor();
        deactivate();
    }
}

void ShopDoorHitDetector::loadSprite(std::shared_ptr<sf::Texture> spriteSheet) {

}