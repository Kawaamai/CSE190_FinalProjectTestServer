#pragma once

#include "yojimbo.h"
#include "NetTypes.h"
#include <iostream>

const int MAX_GAME_OBJECTS = 40;

struct TransformMessageData {
	NetTransform transform;
	int int_uniqueGameObjectId = -1;

	TransformMessageData() {}

	friend std::ostream& operator<<(std::ostream& os, const TransformMessageData& data) {
		os << "GameObject: " << data.int_uniqueGameObjectId << ", "
			<< "position: ("
			<< data.transform.position.x << ", "
			<< data.transform.position.y << ", "
			<< data.transform.position.z << "), "
			<< "orientation: ("
			<< data.transform.orientation.x << ", "
			<< data.transform.orientation.y << ", "
			<< data.transform.orientation.z << ", "
			<< data.transform.orientation.w << ")";

		return os;
	}
};

class TransformMessage : public yojimbo::Message {
public:

	TransformMessageData m_data;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_float(stream, m_data.transform.position.x);
		serialize_float(stream, m_data.transform.position.y);
		serialize_float(stream, m_data.transform.position.z);
		serialize_float(stream, m_data.transform.orientation.x);
		serialize_float(stream, m_data.transform.orientation.y);
		serialize_float(stream, m_data.transform.orientation.z);
		serialize_float(stream, m_data.transform.orientation.w);
		// TODO: change min back to 0 to detect when we don't send with correct gameobject id
		//serialize_int(stream, m_data.int_uniqueGameObjectId, 0, MAX_GAME_OBJECTS);
		serialize_int(stream, m_data.int_uniqueGameObjectId, -1, MAX_GAME_OBJECTS);
		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

//typedef TransformMessage PlayerUpdateMessage;
class PlayerUpdateMessage : public yojimbo::Message {
public:
	TransformMessageData p_data, l_data, r_data;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_float(stream, p_data.transform.position.x);
		serialize_float(stream, p_data.transform.position.y);
		serialize_float(stream, p_data.transform.position.z);
		serialize_float(stream, p_data.transform.orientation.x);
		serialize_float(stream, p_data.transform.orientation.y);
		serialize_float(stream, p_data.transform.orientation.z);
		serialize_float(stream, p_data.transform.orientation.w);
		serialize_float(stream, l_data.transform.position.x);
		serialize_float(stream, l_data.transform.position.y);
		serialize_float(stream, l_data.transform.position.z);
		serialize_float(stream, l_data.transform.orientation.x);
		serialize_float(stream, l_data.transform.orientation.y);
		serialize_float(stream, l_data.transform.orientation.z);
		serialize_float(stream, l_data.transform.orientation.w);
		serialize_float(stream, r_data.transform.position.x);
		serialize_float(stream, r_data.transform.position.y);
		serialize_float(stream, r_data.transform.position.z);
		serialize_float(stream, r_data.transform.orientation.x);
		serialize_float(stream, r_data.transform.orientation.y);
		serialize_float(stream, r_data.transform.orientation.z);
		serialize_float(stream, r_data.transform.orientation.w);
		// TODO: change min back to 0 to detect when we don't send with correct gameobject id
		//serialize_int(stream, m_data.int_uniqueGameObjectId, 0, MAX_GAME_OBJECTS);
		serialize_int(stream, p_data.int_uniqueGameObjectId, -1, MAX_GAME_OBJECTS);
		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};
