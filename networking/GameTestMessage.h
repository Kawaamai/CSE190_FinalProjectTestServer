#pragma once

#include "yojimbo.h"

class GameTestMessage : public yojimbo::Message {
public:
    int m_data;

    GameTestMessage() :
        m_data(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, m_data, 0, 512);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

typedef GameTestMessage ClientConnectedMessage;

class UpdateScoreMessage : public yojimbo::Message {
public:
	int p1Score, p2Score;

	UpdateScoreMessage() : p1Score(0), p2Score(0) {}
	UpdateScoreMessage(int p1, int p2) :
		p1Score(p1), p2Score(p2) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, p1Score, 0, 512);
        serialize_int(stream, p2Score, 0, 512);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};
