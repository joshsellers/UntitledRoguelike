#include "RemotePlayer.h"

RemotePlayer::RemotePlayer(SteamNetworkingIdentity identityPeer, sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused) 
    : Player(pos, window, gamePaused) {
    _identityPeer = identityPeer;
}

void RemotePlayer::update() {
}

void RemotePlayer::messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer) {
    if (message.payloadType == PayloadType::PLAYER_DATA && identityPeer == _identityPeer) {
        std::vector<std::string> parsedData = splitString(message.data, ",");
        float x = std::stof(parsedData[0]);
        float y = std::stof(parsedData[1]);
        _pos.x = x;
        _pos.y = y;


        _sprite.setPosition(getPosition());
    }
}
