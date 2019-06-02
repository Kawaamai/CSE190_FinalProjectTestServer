#pragma once

#include "yojimbo.h"
#include "GameAdapter.h"
#include "GameConnectionConfig.h"

class GameServer {

public:
	GameServer(const yojimbo::Address& address);
	void Run();
	void Update(float dt);
	void Stop();
	void ClientConnected(int clientIndex);
	void ClientDisconnected(int clientIndex);

	bool isRunning() { return m_server.IsRunning(); }
	yojimbo::Address GetAddress() { return m_server.GetAddress(); }

private:
	void ProcessMessages();
	void ProcessMessage(int clientIndex, yojimbo::Message* message);
	void ProcessGameTestMessage(int clientIndex, GameTestMessage* message);
	void ProcessTransformMessage(int clientIndex, TransformMessage* message);

	GameConnectionConfig m_connectionConfig;
	GameAdapter m_adapter;
	yojimbo::Server m_server;
	double m_time = 0.0;
	bool m_running = false;
};
