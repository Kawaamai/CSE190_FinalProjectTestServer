#pragma once

//#include "Model.h"
#include "Shader.h" // has glm.hpp
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "OvrHelper.h"

#include "Core.h"
#include "Lighting.h"

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/basic.hpp>
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#include <oglplus/shapes/cube.hpp>
#include <oglplus/shapes/sphere.hpp>
#include <oglplus/shapes/wrapper.hpp>
#pragma warning( default : 4068 4244 4267 4065)

#include "oglShaderAttributes.h"

class GridScene {
public:
	glm::mat3 toWorld = glm::mat4(1.0f);

	GridScene(int width, int height, float objSize, float scale, Lighting light);
	~GridScene() {}

	void renderSphereGrid(const glm::mat4& projection,
		const glm::mat4& view,
		const ovrPosef& eyePose,
		glm::vec3 color);
	void renderCubeGrid(const glm::mat4& projection,
		const glm::mat4& view,
		const ovrPosef& eyePose,
		glm::vec3 color);

	bool nearGridPoint(const glm::vec3 p);

private:
	int width = 1, height = 1;
	float scale = 1.0f;
	oglplus::Program prog;

	// sphere
	oglplus::shapes::ShapeWrapper sphere;
	oglplus::VertexArray sphereVao;
	// cube
	oglplus::shapes::ShapeWrapper cube;
	oglplus::VertexArray cubeVao;

	oglplus::Buffer instances;
	int instanceCount;
	std::vector<glm::mat4> instancePositions;
	std::vector<std::vector<glm::vec3>> positions;

	Lighting sceneLight;
};

