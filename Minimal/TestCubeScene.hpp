#pragma once

#include "Core.h"
#include "OvrHelper.h"

#include "BasicShader.h"
#include "Skybox.h"
#include <vector>
#include <map>

enum SCENE_TYPE {
	STEREO_CUBE, MONO, STEREO
};
static std::map<SCENE_TYPE, SCENE_TYPE> sceneMap{
	{STEREO_CUBE, STEREO},
	{STEREO, MONO},
	{MONO, STEREO_CUBE}
};

// a class for building and rendering cubes
class TestCubeScene
{
	// Program
	std::vector<glm::mat4> instance_positions;
	GLuint instanceCount;
	GLuint shaderID;

	std::unique_ptr<TexturedCube> cube;
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<Skybox> skyboxRight;

	const float initCubeScale = 0.1f; // 20cm = 200 * .1
	const float maxCubeScale = 0.25f; // 50cm = 200 * .25
	const float minCubeScale = 0.05f; // 10cm = 200 * .05
	const float cubeScaleInc = 0.05f;
	float curCubeScale = initCubeScale;

	// frame timing
	float currentTime;
	float lastTime;

public:
	SCENE_TYPE curScene = STEREO_CUBE;

	TestCubeScene()
	{
		// Create two cube
		instance_positions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.3)));
		instance_positions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.9)));

		instanceCount = instance_positions.size();

		// Shader Program 
		shaderID = LoadShaders("skybox.vert", "skybox.frag");

		cube = std::make_unique<TexturedCube>("cube");

		// 10m wide sky box: size doesn't matter though
		skybox = std::make_unique<Skybox>("skybox");
		skybox->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
		skyboxRight = std::make_unique<Skybox>("skybox_righteye");
		skyboxRight->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
		lastTime = ovr_GetTimeInSeconds();
	}

	void render(const glm::mat4& projection, const glm::mat4& view)
	{
		// Render two cubes
		if (curScene == STEREO_CUBE) {
			for (unsigned int i = 0; i < instanceCount; i++)
			{
				// Scale to 20cm: 200cm * 0.1
				cube->toWorld = instance_positions[i] * glm::scale(glm::mat4(1.0f), glm::vec3(curCubeScale));
				cube->draw(shaderID, projection, view);
			}
		}

		// Render Skybox : remove view translation
		skybox->draw(shaderID, projection, view);
	}

	void render(const glm::mat4& projection, const glm::mat4& view, ovrEyeType eye)
	{
		// Render two cubes
		if (curScene == STEREO_CUBE) {
			for (unsigned int i = 0; i < instanceCount; i++)
			{
				// Scale to 20cm: 200cm * 0.1
				cube->toWorld = instance_positions[i] * glm::scale(glm::mat4(1.0f), glm::vec3(curCubeScale));
				cube->draw(shaderID, projection, view);
			}
		}

		// Render Skybox : remove view translation
		if (curScene == MONO || eye == ovrEye_Left)
			skybox->draw(shaderID, projection, view);
		else
			skyboxRight->draw(shaderID, projection, view);
	}

	void increaseCubeScale() {
		curCubeScale += cubeScaleInc * getTimeDelta();
		if (curCubeScale > maxCubeScale)
			curCubeScale = maxCubeScale;
	}

	void decreaseCubeScale() {
		curCubeScale -= cubeScaleInc * getTimeDelta();
		if (curCubeScale < minCubeScale)
			curCubeScale = minCubeScale;
	}

	void resetCubeScale() {
		curCubeScale = initCubeScale;
	}

	float getTimeDelta() {
		return currentTime - lastTime;
	}

	void updateTime() {
		lastTime = currentTime;
		currentTime = ovr_GetTimeInSeconds();
	}
};