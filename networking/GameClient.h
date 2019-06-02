#pragma once

#include "GameAdapter.h"
#include "GameConnectionConfig.h"

class GameClient {
public:
	GameClient(const yojimbo::Address& serverAddress);
	int Run();
	void Update(float dt);
	bool ConnectionFailed();

private:
	void ProcessMessages();
	void ProcessMessage(yojimbo::Message* message);
	void ProcessGameTestMessage(GameTestMessage* message);
	void ProcessTransformMessage(TransformMessage* message);

	GameConnectionConfig m_connectionConfig;
	GameAdapter m_adapter;
	yojimbo::Client m_client;
	yojimbo::Address m_serverAddress;

	int sendMessageCounter = 0;
	double m_time = 0.0;
};

