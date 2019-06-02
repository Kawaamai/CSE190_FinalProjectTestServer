#include "GameClient.h"
#include "GameConstants.h"
#include <iostream>
#include <windows.h>
#ifdef _WINUSER_
#undef SendMessage
#endif

static volatile int quit = 0;

BOOL WINAPI consoleHandler(DWORD signal) {
	printf("something here\n");
	if (signal == CTRL_C_EVENT) {
		printf("something else here\n");
		quit = 1;
	}

	return TRUE;
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
		double currentTime = yojimbo_time();

		if (m_client.ConnectionFailed())
			break;

		if (m_time <= currentTime) {
			Update(FIXED_DT);
			m_time += FIXED_DT;
		} else {
			yojimbo_sleep(m_time - currentTime);
		}
	} 

	std::cerr << "disconnecting" << std::endl;
	m_client.Disconnect();

	return 0;
}

void GameClient::Update(float dt) {
	// update client
	m_client.AdvanceTime(m_client.GetTime() + dt);
	m_client.ReceivePackets();

	if (m_client.IsConnected()) {
		ProcessMessages();

		// ... do connected stuff ...

		// send a message when space is pressed
		if (sendMessageCounter == 0) {
			//GameTestMessage* message = (GameTestMessage*)m_client.CreateMessage((int)GameMessageType::TEST);
			//message->m_data = 42;
			//m_client.SendMessage((int)GameChannel::RELIABLE, message);
			TransformMessage* message = (TransformMessage*)m_client.CreateMessage((int)GameMessageType::TRANSFORM_INFO);
			NetTransform data(NetVec3(1.0f, 2.0f, 3.0f), NetQuat(4.0f, 5.0f, 6.0f, 7.0f));
			//message->m_data.transform.position = NetVec3(1.0f, 2.0f, 3.0f);
			//message->m_data.transform.orientation = NetQuat(4.0f, 5.0f, 6.0f, 7.0f);
			message->m_data.transform = data;
			message->m_data.int_uniqueGameObjectId = 3;
			m_client.SendMessage((int)GameChannel::UNRELIABLE, message);
		}

		sendMessageCounter = (sendMessageCounter + 1) % 20;

		m_client.SendPackets();
	}
}

bool GameClient::ConnectionFailed() {
	return m_client.ConnectionFailed();
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
    case (int)GameMessageType::TEST:
        ProcessGameTestMessage((GameTestMessage*)message);
        break;
	case (int)GameMessageType::TRANSFORM_INFO:
		ProcessTransformMessage((TransformMessage*)message);
		break;
    default:
        break;
    }
}

void GameClient::ProcessGameTestMessage(GameTestMessage * message) {
	std::cout << "Test message received from server with data " << message->m_data << std::endl;
}

void GameClient::ProcessTransformMessage(TransformMessage * message) {
	std::cout << "Transform message received from server with data " << message->m_data << std::endl;
}

