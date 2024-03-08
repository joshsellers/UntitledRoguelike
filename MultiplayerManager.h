#ifndef _MULTIPLAYER_MANAGER_H
#define _MULTIPLAYER_MANAGER_H

#include "..//SteamworksHeaders/steam_api.h"
#include "MessageManager.h"

enum class PayloadType {

};

struct MultiplayerMessage {

};

class MultiplayerManager {
public:
    void sendMessage(std::string message, const SteamNetworkingIdentity& identityRemote) {
        EResult result = SteamNetworkingMessages()->SendMessageToUser(identityRemote, message.c_str(), message.length(), k_nSteamNetworkingSend_Reliable, 0);
    }

    void recieveMessages() const {
        //while (!_halted) {
        // I think we basically have to ensure that this thread is killed before we shut down 
        // Probably better to just not do it in a thread and instead give it an update function called from Game.update
        //sf::sleep(sf::milliseconds(5));
        if (!_halted) {
            SteamNetworkingMessage_t* ppOutMessages = {};
            int messageCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, &ppOutMessages, 1);
            if (messageCount > 0) {
                std::string data = (const char*)(*ppOutMessages).GetData();
                auto userData = ppOutMessages->m_identityPeer;
                auto steamId = userData.GetSteamID();
                std::string userName = SteamFriends()->GetFriendPersonaName(steamId);
                MessageManager::displayMessage("Message in from " + userName + ": " + data, 10, DEBUG);

                ppOutMessages->Release();
            }
        }
        //}
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