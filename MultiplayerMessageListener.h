#ifndef _MULTIPLAYER_MESSAGE_LISTENER_H
#define _MULTIPLAYER_MESSAGE_LISTENER_H

#include <string>
#include "..//SteamworksHeaders/steam_api.h"

enum class PayloadType {
    JOIN_REQUEST,
    JOIN_REJECT,
    PEER_DISCONNECT,
    PLAYER_DATA,
    PLAYER_AIM_ANGLE,
    WORLD_SEED,
    INVENTORY_DATA
};

struct MultiplayerMessage {
    MultiplayerMessage(PayloadType payloadType, std::string data) {
        this->payloadType = payloadType;
        this->data = data;
    }

    PayloadType payloadType;
    std::string data;
};

class MultiplayerMessageListener {
public:
    bool _doNotRemove = false;
    virtual void messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer) = 0;
};

#endif