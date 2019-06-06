#pragma once

#include "Core.h"
#include "ControllerHandler.h"
#include "oglShaderAttributes.h"
#include "Lighting.h"
#include <random>

class SphereScene
{
public:

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

	glm::vec4 baseColor = vec4(0, 0.7, 0.7f, 1.0f);
	glm::vec4 highlightColor = vec4(0.8f, 0.8f, 0, 1.0f);
	int highlightedSphere = 0;

	// VBOs for the cube's vertices and normals

	const unsigned int GRID_SIZE{ 5 };
	//const float gridSizeScale = 0.14f;
	const float gridSizeScale = 0.12f;
	//glm::mat4 orientation = glm::mat4(1.0f);
	//glm::mat4 translation = glm::mat4(1.0f);
	glm::mat4 toWorld = glm::mat4(1.0f);

	// random number generation
	std::random_device rd;
	Lighting sceneLight;
	bool lighting = false;

	SphereScene();
	SphereScene(Lighting light);
	~SphereScene();

	void chooseNewHighlightSphere();
	void render(const mat4 & projection, const mat4 & view);
	void resetPositions();

private:

	int genRandNum();
	// TODO: just use model matrix instead
	void updatePosition();
};

/*
struct SphereScene {
	// Program
	std::string path = "sphere2.obj";
	// model
	// TODO: make this into a class so we can properly instantiate this
	Model m = Model(path);
	//Shader shader = Shader("basicVertex.vs", "basicFragment.fs");
	Shader shader = Shader("basicColor.vert", "basicColor.frag");

	// colors
	vec4 defaultColor = vec4(0, 0, 0.3, 1.0);
	vec4 highlightColor = vec4(.9, 0, 0, 1.0);

	// other
	std::vector<mat4> instance_positions;
	GLuint instanceCount;

	const unsigned int GRID_SIZE{ 5 };
	const float sizeScale = 0.2f;

public:
	SphereScene() {
		// Create a cube of cubes
		{
			for (unsigned int z = 0; z < GRID_SIZE; ++z) {
				for (unsigned int y = 0; y < GRID_SIZE; ++y) {
					for (unsigned int x = 0; x < GRID_SIZE; ++x) {
						int xpos = (x - (GRID_SIZE / 2)) * 2;
						int ypos = (y - (GRID_SIZE / 2)) * 2;
						int zpos = (z - (GRID_SIZE / 2)) * 2;
						vec3 relativePosition = vec3(xpos, ypos, zpos);
						if (relativePosition == vec3(0)) {
							continue;
						}
						instance_positions.push_back(glm::translate(glm::mat4(1.0f), sizeScale * relativePosition));
					}
				}
			}

			instanceCount = (GLuint)instance_positions.size();
		}
	}

	// Note: modelview is actually the view matrix
	void render(const mat4 & projection, const mat4 & modelview) {
		for (mat4 mat : instance_positions) {
			vec3 scale = vec3(0.2f, 0.2f, 0.2f);
			shader.use();
			shader.setMat4("ProjectionMatrix", projection);
			shader.setMat4("CameraMatrix", modelview);
			shader.setMat4("InstanceTransform", mat);
			shader.setMat4("ModelMatrix", glm::scale(mat4(1.0), vec3(0.1, 0.1, 0.1)));
			shader.setVec4("Color", defaultColor);
			m.Draw(shader);
		}
	}

};
*/
