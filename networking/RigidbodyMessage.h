#pragma once

#include "yojimbo.h"
#include "NetTypes.h"

struct RigidbodyMessageData {
	NetTransform transform;
	NetVec3 linVel, angVel;
	int int_uniqueGameObjectId = -1;
};

class RigidbodyMessage : public yojimbo::Message {
public:
	RigidbodyMessageData m_data;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_float(stream, m_data.transform.position.x);
		serialize_float(stream, m_data.transform.position.y);
		serialize_float(stream, m_data.transform.position.z);
		serialize_float(stream, m_data.transform.orientation.x);
		serialize_float(stream, m_data.transform.orientation.y);
		serialize_float(stream, m_data.transform.orientation.z);
		serialize_float(stream, m_data.transform.orientation.w);
		serialize_float(stream, m_data.linVel.x);
		serialize_float(stream, m_data.linVel.y);
		serialize_float(stream, m_data.linVel.z);
		serialize_float(stream, m_data.angVel.x);
		serialize_float(stream, m_data.angVel.y);
		serialize_float(stream, m_data.angVel.z);
		// TODO: change min back to 0 to detect when we don't send with correct gameobject id
		//serialize_int(stream, m_data.int_uniqueGameObjectId, 0, MAX_GAME_OBJECTS);
		serialize_int(stream, m_data.int_uniqueGameObjectId, -1, MAX_GAME_OBJECTS);
		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};
