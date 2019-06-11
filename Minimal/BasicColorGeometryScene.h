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
#include "ShadowRenderingHeader.h"

namespace BasicColors {
	static const glm::vec3 orangeColor = glm::vec3(238.0f, 127.0f, 27.0f) / 255.0f;
	static const glm::vec3 blueColor = glm::vec3(0.0f, 120.0f, 255.0f) / 255.0f;
}

class BasicColorGeometryScene {
public:
	Lighting sceneLight;

	BasicColorGeometryScene(Lighting light);
	~BasicColorGeometryScene() {};

	void renderSphere(const glm::mat4& projection,
		const glm::mat4& view,
		const glm::mat4& toWorld,
		const glm::vec3 & eyePos,
		glm::vec3 color = BasicColors::orangeColor);
	void renderCube(const glm::mat4& projection,
		const glm::mat4& view,
		const glm::mat4& toWorld,
		const glm::vec3 & eyePos,
		glm::vec3 color = BasicColors::orangeColor);
	void renderLine(const glm::mat4& projection,
		const glm::mat4& view,
		const glm::vec3& pt1,
		const glm::vec3& pt2,
		const glm::vec3 & eyePos,
		glm::vec3 color = BasicColors::orangeColor);
	void shadowrenderSphere(const glm::mat4& projection,
		const glm::mat4& view,
		const glm::mat4& toWorld,
		glm::vec3 color = BasicColors::orangeColor);
	void shadowrenderCube(const glm::mat4& projection,
		const glm::mat4& view,
		const glm::mat4& toWorld,
		glm::vec3 color = BasicColors::orangeColor);

private:
	oglplus::Program prog;

	// sphere
	oglplus::shapes::ShapeWrapper sphere;
	oglplus::VertexArray sphereVao;
	// cube
	oglplus::shapes::ShapeWrapper cube;
	oglplus::VertexArray cubeVao;

	oglplus::Buffer instances;
	std::vector<glm::mat4> instance_positions = { glm::mat4(1.0f) };

	// line
	Shader lineShader = Shader("../Minimal/basicColor.vert", "../Minimal/basicColor.frag");
	GLuint lineVao, lineVbo;
	static const int numLinePoints = 2;
	glm::vec3 lineVertices[numLinePoints] = { glm::vec3(1.0), glm::vec3(0.0) };

	Shader shadowShader = Shader("../Minimal/osimpleDepthMapShader.vert", "../Minimal/simpleDepthMapShader.frag");
};

