#pragma once

#include "Core.h"
#include "Model.h"
#include "Shader.h"
#include <glm/gtx/transform.hpp>

class HeadModel {
private:
	std::string modelPath = "../Models/user_head.obj";
	Model m = Model(modelPath);
	//Shader shader = Shader("../Minimal/basicColor.vert", "../Minimal/basicColor.frag");
	Shader shader = Shader("../Minimal/blinnPhongColor.vert", "../Minimal/oglBlinnPhongColor.frag");
	//Shader shader = Shader("../Minimal/modelLoading.vert", "../Minimal/modelLoading.frag");

	glm::vec4 defColor = glm::vec4(1, 0, 1, 1);
	glm::mat4 scale = glm::scale(glm::vec3(.2));

public:
	void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eyePos, const glm::vec3& lightPos, const glm::mat4& toWorld) {
		shader.use();
		shader.setMat4("ProjectionMatrix", projection);
		shader.setMat4("CameraMatrix", view);
		shader.setMat4("InstanceTransform", scale);
		shader.setMat4("ModelMatrix", toWorld);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", eyePos);
		shader.setVec3("color", glm::vec3(1, 0, 0));
		m.Draw(shader);
	}
};
