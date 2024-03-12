#ifndef _REMOTE_PLAYER_H
#define _REMOTE_PLAYER_H

#include "Player.h"
#include "MultiplayerMessageListener.h"

class RemotePlayer : public Player, public MultiplayerMessageListener {
public:
    RemotePlayer(SteamNetworkingIdentity identityPeer, sf::Vector2f pos, sf::RenderWindow* window, bool& gamePaused);

    void update();

    void messageReceived(MultiplayerMessage message, SteamNetworkingIdentity identityPeer);

    void damage(int damage);
private:
    SteamNetworkingIdentity _identityPeer;

    long long _lastUpdateTime = currentTimeMillis();

    TERRAIN_TYPE getCurrentTerrain();
};

#endif

