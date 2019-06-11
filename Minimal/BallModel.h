#pragma once

#include "Core.h"
#include "Model.h"
#include "Shader.h"
#include <glm/gtx/transform.hpp>
#include "ShadowRenderingHeader.h"

class BallModel {
private:
	std::string modelPath = "../Models/ico_ball.obj";
	Model m = Model(modelPath);
	//Shader shader = Shader("../Minimal/basicColor.vert", "../Minimal/basicColor.frag");
	Shader shader = Shader("../Minimal/blinnPhongColor.vert", "../Minimal/oglBlinnPhongColor.frag");
	//Shader shader = Shader("../Minimal/modelLoading.vert", "../Minimal/modelLoading.frag");
	Shader shadowShader = Shader("../Minimal/simpleDepthMapShader.vert", "../Minimal/simpleDepthMapShader.frag");

	glm::vec4 defColor = glm::vec4(1, 0, 1, 1);
	//glm::mat4 scale = glm::scale(glm::vec3(.1));
	glm::mat4 scale = glm::scale(glm::vec3(.3));

public:
	void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eyePos, const glm::vec3& lightPos, const glm::mat4& toWorld) {
		shader.use();
		shader.setMat4("ProjectionMatrix", projection);
		shader.setMat4("CameraMatrix", view);
		shader.setMat4("InstanceTransform", scale);
		shader.setMat4("ModelMatrix", toWorld);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", eyePos);
		shader.setVec3("color", glm::vec3(1));
		shader.setMat4("LightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		m.Draw(shader);
	}

	void shadowRender(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& toWorld) {
		shadowShader.use();
		shadowShader.setMat4("ProjectionMatrix", projection);
		shadowShader.setMat4("CameraMatrix", view);
		shadowShader.setMat4("InstanceTransform", scale);
		shadowShader.setMat4("ModelMatrix", toWorld);
		m.Draw(shadowShader);
	}
};
