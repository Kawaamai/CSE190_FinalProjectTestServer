#pragma once

#include "Core.h"
#include "Model.h"
#include "Shader.h"
#include <glm/gtx/transform.hpp>

class HandModel {
private:
	std::string modelPath = "../Models/user_hand.obj";
	Model m = Model(modelPath);
	//Shader shader = Shader("../Minimal/basicColor.vert", "../Minimal/basicColor.frag");
	Shader shader = Shader("../Minimal/blinnPhongColor.vert", "../Minimal/oglBlinnPhongColor.frag");
	//Shader shader = Shader("../Minimal/modelLoading.vert", "../Minimal/modelLoading.frag");

	glm::vec4 defColor = glm::vec4(1, 0, 1, 1);
	glm::mat4 scale = glm::scale(glm::vec3(.1));
	glm::mat4 lhandLocal = glm::translate(glm::vec3(0, -0.03, .08)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, -1)) * glm::rotate(glm::radians(91.0f), glm::vec3(-1, 0, 0));
	glm::mat4 rhandLocal = glm::translate(glm::vec3(0, -0.02, .08)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));

public:
	void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eyePos, const glm::vec3& lightPos, const glm::mat4& toWorld, ovrHandType hand) {
		glm::mat4 rot;
		if (hand == ovrHand_Left)
			rot = lhandLocal;
		else
			rot = rhandLocal;
		shader.use();
		shader.setMat4("ProjectionMatrix", projection);
		shader.setMat4("CameraMatrix", view);
		shader.setMat4("InstanceTransform", rot * scale);
		shader.setMat4("ModelMatrix", toWorld);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", eyePos);
		shader.setVec3("color", glm::vec3(1, 0, 0));
		m.Draw(shader);
	}
};
