#pragma once

struct NetVec3 {
public:
	float x, y, z = 0.0f;

	NetVec3() {}
	NetVec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct NetQuat {
public:
	float w = 1.0f, x = 0.0f, y = 0.0f, z = 0.0f;

	NetQuat() {}
	NetQuat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
};

struct NetTransform {
public:
	NetVec3 position;
	NetQuat orientation;

	NetTransform() {}
	NetTransform(NetVec3 p, NetQuat o) : position(p), orientation(o) {}
};
