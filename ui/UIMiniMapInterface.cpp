#include "UIMiniMapInterface.h"
#include "../world/MiniMapGenerator.h"
#include "../world/TerrainColor.h"
#include "UIHandler.h"
#include "../world/World.h"
#include "UIControlsDisplay.h"

UIMiniMapInterface::UIMiniMapInterface(Player* player, sf::Font font) : UIElement(50, 50, 30, 30, false, false, font), 
_dispSize(getRelativeWidth(4)), _dispScale(5.25f) {
    _player = player;

    const float scale = (float)MiniMapGenerator::CHUNK_SIZE_SCALED / (float)CHUNK_SIZE; 
    const int centerX = ((float)MiniMapGenerator::CHUNK_SIZE_SCALED * (float)MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;
    _cameraX = centerX - _dispSize / 2;
    _cameraY = centerY - _dispSize / 2;
    _x = getRelativeWidth(_x) - _dispSize * _dispScale / 2.f;
    _y = getRelativeHeight(_y) - _dispSize * _dispScale / 2.f;

    const float padding = getRelativeWidth(1.f);
    _shape.setPosition(_x - padding, _y - padding);
    _shape.setSize(sf::Vector2f(_dispSize * _dispScale + padding * 2, _dispSize * _dispScale + padding * 2));
    _shape.setTexture(UIHandler::getUISpriteSheet().get());
    _shape.setTextureRect(sf::IntRect(96, 96, 144, 144));

    _shopIcon.setTexture(UIHandler::getUISpriteSheet().get());
    _shopIcon.setTextureRect(sf::IntRect(96, 240, 16, 16));
    _shopIcon.setSize(sf::Vector2f(getRelativeWidth(1.25f), getRelativeWidth(1.25f)));

    _pinIcon.setTexture(UIHandler::getUISpriteSheet().get());
    _pinIcon.setTextureRect(sf::IntRect(32, 224, 32, 32));
    _pinIcon.setSize(sf::Vector2f(getRelativeWidth(2.5f), getRelativeWidth(2.5f)));

    _playerIcon.setTexture(UIHandler::getUISpriteSheet().get());
    _playerIcon.setTextureRect(sf::IntRect(64, 224, 32, 32));
    _playerIcon.setSize(sf::Vector2f(getRelativeWidth(2.4f), getRelativeWidth(2.4f)));

    _bossIcon.setTexture(UIHandler::getUISpriteSheet().get());
    _bossIcon.setTextureRect(sf::IntRect(112, 240, 16, 16));
    _bossIcon.setSize(sf::Vector2f(getRelativeWidth(1.75f), getRelativeWidth(1.75f)));
}

void UIMiniMapInterface::update() {
    if (GamePad::isConnected()) {
        const float xAxis = GamePad::getRightStickXAxis();
        const float yAxis = GamePad::getRightStickYAxis();

        const float angle = std::atan2(yAxis, xAxis);

        const float absX = std::abs(xAxis);
        const float absY = std::abs(yAxis);
        const float mag = std::min(100.f, std::sqrt(xAxis * xAxis + yAxis * yAxis)) / 100.f;

        constexpr float panSpeed = 4.f;
        if (xAxis > 20.f || xAxis < -20.f) _cameraX += panSpeed * std::cos(angle) * mag;
        if (yAxis > 20.f || yAxis < -20.f) _cameraY += panSpeed * std::sin(angle) * mag;
        
        constexpr long long dpadHoldInterval = 100LL;
        if (currentTimeMillis() - _lastDpadPressTime >= dpadHoldInterval) {
            if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_UP)) zoom(1.f);
            else if (GamePad::isButtonPressed(GAMEPAD_BUTTON::DPAD_DOWN)) zoom(-1.f);
            _lastDpadPressTime = currentTimeMillis();
        }
    }
}

void UIMiniMapInterface::draw(sf::RenderTexture& surface) {
    surface.draw(_shape);

    std::vector<sf::Vector2i> convertedPoiLocations; 
    std::vector<sf::Vector2i> convertedPinLocations;
    for (int y = 0; y < _dispSize; y++) {
        for (int x = 0; x < _dispSize; x++) {
            TERRAIN_TYPE terrainType = MiniMapGenerator::getData((_cameraX + x) + (_cameraY + y) * (MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS * MiniMapGenerator::CHUNK_SIZE_SCALED));
            if (terrainType == TERRAIN_TYPE::EMPTY) continue;

            sf::Uint32 col = 0;
            if (terrainType == TERRAIN_TYPE::MOUNTAIN_LOW) {
                col = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_LOW;
            } else if (terrainType == TERRAIN_TYPE::MOUNTAIN_MID) {
                col = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_MID;
            } else if (terrainType == TERRAIN_TYPE::MOUNTAIN_HIGH) {
                col = (sf::Uint32)TERRAIN_COLOR::MOUNTAIN_HIGH;
            } else if (terrainType == TERRAIN_TYPE::SAND) {
                col = (sf::Uint32)TERRAIN_COLOR::SAND;
            } else if (terrainType == TERRAIN_TYPE::WATER) {
                col = (sf::Uint32)TERRAIN_COLOR::WATER_MID;
            } else if (terrainType == TERRAIN_TYPE::GRASS) {
                col = (sf::Uint32)TERRAIN_COLOR::DIRT_LOW;
            } else if (terrainType == TERRAIN_TYPE::TUNDRA) {
                col = (sf::Uint32)TERRAIN_COLOR::TUNDRA;
            } else if (terrainType == TERRAIN_TYPE::DESERT) {
                col = (sf::Uint32)TERRAIN_COLOR::DESERT;
            } else if (terrainType == TERRAIN_TYPE::SAVANNA) {
                col = (sf::Uint32)TERRAIN_COLOR::SAVANNA;
            } else if (terrainType == TERRAIN_TYPE::GRASS_FOREST) {
                col = (sf::Uint32)TERRAIN_COLOR::FOREST;
            } else if (terrainType == TERRAIN_TYPE::FLESH) {
                col = (sf::Uint32)TERRAIN_COLOR::FLESH;
            } else if (terrainType == TERRAIN_TYPE::FUNGUS) {
                col = (sf::Uint32)TERRAIN_COLOR::FUNGUS;
            }

            col = (col << 8) + 0xFF;
            
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(_dispScale, _dispScale));
            rect.setPosition(_x + x * _dispScale, _y + y * _dispScale);
            rect.setFillColor(sf::Color(col));
            surface.draw(rect);

            for (const auto& location : MiniMapGenerator::getPoiLocations()) {
                if (location.x == _cameraX + x && location.y == _cameraY + y) {
                    convertedPoiLocations.push_back(sf::Vector2i(_x + x * _dispScale, _y + y * _dispScale));
                }
            }

            for (const auto& location : MiniMapGenerator::getPinLocations()) {
                if (location.x == _cameraX + x && location.y == _cameraY + y) {
                    convertedPinLocations.push_back(sf::Vector2i(_x + x * _dispScale, _y + y * _dispScale));
                }
            }
        }
    }

    for (const auto& location : convertedPoiLocations) {
        _shopIcon.setPosition((float)(location.x) - _shopIcon.getSize().x / 2.f + (float)_dispScale / 2.f, (float)(location.y) - _shopIcon.getSize().y / 2.f + (float)_dispScale / 2.f);
        surface.draw(_shopIcon);
    }

    for (const auto& location : convertedPinLocations) {
        _pinIcon.setPosition((float)(location.x) - _pinIcon.getSize().x / 2.f + (float)_dispScale / 2.f, (float)(location.y) - _pinIcon.getSize().y + (float)_dispScale / 2.f);
        surface.draw(_pinIcon);
    }

    const int centerX = ((float)MiniMapGenerator::CHUNK_SIZE_SCALED * (float)MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;
    const float scale = (float)MiniMapGenerator::CHUNK_SIZE_SCALED / (float)CHUNK_SIZE;

    const sf::Vector2f playerPos = _player->getPosition();
    _playerIcon.setPosition(
        (_x + ((playerPos.x * scale + centerX) - _cameraX) * _dispScale) - (_playerIcon.getSize().x / 2.f),
        (_y + ((playerPos.y * scale + centerY) - _cameraY) * _dispScale) - (_playerIcon.getSize().y / 2.f)
    );

    if (_playerIcon.getPosition().x + _playerIcon.getSize().x / 2.f > _x
        && _playerIcon.getPosition().x + _playerIcon.getSize().x / 2.f < _x + _dispSize * _dispScale
        && _playerIcon.getPosition().y + _playerIcon.getSize().y / 2.f > _y
        && _playerIcon.getPosition().y + _playerIcon.getSize().y / 2.f < _y + _dispSize * _dispScale) {
        surface.draw(_playerIcon);
    }

    if (_player->getWorld()->bossIsActive()) {
        const sf::Vector2f bossPos = _player->getWorld()->getCurrentBoss()->getPosition();

        _bossIcon.setPosition(
            (_x + ((bossPos.x * scale + centerX) - _cameraX) * _dispScale) - (_bossIcon.getSize().x / 2.f),
            (_y + ((bossPos.y * scale + centerY) - _cameraY) * _dispScale) - (_bossIcon.getSize().y / 2.f)
        );

        if (_bossIcon.getPosition().x + _bossIcon.getSize().x / 2.f > _x
            && _bossIcon.getPosition().x + _bossIcon.getSize().x / 2.f < _x + _dispSize * _dispScale
            && _bossIcon.getPosition().y + _bossIcon.getSize().y / 2.f > _y
            && _bossIcon.getPosition().y + _bossIcon.getSize().y / 2.f < _y + _dispSize * _dispScale) {
            surface.draw(_bossIcon);
        }
    }

    if (GamePad::isConnected()) {
        drawControls(surface);
    }
}

void UIMiniMapInterface::drawControls(sf::RenderTexture& surface) {
    sf::RectangleShape button;
    button.setTexture(UIHandler::getUISpriteSheet().get());
    button.setSize(sf::Vector2f(getRelativeHeight(4.f), getRelativeHeight(4.f)));
    sf::Text controlLabel; 
    controlLabel.setFont(_font);
    controlLabel.setCharacterSize(getRelativeWidth(1.25f));
    controlLabel.setOutlineThickness(2.f);
    const float x = 43.f;
    const float xTextPadding = 3.5f;
    const float y = 70.f;
    const float ySpacing = 4.f;

    button.setTextureRect(UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON::RIGHT_STICK));
    button.setPosition(getRelativePos(x, y + ySpacing * 0));
    surface.draw(button);

    controlLabel.setString("look around");
    controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 0 + 0.5f));
    surface.draw(controlLabel);

    button.setTextureRect(UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON::RIGHT_STICK, true));
    button.setPosition(getRelativePos(x, y + ySpacing * 1));
    surface.draw(button);

    controlLabel.setString("center map on your location");
    controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 1 + 0.5f));
    surface.draw(controlLabel);

    button.setTextureRect(UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON::LEFT_STICK));
    button.setPosition(getRelativePos(x, y + ySpacing * 2));
    surface.draw(button);

    controlLabel.setString("drop/remove pin");
    controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 2 + 0.5f));
    surface.draw(controlLabel);

    button.setTextureRect(UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON::DPAD_UP));
    button.setPosition(getRelativePos(x, y + ySpacing * 3));
    surface.draw(button);
    controlLabel.setString("zoom in");
    controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 3 + 0.5f));
    surface.draw(controlLabel);

    button.setTextureRect(UIControlsDisplay::getButtonIcon(GAMEPAD_BUTTON::DPAD_DOWN));
    button.setPosition(getRelativePos(x, y + ySpacing * 4));
    surface.draw(button);
    controlLabel.setString("zoom out");
    controlLabel.setPosition(getRelativePos(x + xTextPadding, y + ySpacing * 4 + 1.f));
    surface.draw(controlLabel);
}

void UIMiniMapInterface::controllerButtonPressed(GAMEPAD_BUTTON button) {

}

void UIMiniMapInterface::controllerButtonReleased(GAMEPAD_BUTTON button) {
    if (button == GAMEPAD_BUTTON::RIGHT_STICK) {
        centerOnPlayer();
    } else if (button == GAMEPAD_BUTTON::LEFT_STICK) {
        MiniMapGenerator::dropPin(_player->getPosition());
    }
}

void UIMiniMapInterface::mouseButtonPressed(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Middle) {
        _mPosOnClick.x = mx;
        _mPosOnClick.y = my;
        _cameraPosOnClick = sf::Vector2i(_cameraX, _cameraY);
        _middleButtonPressed = true;
    }
}

void UIMiniMapInterface::mouseButtonReleased(const int mx, const int my, const int button) {
    if (button == sf::Mouse::Middle) {
        _middleButtonPressed = false;
    } else if (button == sf::Mouse::Right) {
        centerOnPlayer();
    } else if (button == sf::Mouse::Left) {
        MiniMapGenerator::dropPin(_player->getPosition());
    }
}

void UIMiniMapInterface::mouseMoved(const int mx, const int my) {
    if (_middleButtonPressed) {
        const int dX = _mPosOnClick.x - mx;
        const int dY = _mPosOnClick.y - my;

        _cameraX = ((float)dX) / _dispScale + _cameraPosOnClick.x;
        _cameraY = ((float)dY) / _dispScale + _cameraPosOnClick.y;
    }
}

void UIMiniMapInterface::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    zoom(mouseWheelScroll.delta);
}

void UIMiniMapInterface::textEntered(const sf::Uint32 character) {
}

void UIMiniMapInterface::centerOnPlayer() {
    const int centerX = ((float)MiniMapGenerator::CHUNK_SIZE_SCALED * (float)MiniMapGenerator::MAP_SIZE_DEFAULT_CHUNKS) / 2.f;
    const int centerY = centerX;
    const float scale = (float)MiniMapGenerator::CHUNK_SIZE_SCALED / (float)CHUNK_SIZE;
    const sf::Vector2f playerPos = _player->getPosition();
    _cameraX = playerPos.x * scale - ((float)_dispSize / 2) + centerX;
    _cameraY = playerPos.y * scale - ((float)_dispSize / 2) + centerY;
}

void UIMiniMapInterface::zoom(float factor) {
    _dispScale += factor;
    if (_dispScale < 1) _dispScale = 1;

    const int oldDispSize = _dispSize;
    const float padding = getRelativeWidth(1.f);
    _dispSize = (_shape.getSize().x - padding * 2) / _dispScale;

    const int previousCenterX = _cameraX + oldDispSize / 2;
    const int previousCenterY = _cameraY + oldDispSize / 2;
    _cameraX = previousCenterX - _dispSize / 2;
    _cameraY = previousCenterY - _dispSize / 2;
}
