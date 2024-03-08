#ifndef _MULTIPLAYER_MANAGER_H
#define _MULTIPLAYER_MANAGER_H

#include "..//SteamworksHeaders/steam_api.h"
#include "MessageManager.h"

enum class PayloadType {
    PLAYER_DATA,
    WORLD_SEED
};

struct MultiplayerMessage {
    MultiplayerMessage(PayloadType payloadType, std::string data) {
        this->payloadType = payloadType;
        this->data = data;
    }

    PayloadType payloadType;
    std::string data;
};

class MultiplayerManager {
public:
    void sendMessage(MultiplayerMessage message, const SteamNetworkingIdentity& identityRemote) {
        EResult result = SteamNetworkingMessages()->SendMessageToUser(identityRemote, &message, sizeof(message), k_nSteamNetworkingSend_Reliable, 0);
    }

    void recieveMessages() {
        if (!_halted) {
            SteamNetworkingMessage_t* ppOutMessages = {};
            int messageCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, &ppOutMessages, 1);
            if (messageCount > 0) {
                MultiplayerMessage message = *(MultiplayerMessage*)(*ppOutMessages).GetData();
                PayloadType payloadType = message.payloadType;

                auto userData = ppOutMessages->m_identityPeer;
                auto steamId = userData.GetSteamID();
                std::string userName = SteamFriends()->GetFriendPersonaName(steamId);
                MessageManager::displayMessage("Message in from " + userName + "(" + std::to_string((int)payloadType) + ") " + message.data, 5, DEBUG);

                ppOutMessages->Release();
            }
        }
    }

    void halt() {
        _halted = true;
    }

    void resume() {
        _halted = false;
    }

    bool isHalted() {
        return _halted;
    }
private:
    STEAM_CALLBACK(MultiplayerManager, onSessionRequest, SteamNetworkingMessagesSessionRequest_t);
    STEAM_CALLBACK(MultiplayerManager, onSessionFail, SteamNetworkingMessagesSessionFailed_t);

    bool _halted = false;
};

inline void MultiplayerManager::onSessionRequest(SteamNetworkingMessagesSessionRequest_t* pCallback) {
    SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
    MessageManager::displayMessage("Accepted session request with " + std::to_string(pCallback->m_identityRemote.GetSteamID().GetAccountID()), 5, DEBUG);
}

inline void MultiplayerManager::onSessionFail(SteamNetworkingMessagesSessionFailed_t* pCallback) {
    MessageManager::displayMessage("Session failed", 10, WARN);
}

class Multiplayer {
public:
    inline static MultiplayerManager messenger;
private:
};
#endif