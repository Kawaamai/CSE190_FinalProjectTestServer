#include "GameAdapter.h"
#include "GameServer.h"

void GameAdapter::OnServerClientConnected(int clientIndex) {
	if (m_server != NULL) {
		m_server->ClientConnected(clientIndex);
	}
}

void GameAdapter::OnServerClientDisconnected(int clientIndex) {
	if (m_server != NULL) {
		m_server->ClientDisconnected(clientIndex);
	}
}