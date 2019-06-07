#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Player {
public:
	glm::vec3 position;
	glm::quat orientation;

	Player() {}

	glm::mat4 toWorld() {
		return glm::translate(position);
	}
};
