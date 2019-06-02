#include <iostream>
#include "GameClient.h"
#include "GameConstants.h"

int main() {
	std::cout << "Init client" << std::endl;

	if (!InitializeYojimbo()) {
		std::cerr << "Error: failed to initialize Yojimbo!" << std::endl;
		return 1;
	}

	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);

	GameClient client(yojimbo::Address("127.0.0.1", ServerPort));
	int result = client.Run();

	ShutdownYojimbo();

	return result;
}