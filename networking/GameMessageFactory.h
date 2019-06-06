#pragma once

#include "GameMessageType.h"
#include "GameTestMessage.h"
#include "TransformMessage.h"
#include "SweepForceInput.h"

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST, GameTestMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TRANSFORM_INFO, TransformMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SWEEP_FORCE_INPUT, SweepForceInputMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();
