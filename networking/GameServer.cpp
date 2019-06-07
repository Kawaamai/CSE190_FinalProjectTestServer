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
	//m_scene.SetRunning(m_running);
	m_scene.InitScene();
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

	m_scene.Cleanup();
	m_server.Stop();
}

void GameServer::PhysicsRun() {
	//if (m_running)
	//	m_scene.Run();
	while (m_running) {
		m_scene.Update();
	}

	m_scene.Cleanup();
}

//static unsigned int i = 0;

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

	m_scene.Update(); // run physics sim

	// ... process client inputs ...
	// ... update game ...
	// ... send game state to clietns ...
	//m_scene.UpdateSnapshot(); // done in physic's update
	SendSceneSnapshot();

	m_server.SendPackets();
}

void GameServer::SendSceneSnapshot() {
	m_scene.ForEachActor([&](int actorId, PxRigidActor const * const actor) {
		PxRigidBody const * const rb = reinterpret_cast<PxRigidBody const * const>(actor);
		PxTransform tm = actor->getGlobalPose();
		PxVec3 linVel = rb->getLinearVelocity();
		PxVec3 angVel = rb->getAngularVelocity();

		//NetTransform data(NetVec3(tm.p.x, tm.p.y, tm.p.z), NetQuat(tm.q.w, tm.q.x, tm.q.y, tm.q.z));
		NetTransform data;
		converter::PhysXTmToNetTm(tm, data);
		NetVec3 linVelData, angVelData;
		converter::PhysXVec3ToNetVec3(linVel, linVelData);
		converter::PhysXVec3ToNetVec3(angVel, angVelData);
		//if (actorId == 1)
		//	std::cerr << "sentSnapshot " << actor->getGlobalPose().p.x << std::endl;

		// message setup	
		ForEachConnectedClient([&](int clientIdx) {
			//TransformMessage* message = (TransformMessage*)m_server.CreateMessage(clientIdx, (int)GameMessageType::TRANSFORM_INFO);
			RigidbodyMessage* message = (RigidbodyMessage*)m_server.CreateMessage(clientIdx, (int)GameMessageType::RIGIDBODY_INFO);
			message->m_data.transform = data;
			message->m_data.int_uniqueGameObjectId = actorId;
			m_server.SendMessage(clientIdx, (int)GameChannel::UNRELIABLE, message);
		});
	});
}

void GameServer::SendActorMessage() {
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
	case (int)GameMessageType::SWEEP_FORCE_INPUT:
		ProcessSweepForceInputMessage(clientIndex, (SweepForceInputMessage*)message);
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

void GameServer::ProcessSweepForceInputMessage(int clientIndex, SweepForceInputMessage * message) {
	std::cerr << "process sweep force input message" << std::endl;
	m_scene.ProcessSweepForceInputMessage(message->m_data);
}

void GameServer::ForEachConnectedClient(std::function<void(int)> f) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (m_server.IsClientConnected(i)) {
			f(i);
		}
	}
}
