#include "GridScene.h"

GridScene::GridScene(int width, int height, float objSize, float scale, Lighting light) :
	width(width),
	height(height),
	objSize(objSize),
	scale(scale),
	sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(objSize, 18, 12)),
	cube({ "Position", "Normal" }, oglplus::shapes::Cube(objSize, objSize, objSize)),
	sceneLight(light)
{
	using namespace oglplus;
	try {
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/oglBlinnPhongColor.frag"))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/oglBlinnPhongColor.vert"))))
			.Compile()
		);
		prog.Link();
	} catch (ProgramBuildError & err) {
		FAIL((const char*)err.what());
	}

	for (unsigned int y = 0; y < height; ++y) {
		positions.push_back(std::vector<glm::vec3>());
		for (unsigned int x = 0; x < width; ++x) {
			int xpos = (x - (width / 2)) * 2;
			int ypos = (y - (height / 2)) * 2;
			int zpos = 0;
			vec3 relativePosition = vec3(xpos, ypos, zpos);
			positions.at(y).push_back(scale * relativePosition);
			mat4 loc = glm::translate(mat4(1.0f), scale * relativePosition);
			instancePositions.push_back(loc);
		}
	}

	instanceCount = instancePositions.size();

	prog.Use();
	sphereVao = sphere.VAOForProgram(prog);
	sphereVao.Bind();

	int stride = sizeof(glm::mat4);
	Context::Bound(Buffer::Target::Array, instances).Data(instancePositions);
	for (int i = 0; i < 4; i++) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
	}

	cubeVao = cube.VAOForProgram(prog);
	cubeVao.Bind();
	Context::Bound(Buffer::Target::Array, instances).Data(instancePositions);
	for (int i = 0; i < 4; i++) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
	}
}

void GridScene::renderSphereGrid(
	const glm::mat4 & projection,
	const glm::mat4 & view,
	const glm::vec3 & eyePos,
	glm::vec3 color)
{
	using namespace oglplus;

	prog.Use();
	Uniform<glm::mat4>(prog, "ProjectionMatrix").Set(projection);
	Uniform<glm::mat4>(prog, "CameraMatrix").Set(view);
	Uniform<glm::vec3>(prog, "lightPos").Set(sceneLight.lightPos);
	Uniform<glm::mat4>(prog, "ModelMatrix").Set(toWorld);
	Uniform<glm::vec3>(prog, "color").Set(color);
	Uniform<glm::vec3>(prog, "viewPos").Set(eyePos);
	Uniform<glm::mat4>(prog, "LightSpaceMatrix").Set(lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	sphereVao.Bind();
	sphere.Draw(instanceCount);
}

void GridScene::renderCubeGrid(
	const glm::mat4 & projection,
	const glm::mat4 & view,
	const glm::vec3 & eyePos,
	glm::vec3 color)
{
	using namespace oglplus;

	prog.Use();
	Uniform<glm::mat4>(prog, "ProjectionMatrix").Set(projection);
	Uniform<glm::mat4>(prog, "CameraMatrix").Set(view);
	Uniform<glm::vec3>(prog, "lightPos").Set(sceneLight.lightPos);
	Uniform<glm::mat4>(prog, "ModelMatrix").Set(toWorld);
	Uniform<glm::vec3>(prog, "color").Set(color);
	Uniform<glm::vec3>(prog, "viewPos").Set(eyePos);
	Uniform<glm::mat4>(prog, "LightSpaceMatrix").Set(lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	cubeVao.Bind();
	cube.Draw(instanceCount);
}

void GridScene::shadowrenderSphereGrid(const glm::mat4 & projection, const glm::mat4 & view) {
	shadowShader.use();
	shadowShader.setMat4("ProjectionMatrix", projection);
	shadowShader.setMat4("CameraMatrix", view);
	shadowShader.setMat4("ModelMatrix", toWorld);
	sphereVao.Bind();
	sphere.Draw(instanceCount);
}

void GridScene::shadowrenderCubeGrid(const glm::mat4 & projection, const glm::mat4 & view) {
	shadowShader.use();
	shadowShader.setMat4("ProjectionMatrix", projection);
	shadowShader.setMat4("CameraMatrix", view);
	shadowShader.setMat4("ModelMatrix", toWorld);
	cubeVao.Bind();
	cube.Draw(instanceCount);
}

bool GridScene::nearGridPoint(const glm::vec3 p) {
	for (const std::vector<glm::vec3>& v : positions) {
		for (const glm::vec3& gp : v) {
			if (glm::distance(glm::vec3(toWorld * glm::vec4(gp, 1.0f)), p) < objSize + .05f)
				return true;
		}
	}
	return false;
}
