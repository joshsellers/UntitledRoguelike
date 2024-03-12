#include "RemotePlayer.h"
#include "World.h"
#include "Globals.h"

RemotePlayer::RemotePlayer(SteamNetworkingIdentity identityPeer, sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused) 
    : Player(pos, window, gamePaused) {
    _identityPeer = identityPeer;
    _multiplayerSendUpdates = false;
}

void RemotePlayer::update() {
    if (!IS_MULTIPLAYER_CONNECTED && currentTimeMillis() - _lastUpdateTime > 1000) deactivate();
    
    if (IS_MULTIPLAYER_CONNECTED && currentTimeMillis() - _lastUpdateTime > 100) move(0, 0);

    if (isSwimming()) _animSpeed = 4;
    else if (!isSwimming() && isMoving()) _animSpeed = 3;

    if (isSwimming()) {
        _hitBoxYOffset = TILE_SIZE;
        _hitBox.height = TILE_SIZE;
    } else {
        _hitBoxYOffset = 0;
        _hitBox.height = TILE_SIZE * 2;
    }
    _hitBox.left = getPosition().x + _hitBoxXOffset;
    _hitBox.top = getPosition().y + _hitBoxYOffset;

}

void RemotePlayer::messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer) {
    if (message.payloadType == PayloadType::PLAYER_DATA && identityPeer == _identityPeer) {
        std::vector<std::string> parsedData = splitString(message.data, ",");

        float x = std::stof(parsedData[0]);
        float y = std::stof(parsedData[1]);
        
        float currentX = getPosition().x;
        float currentY = getPosition().y;
        float deltaX = x - currentX;
        float deltaY = y - currentY;

        if (deltaY < 0) {
            _movingDir = UP;
        } else if (deltaY > 0) {
            _movingDir = DOWN;
        } else if (deltaX < 0) {
            _movingDir = LEFT;
        } else if (deltaX > 0) {
            _movingDir = RIGHT;
        }

        _isDodging = std::stoi(parsedData[2]);

        move(deltaX, deltaY);

        _sprite.setPosition(getPosition());

        _multiplayerAimAngle = std::stof(parsedData[3]);

        _lastUpdateTime = currentTimeMillis();
    } else if (message.payloadType == PayloadType::INVENTORY_DATA && identityPeer == _identityPeer) {
        std::vector<std::string> parsedData = splitString(message.data, ".");
        std::string functionName = parsedData[0];

        std::vector<std::string> parsedParams = splitString(parsedData[1], ",");
        if (functionName == "addItem") {
            getInventory().addItem(std::stoi(parsedParams[0]), std::stoi(parsedParams[1]));
        } else if (functionName == "removeItemAt") {
            getInventory().removeItemAt(std::stoi(parsedParams[0]), std::stoi(parsedParams[1]));
        } else if (functionName == "dropItem") {
            getInventory().dropItem(std::stoi(parsedParams[0]), std::stoi(parsedParams[1]));
        } else if (functionName == "equip") {
            getInventory().equip(std::stoi(parsedParams[0]), (EQUIPMENT_TYPE)std::stoi(parsedParams[1]));
        }
    } else if (message.payloadType == PayloadType::PLAYER_AIM_ANGLE && identityPeer == _identityPeer) {
        _multiplayerAimAngle = std::stof(message.data);
    }
}

void RemotePlayer::damage(int damage) {
    if (!isDodging()) {
        //probably send something to connected player here
    }
}

TERRAIN_TYPE RemotePlayer::getCurrentTerrain() {
    sf::Vector2f feetPos = sf::Vector2f(((int)_pos.x + PLAYER_WIDTH / 2), ((int)_pos.y + PLAYER_HEIGHT));
    return _world->getTerrainDataAt(feetPos);
}
