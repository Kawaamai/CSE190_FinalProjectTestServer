#pragma once

// a simple test message
enum class GameMessageType {
	CLIENT_CONNECTED,
	TRANSFORM_INFO,
	RIGIDBODY_INFO,
	PLAYER_UPDATE,
	SWEEP_FORCE_INPUT,
    TEST,
    COUNT
};
