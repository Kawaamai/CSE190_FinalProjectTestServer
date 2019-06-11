#include "BasicColorGeometryScene.h"

BasicColorGeometryScene::BasicColorGeometryScene(Lighting light) :
	sphere({ "Position", "Normal" }, oglplus::shapes::Sphere()),
	cube({ "Position", "Normal" }, oglplus::shapes::Cube()),
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

	prog.Use();
	sphereVao = sphere.VAOForProgram(prog);
	sphereVao.Bind();

	int stride = sizeof(glm::mat4);
	Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
	for (int i = 0; i < 4; i++) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
	}

	cubeVao = cube.VAOForProgram(prog);
	cubeVao.Bind();
	Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
	for (int i = 0; i < 4; i++) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
	}

	// line
	glGenVertexArrays(1, &lineVao);
	glGenBuffers(1, &lineVbo);

	glBindVertexArray(lineVao);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(Attribute::Position);
	glVertexAttribPointer(Attribute::Position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glDisableVertexAttribArray(Attribute::InstanceTransform);
	glDisableVertexAttribArray(Attribute::Normal);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BasicColorGeometryScene::renderSphere(
	const glm::mat4 & projection,
	const glm::mat4 & view,
	const glm::mat4 & toWorld,
	const glm::vec3& eyePos,
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
	sphere.Draw(1);
}

void BasicColorGeometryScene::renderCube(
	const glm::mat4 & projection,
	const glm::mat4 & view,
	const glm::mat4 & toWorld,
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
	cube.Draw(1);
}

void BasicColorGeometryScene::renderLine(
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3& pt1,
	const glm::vec3& pt2,
	const glm::vec3 & eyePos,
	glm::vec3 color)
{
	lineVertices[0] = pt1;
	lineVertices[1] = pt2;

	glBindVertexArray(lineVao);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineVertices), lineVertices);
	glVertexAttribPointer(Attribute::Position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	lineShader.use();
	lineShader.setMat4("ProjectionMatrix", projection);
	lineShader.setMat4("CameraMatrix", view);
	lineShader.setVec4("Color", glm::vec4(color, 1.0f));

	glBindVertexArray(lineVao);

	glLineWidth(5.0f);
	glDrawArrays(GL_LINES, 0, numLinePoints);
	glBindVertexArray(0);
	glLineWidth(1.0f);
}

void BasicColorGeometryScene::shadowrenderSphere(const glm::mat4 & projection, const glm::mat4 & view, const glm::mat4 & toWorld, glm::vec3 color) {
	shadowShader.use();
	shadowShader.setMat4("ProjectionMatrix", projection);
	shadowShader.setMat4("CameraMatrix", view);
	shadowShader.setMat4("ModelMatrix", toWorld);
	sphereVao.Bind();
	sphere.Draw(1);
}

void BasicColorGeometryScene::shadowrenderCube(const glm::mat4 & projection, const glm::mat4 & view, const glm::mat4 & toWorld, glm::vec3 color) {
	shadowShader.use();
	shadowShader.setMat4("ProjectionMatrix", projection);
	shadowShader.setMat4("CameraMatrix", view);
	shadowShader.setMat4("ModelMatrix", toWorld);
	cubeVao.Bind();
	cube.Draw(1);
}

