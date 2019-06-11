#include "GameClient.h"
#include "GameConstants.h"
#include <iostream>
#include <windows.h>
#ifdef _WINUSER_
#undef SendMessage // so that we can use yojimbo's SendMessage
#endif

#define GAME_DT 1.0f / 90.0f

static volatile int quit = 0;

BOOL WINAPI consoleHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT) {
		printf("ctrl-c handled\n");
		quit = 1;
		return TRUE;
	}
	
	return FALSE;
}

GameClient::GameClient(const yojimbo::Address& serverAddress) :
	m_serverAddress(serverAddress),
	m_client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), m_connectionConfig, m_adapter, 0.0)
{
	uint64_t clientId = 0;
	yojimbo::random_bytes((uint8_t*)&clientId, 8);
	m_client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, m_serverAddress);
}

int GameClient::Run() {
	// disconnect on ctrl-c
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		std::cerr << "error: could not set control handler" << std::endl;
		return 1;
	}

	while (!quit) {
		if (Step())
			break;
	} 

	return 0;
}

int GameClient::Step() {
	double currentTime = yojimbo_time();

	if (m_client.ConnectionFailed())
		return 1;

	if (m_time <= currentTime) {
		//Update(FIXED_DT);
		//m_time += FIXED_DT;
		Update(GAME_DT);
		m_time += GAME_DT;
	} else {
		//yojimbo_sleep(m_time - currentTime);
		// simulate sleep
	}

	return 0;
}

void GameClient::Update(float dt) {
	// update client
	m_client.AdvanceTime(m_client.GetTime() + dt);
	m_client.ReceivePackets();

	if (m_client.IsConnected()) {
		ProcessMessages();
		SendEveryUpdate();
	}

	m_client.SendPackets();
}

bool GameClient::ConnectionFailed() {
	return m_client.ConnectionFailed();
}

void GameClient::Disconnect() {
	if (m_client.IsConnected())
		m_client.Disconnect();
}

void GameClient::ProcessMessages() {
	for (int i = 0; i < m_connectionConfig.numChannels; i++) {
		yojimbo::Message* message = m_client.ReceiveMessage(i);
		while (message != NULL) {
			ProcessMessage(message);
			m_client.ReleaseMessage(message);
			message = m_client.ReceiveMessage(i);
		}
	}
}

void GameClient::ProcessMessage(yojimbo::Message * message) {
    switch (message->GetType()) {
	case (int)GameMessageType::CLIENT_CONNECTED:
		ProcessClientConnectedMessage((ClientConnectedMessage*) message);
		break;
    case (int)GameMessageType::TEST:
        ProcessGameTestMessage((GameTestMessage*)message);
        break;
	case (int)GameMessageType::TRANSFORM_INFO:
		ProcessTransformMessage((TransformMessage*)message);
		break;
	case (int)GameMessageType::RIGIDBODY_INFO:
		ProcessRigidbodyMessage((RigidbodyMessage*)message);
	case (int)GameMessageType::PLAYER_UPDATE:
		ProcessPlayerUpdateMessage((PlayerUpdateMessage*)message);
    default:
        break;
    }
}

void GameClient::ProcessClientConnectedMessage(ClientConnectedMessage * message) {}

void GameClient::ProcessGameTestMessage(GameTestMessage * message) {
	std::cout << "Test message received from server with data\n" << message->m_data << std::endl;
}

void GameClient::ProcessTransformMessage(TransformMessage * message) {
	//std::cout << "Transform message received from server with data:\n" << message->m_data << std::endl;
}

void GameClient::ProcessRigidbodyMessage(RigidbodyMessage * message) {}

void GameClient::ProcessPlayerUpdateMessage(PlayerUpdateMessage * message) {}

