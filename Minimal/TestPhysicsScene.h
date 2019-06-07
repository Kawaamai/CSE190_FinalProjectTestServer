#pragma once

#include "Core.h"
#include "ControllerHandler.h"
#include "oglShaderAttributes.h"
#include "Lighting.h"
#include <random>

class TestPhysicsScene {
public:
	TestPhysicsScene(Lighting light);
	~TestPhysicsScene();

	void render(const glm::mat4& projection, const glm::mat4& view);
	void chooseNewHighlightSphere();
	void resetPositions();
	void renderCube(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& toWorld);

	glm::mat4 toWorld = glm::mat4(1.0f);

private:

	int genRandNum();
	// TODO: just use model matrix instead
	void updatePosition();

	// Program
	oglplus::shapes::ShapeWrapper sphere;
	const double sphereRadius = .05;
	oglplus::Program prog;
	oglplus::VertexArray vao;
	GLuint instanceCount;
	oglplus::Buffer instances;
	oglplus::Buffer colors;
	std::vector<mat4> base_instance_positions;
	std::vector<mat4> instance_positions;
	std::vector<vec4> instance_colors;

	// cube
	oglplus::shapes::ShapeWrapper cube;
	//const double cubeSize = 4.0;
	oglplus::Program cubeProg;
	oglplus::VertexArray cubeVao;
	GLuint cubeInstanceCount = 0;
	oglplus::Buffer cubeInstances;
	oglplus::Buffer cubeColors;
	std::vector<mat4> cube_instance_positions = { glm::mat4(1.0) };
	std::vector<vec4> cube_instance_colors = { glm::vec4(238.0f / 255.0f, 127.0f / 255.0f, 27.0f / 255.0f, 1.0f) };

	glm::vec4 baseColor = vec4(0, 0.7, 0.7f, 1.0f);
	glm::vec4 highlightColor = vec4(0.8f, 0.8f, 0, 1.0f);
	int highlightedSphere = 0;

	// VBOs for the cube's vertices and normals

	const unsigned int GRID_SIZE{ 10 };
	const float gridSizeScale = 0.12f;
	//glm::mat4 orientation = glm::mat4(1.0f);
	//glm::mat4 translation = glm::mat4(1.0f);

	// random number generation
	std::random_device rd;
	Lighting sceneLight;
	bool lighting = false;
};

