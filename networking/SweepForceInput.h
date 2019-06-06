#pragma once

#include "yojimbo.h"
#include "NetTypes.h"

struct SweepForceInputMessageData {
	NetVec3 sweepDir, sweepPos;
	float sweepRadius = 0, sweepDistance = 0, sweepForce = 0;
};

class SweepForceInputMessage : public yojimbo::Message {
public:
	SweepForceInputMessageData m_data;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_float(stream, m_data.sweepDir.x);
		serialize_float(stream, m_data.sweepDir.y);
		serialize_float(stream, m_data.sweepDir.z);
		serialize_float(stream, m_data.sweepPos.x);
		serialize_float(stream, m_data.sweepPos.y);
		serialize_float(stream, m_data.sweepPos.z);
		serialize_float(stream, m_data.sweepRadius);
		serialize_float(stream, m_data.sweepDistance);
		serialize_float(stream, m_data.sweepForce);
		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};
