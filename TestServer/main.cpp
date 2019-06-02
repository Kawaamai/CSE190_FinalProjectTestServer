#include <iostream>
#include "GameServer.h"
#include "GameConstants.h"

int main() {
	std::cout << "Init server" << std::endl;

	if (!InitializeYojimbo()) {
		std::cerr << "Error: failed to initialize Yojimbo!" << std::endl;
		return 1;
	}

	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);

	GameServer server(yojimbo::Address("127.0.0.1", ServerPort));

	server.Run();

	ShutdownYojimbo();

	return 0;
}