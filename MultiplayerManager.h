#ifndef _MULTIPLAYER_MANAGER_H
#define _MULTIPLAYER_MANAGER_H

#include "MessageManager.h"
#include "MultiplayerMessageListener.h"

class MultiplayerManager {
public:
    void sendMessage(MultiplayerMessage message, const SteamNetworkingIdentity& identityRemote) {
        const int flag = k_nSteamNetworkingSend_Reliable;
        std::string buffer = std::to_string((int)message.payloadType) + (std::string)":" + message.data;
        EResult result = SteamNetworkingMessages()->SendMessageToUser(identityRemote, buffer.c_str(), buffer.length() + 1, flag, 0);
        if (result != k_EResultOK) MessageManager::displayMessage("Multiplayer error @ sendMessage: " + std::to_string((int)result), 5, ERR);
    }

    void recieveMessages() {
        if (!_halted) {
            SteamNetworkingMessage_t* ppOutMessages = {};
            int messageCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, &ppOutMessages, 1);
            if (messageCount > 0) {
                std::string message = (const char*)(*ppOutMessages).GetData();
                std::vector<std::string> parsedMessage = splitString(message, ":");
                PayloadType payloadType = (PayloadType)stoi(parsedMessage[0]);
                std::string data = parsedMessage[1];

                auto userData = ppOutMessages->m_identityPeer;
                auto steamId = userData.GetSteamID();
                std::string userName = SteamFriends()->GetFriendPersonaName(steamId);

                for (auto& listener : _listeners) {
                    listener->messageReceived(MultiplayerMessage(payloadType, data), userData);
                }

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

    void addListener(std::shared_ptr<MultiplayerMessageListener> listener) {
        _listeners.push_back(listener);
    }

    void clearListeners() {
        for (int i = 0; i < _listeners.size(); i++) {
            auto& listener = _listeners[i];
            if (!listener->_doNotRemove) {
                _listeners.erase(_listeners.begin() + i);
            }
        }
    }

    std::vector<SteamNetworkingIdentity> getConnectedPeers() const {
        return _connectedPeers;
    }

    void addConnectedPeer(SteamNetworkingIdentity connectedPeer) {
        _connectedPeers.push_back(connectedPeer);
    }

    void clearPeers() {
        _connectedPeers.clear();
    }
private:
    STEAM_CALLBACK(MultiplayerManager, onSessionRequest, SteamNetworkingMessagesSessionRequest_t);
    STEAM_CALLBACK(MultiplayerManager, onSessionFail, SteamNetworkingMessagesSessionFailed_t);

    bool _halted = false;

    std::vector<std::shared_ptr<MultiplayerMessageListener>> _listeners;

    std::vector<SteamNetworkingIdentity> _connectedPeers;
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
    inline static MultiplayerManager manager;

    static std::string getPeerSteamName(SteamNetworkingIdentity identityPeer) {
        return SteamFriends()->GetFriendPersonaName(identityPeer.GetSteamID());
    }
private:
};
#endif