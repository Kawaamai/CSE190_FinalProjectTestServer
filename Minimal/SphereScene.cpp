#include "SphereScene.h"

// not this one. no lighting
SphereScene::SphereScene() : sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(.05, 18, 12)) {
	using namespace oglplus;
	try {
		// attach the shaders to the program
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("oglBasicColor.frag"))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("oglBasicColor.vert"))))
			.Compile()
		);
		prog.Link();
	}
	catch (ProgramBuildError & err) {
		FAIL((const char*)err.what());
	}

	// link and use it
	prog.Use();
	vao = sphere.VAOForProgram(prog);
	vao.Bind();
	// Create a cube of cubes
	{
		for (unsigned int z = 0; z < GRID_SIZE; ++z) {
			for (unsigned int y = 0; y < GRID_SIZE; ++y) {
				for (unsigned int x = 0; x < GRID_SIZE; ++x) {
					int xpos = (x - (GRID_SIZE / 2)) * 2;
					int ypos = (y - (GRID_SIZE / 2)) * 2;
					int zpos = (z - (GRID_SIZE / 2)) * 2;
					vec3 relativePosition = vec3(xpos, ypos, zpos);
					//if (relativePosition == vec3(0)) {
					//	continue;
					//}
					mat4 loc = glm::translate(mat4(1.0f), gridSizeScale * relativePosition);
					// translate down a bit
					loc = glm::translate(loc, vec3(0, -0.2f, 0));
					instance_positions.push_back(loc);
					instance_colors.push_back(baseColor);
				}
			}
		}

		base_instance_positions = instance_positions;
		Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
		instanceCount = (GLuint)instance_positions.size();
		int stride = sizeof(mat4);
		// position
		for (int i = 0; i < 4; ++i) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
		}

		// color
		Context::Bound(Buffer::Target::Array, colors).Data(instance_colors);
		stride = sizeof(vec4);
		VertexArrayAttrib instance_attr(prog, Attribute::Color);
		instance_attr.Pointer(4, DataType::Float, false, stride, (void*)0);
		instance_attr.Divisor(1);
		instance_attr.Enable();

		highlightedSphere = genRandNum();
		chooseNewHighlightSphere();
	}
}

SphereScene::SphereScene(Lighting light) :  sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(.05, 18, 12)), sceneLight(light), lighting(true) {
	using namespace oglplus;
	try {
		// attach the shaders to the program
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("lightedColor.frag"))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("lightedColor.vert"))))
			.Compile()
		);
		prog.Link();
	}
	catch (ProgramBuildError & err) {
		FAIL((const char*)err.what());
	}

	// link and use it
	prog.Use();
	vao = sphere.VAOForProgram(prog);
	vao.Bind();
	// Create a cube of cubes
	{
		for (unsigned int z = 0; z < GRID_SIZE; ++z) {
			for (unsigned int y = 0; y < GRID_SIZE; ++y) {
				for (unsigned int x = 0; x < GRID_SIZE; ++x) {
					int xpos = (x - (GRID_SIZE / 2)) * 2;
					int ypos = (y - (GRID_SIZE / 2)) * 2;
					int zpos = (z - (GRID_SIZE / 2)) * 2;
					vec3 relativePosition = vec3(xpos, ypos, zpos);
					//if (relativePosition == vec3(0)) {
					//	continue;
					//}
					mat4 loc = glm::translate(mat4(1.0f), gridSizeScale * relativePosition);
					// translate down a bit
					loc = glm::translate(loc, vec3(0, -0.2f, 0));
					instance_positions.push_back(loc);
					instance_colors.push_back(baseColor);
				}
			}
		}

		base_instance_positions = instance_positions;
		Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
		instanceCount = (GLuint)instance_positions.size();
		int stride = sizeof(mat4);
		// position
		for (int i = 0; i < 4; ++i) {
			VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
		}

		// color
		Context::Bound(Buffer::Target::Array, colors).Data(instance_colors);
		stride = sizeof(vec4);
		VertexArrayAttrib instance_attr(prog, Attribute::Color);
		instance_attr.Pointer(4, DataType::Float, false, stride, (void*)0);
		instance_attr.Divisor(1);
		instance_attr.Enable();

		highlightedSphere = genRandNum();
		chooseNewHighlightSphere();
	}
}

SphereScene::~SphereScene()
{
}

void SphereScene::chooseNewHighlightSphere()
{
	instance_colors[highlightedSphere] = baseColor;
	highlightedSphere = genRandNum();
	instance_colors[highlightedSphere] = highlightColor;

	// link and use it
	prog.Use();
	vao.Bind();

	// recolor
	oglplus::Context::Bound(oglplus::Buffer::Target::Array, colors).Data(instance_colors);
	GLuint stride = sizeof(vec4);
	oglplus::VertexArrayAttrib instance_attr(prog, Attribute::Color);
	instance_attr.Pointer(4, oglplus::DataType::Float, false, stride, (void*)0);
	instance_attr.Divisor(1);
	instance_attr.Enable();
}

void SphereScene::render(const mat4 & projection, const mat4 & view) {
	using namespace oglplus;
	//updatePosition();
	prog.Use();
	Uniform<mat4>(prog, "ProjectionMatrix").Set(projection);
	Uniform<mat4>(prog, "CameraMatrix").Set(view);
	if (lighting) {
		Uniform<vec3>(prog, "lightPos").Set(sceneLight.lightPos);
		Uniform<vec3>(prog, "lightColor").Set(sceneLight.lightColor);
	}
	//Uniform<mat4>(prog, "MModelMatrix").Set((100.0f * translation) * orientation);
	Uniform<mat4>(prog, "ModelMatrix").Set(toWorld);
	vao.Bind();
	
	sphere.Draw(instanceCount);
}

void SphereScene::resetPositions() {
	//translation = glm::mat4(1.0f);
	//orientation = glm::mat4(1.0f);
	toWorld = glm::mat4(1.0f);
	//instance_positions = base_instance_positions;
	updatePosition();
}

int SphereScene::genRandNum()
{
	// random generation
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, instanceCount - 1);
	return dis(gen);
}

// TODO: just use model matrix instead
void SphereScene::updatePosition() {
	using namespace oglplus;
	for (unsigned int i = 0; i < instanceCount; i++) {
		instance_positions[i] = toWorld * base_instance_positions[i];
		//instance_positions[i] = base_instance_positions[i] * translation * orientation;
		//instance_positions[i] = (orientation * base_instance_positions[i]) * translation;
		//instance_positions[i] = translation * (orientation * base_instance_positions[i]);
		//instance_positions[i] = translation * orientation * base_instance_positions[i];
	}

	prog.Use();
	vao.Bind();
	Context::Bound(Buffer::Target::Array, instances).Data(instance_positions);
	GLuint stride = sizeof(mat4);
	// position
	for (int i = 0; i < 4; ++i) {
		VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
		size_t offset = sizeof(vec4) * i;
		instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
		instance_attr.Divisor(1);
		instance_attr.Enable();
	}
}
