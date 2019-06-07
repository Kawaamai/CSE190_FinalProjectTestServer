#pragma once

#include "GameMessageType.h"
#include "GameTestMessage.h"
#include "TransformMessage.h"
#include "SweepForceInput.h"
#include "RigidbodyMessage.h"

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST, GameTestMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TRANSFORM_INFO, TransformMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::RIGIDBODY_INFO, RigidbodyMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PLAYER_UPDATE, PlayerUpdateMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SWEEP_FORCE_INPUT, SweepForceInputMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();
