#pragma once

#include "ClientGameAdapter.h"
#include "GameConnectionConfig.h"

class GameClient {
public:
	GameClient(const yojimbo::Address& serverAddress);
	int Run();
	int Step();
	void Update(float dt);
	bool ConnectionFailed();
	void Disconnect();

protected:
	// message sending
	virtual void SendEveryUpdate() {}

	// message processing
	void ProcessMessages();
	void ProcessMessage(yojimbo::Message* message);
	virtual void ProcessClientConnectedMessage(ClientConnectedMessage* message);
	virtual void ProcessGameTestMessage(GameTestMessage* message);
	virtual void ProcessTransformMessage(TransformMessage* message);
	virtual void ProcessRigidbodyMessage(RigidbodyMessage* message);
	virtual void ProcessPlayerUpdateMessage(PlayerUpdateMessage* message);

	GameConnectionConfig m_connectionConfig;
	ClientGameAdapter m_adapter;
	yojimbo::Client m_client;
	yojimbo::Address m_serverAddress;

	int sendMessageCounter = 0;
	double m_time = 0.0;
};

