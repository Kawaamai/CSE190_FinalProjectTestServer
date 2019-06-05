#include <iostream>
#include <thread>
#include "GameServer.h"
#include "GameConstants.h"
#include "PhysXServerApp.h"

int main() {
	std::cout << "Init server" << std::endl;

	if (!InitializeYojimbo()) {
		std::cerr << "Error: failed to initialize Yojimbo!" << std::endl;
		return 1;
	}

	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);

	GameServer server(yojimbo::Address("127.0.0.1", ServerPort));

	std::cout << "starting server thread" << std::endl;
	std::thread networkThread(&GameServer::Run, &server);

	std::cout << "starting game physics thread" << std::endl;
	std::thread physicsThread(&GameServer::PhysicsRun, &server);

	networkThread.join();
	physicsThread.join();

	ShutdownYojimbo();

	return 0;
}