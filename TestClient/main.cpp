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
	//yojimbo_log_level(YOJIMBO_LOG_LEVEL_DEBUG);

	int result;
	// need this to properly call client destructor
	// may just make this a std::unique_ptr, then call unique_ptr::reset()
	{
		GameClient client(yojimbo::Address("127.0.0.1", ServerPort));
		result = client.Run();
		std::cerr << "disconnecting" << std::endl;
		client.Disconnect();
	}
	std::cerr << "shutdown yojimbo" << std::endl;
	ShutdownYojimbo();
	std::cerr << "finished" << std::endl;

	return result;
}