#include "GameServer.h"
#include "yojimbo.h"
#include "GameConstants.h"
#include <iostream>
#include <string>

GameServer::GameServer(const yojimbo::Address& address) :
	m_adapter(this),
	m_server(yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY, address, m_connectionConfig, m_adapter, 0.0)
{
	// start the server
	m_server.Start(MAX_PLAYERS);
	if (!m_server.IsRunning()) {
		throw std::runtime_error("Could not start server at port " + std::to_string(address.GetPort()));
	} else {
		m_running = true;
	}

	// print the port we got in case we used port 0
	char buffer[256];
	m_server.GetAddress().ToString(buffer, sizeof(buffer));
	std::cout << "Server address is " << buffer << std::endl;

	// ... load game ...
}

void GameServer::Run() {
	while (m_running) {
		double currentTime = yojimbo_time();
		if (m_time <= currentTime) {
			Update(FIXED_DT);
			m_time += FIXED_DT;
		} else {
			yojimbo_sleep(m_time - currentTime);
		}
	}

	m_server.Stop();
}

void GameServer::Update(float dt) {
	// stop if server is not running
	if (!m_server.IsRunning()) {
		m_running = false;
		return;
	}

	// update server and process messages
	m_server.AdvanceTime(m_time);
	m_server.ReceivePackets();
	ProcessMessages();

	// ... process client inputs ...
	// ... update game ...
	// ... send game state to clietns ...

	m_server.SendPackets();
}

void GameServer::Stop() {
	m_server.Stop();
}

void GameServer::ClientConnected(int clientIndex) {
	std::cout << "client " << clientIndex << " connected" << std::endl;
}

void GameServer::ClientDisconnected(int clientIndex) {
	std::cout << "client " << clientIndex << " disconnected" << std::endl;
}

void GameServer::ProcessMessages() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (m_server.IsClientConnected(i)) {
			for (int j = 0; j < m_connectionConfig.numChannels; j++) {
				yojimbo::Message* message = m_server.ReceiveMessage(i, j);
				while (message != NULL) {
					ProcessMessage(i, message);
					m_server.ReleaseMessage(i, message);
					message = m_server.ReceiveMessage(i, j);
				}
			}
		}
	}
}

void GameServer::ProcessMessage(int clientIndex, yojimbo::Message * message) {
	switch (message->GetType()) {
	case (int)GameMessageType::TEST:
		ProcessGameTestMessage(clientIndex, (GameTestMessage*)message);
		break;
	case (int)GameMessageType::TRANSFORM_INFO:
		ProcessTransformMessage(clientIndex, (TransformMessage*)message);
		break;
	default:
		break;
	}
}

void GameServer::ProcessGameTestMessage(int clientIndex, GameTestMessage * message) {
	std::cout << "Test message received from client " << clientIndex << " with data " << message->m_data << std::endl;
	GameTestMessage* testMessage = (GameTestMessage*)m_server.CreateMessage(clientIndex, (int)GameMessageType::TEST);
	testMessage->m_data = (int)message->m_data + 1;
	m_server.SendMessage(clientIndex, (int)GameChannel::RELIABLE, testMessage);
}

void GameServer::ProcessTransformMessage(int clientIndex, TransformMessage * message) {
	std::cout << "transform message received from client " << clientIndex << " with data " << message->m_data << std::endl;
	TransformMessage* testMessage = (TransformMessage*)m_server.CreateMessage(clientIndex, (int)GameMessageType::TRANSFORM_INFO);
	m_server.SendMessage(clientIndex, (int)GameChannel::UNRELIABLE, testMessage);

}
