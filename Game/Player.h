#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Player {
public:
	glm::vec3 position;
	glm::quat orientation;
	glm::quat baseOrientation;

	Player() {}

	glm::mat4 toWorld() {
		return glm::translate(position) * glm::mat4_cast(baseOrientation);
	}
};

class PlayerServer : public Player {
public:
	glm::vec3 lhandPosition, rhandPosition;
	glm::quat lhandOrientation, rhandOrientation;
};
