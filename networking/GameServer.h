#pragma once

#include "yojimbo.h"
#include "GameAdapter.h"
#include "GameConnectionConfig.h"
#include "MainGameScene.h"
#include "GameMessageFactory.h"
#include <functional>

class GameServer {

public:
	GameServer(const yojimbo::Address& address);

	void SendScores(int p1, int p2);
	void Run();
	void PhysicsRun();
	void Update(float dt);
	void Stop();
	void ClientConnected(int clientIndex);
	void ClientDisconnected(int clientIndex);

	bool isRunning() { return m_server.IsRunning(); }
	yojimbo::Address GetAddress() { return m_server.GetAddress(); }

private:

	// sending messages
	void SendSceneSnapshot();
	void SendPlayerPositions();

	// process messages
	void ProcessMessages();
	void ProcessMessage(int clientIndex, yojimbo::Message* message);
	void ProcessGameTestMessage(int clientIndex, GameTestMessage* message);
	virtual void ProcessTransformMessage(int clientIndex, TransformMessage* message);
	virtual void ProcessPlayerUpdateMessage(int clientIndex, PlayerUpdateMessage* message);
	virtual void ProcessSweepForceInputMessage(int clientIndex, SweepForceInputMessage* message);

	void ForEachConnectedClient(std::function<void(int)> f);

	GameConnectionConfig m_connectionConfig;
	GameAdapter m_adapter;
	yojimbo::Server m_server;
	MainGameScene m_scene;
	double m_time = 0.0;
	bool m_running = false;
};
