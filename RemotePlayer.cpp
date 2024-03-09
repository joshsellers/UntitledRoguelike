#include "RemotePlayer.h"

RemotePlayer::RemotePlayer(SteamNetworkingIdentity identityPeer, sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused) 
    : Player(pos, window, gamePaused) {
    _identityPeer = identityPeer;
}

void RemotePlayer::update() {
    if (currentTimeMillis() - _lastUpdateTime > 5000) deactivate();
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

        move(deltaX, deltaY);

        _sprite.setPosition(getPosition());

        _lastUpdateTime = currentTimeMillis();
    }
}
