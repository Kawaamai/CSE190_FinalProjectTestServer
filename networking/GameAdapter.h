#pragma once

#include "yojimbo.h"
#include "GameMessageFactory.h"
class GameServer;

class GameAdapter : public yojimbo::Adapter {
public:
    explicit GameAdapter(GameServer* server = NULL) :
		m_server(server) {}

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
    
    void OnServerClientConnected(int clientIndex) override;
    void OnServerClientDisconnected(int clientIndex) override;
    
private:
    GameServer* m_server;
};
