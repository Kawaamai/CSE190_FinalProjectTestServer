#include "TestPhysicsScene.h"

TestPhysicsScene::TestPhysicsScene(Lighting light) :
	sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(.05, 18, 12)),
	sceneLight(light),
	lighting(true),
	cube({ "Position", "Normal" }, oglplus::shapes::Cube())
{
	using namespace oglplus;
	try {
		// attach the shaders to the program
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/lightedColor.frag"))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/lightedColor.vert"))))
			.Compile()
		);
		prog.Link();

		// attach the shaders to the program
		cubeProg.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/lightedColor.frag"))))
			.Compile()
		);
		cubeProg.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile("../Minimal/lightedColor.vert"))))
			.Compile()
		);
		cubeProg.Link();
	}
	catch (ProgramBuildError & err) {
		FAIL((const char*)err.what());
	}

	// Create a cube of cubes
	{
		// link and use it
		prog.Use();
		vao = sphere.VAOForProgram(prog);
		vao.Bind();
		//for (unsigned int z = 0; z < GRID_SIZE; ++z) {
		for (unsigned int z = 0; z < 1; ++z) {
			for (unsigned int y = 0; y < GRID_SIZE; ++y) {
				for (unsigned int x = 0; x < GRID_SIZE; ++x) {
					int xpos = (x - (GRID_SIZE / 2)) * 2;
					//int ypos = (y - (GRID_SIZE / 2)) * 2;
					int ypos = GRID_SIZE - y;
					//int zpos = (z - (GRID_SIZE / 2)) * 2;
					vec3 relativePosition = vec3(xpos, ypos, z);
					//if (relativePosition == vec3(0)) {
					//	continue;
					//}
					mat4 loc = glm::translate(mat4(1.0f), gridSizeScale * relativePosition);
					// translate down a bit
					//loc = glm::translate(loc, vec3(0, -0.2f, 0));
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

	{
		// for the cube!
		cubeProg.Use();
		cubeVao = cube.VAOForProgram(cubeProg);
		cubeVao.Bind();
		Context::Bound(Buffer::Target::Array, cubeInstances).Data(cube_instance_positions);
		int stride = sizeof(mat4);
		// position
		for (int i = 0; i < 4; ++i) {
			VertexArrayAttrib instance_attr(cubeProg, Attribute::InstanceTransform + i);
			size_t offset = sizeof(vec4) * i;
			instance_attr.Pointer(4, DataType::Float, false, stride, (void*)offset);
			instance_attr.Divisor(1);
			instance_attr.Enable();
		}

		// color
		Context::Bound(Buffer::Target::Array, cubeColors).Data(cube_instance_colors);
		stride = sizeof(vec4);
		VertexArrayAttrib cube_instance_attr(cubeProg, Attribute::Color);
		cube_instance_attr.Pointer(4, DataType::Float, false, stride, (void*)0);
		cube_instance_attr.Divisor(1);
		cube_instance_attr.Enable();
	}
}

TestPhysicsScene::~TestPhysicsScene() {}

void TestPhysicsScene::render(const glm::mat4 & projection, const glm::mat4 & view) {
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

void TestPhysicsScene::chooseNewHighlightSphere() {
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

void TestPhysicsScene::resetPositions() {
	toWorld = glm::mat4(1.0f);
}

void TestPhysicsScene::renderCube(const glm::mat4 & projection, const glm::mat4 & view, const glm::mat4& toWorld) {
	using namespace oglplus;
	cubeProg.Use();
	Uniform<mat4>(cubeProg, "ProjectionMatrix").Set(projection);
	Uniform<mat4>(cubeProg, "CameraMatrix").Set(view);
	if (lighting) {
		Uniform<vec3>(cubeProg, "lightPos").Set(sceneLight.lightPos);
		Uniform<vec3>(cubeProg, "lightColor").Set(sceneLight.lightColor);
	}
	Uniform<mat4>(cubeProg, "ModelMatrix").Set(toWorld);
	cubeVao.Bind();
	cube.Draw(1);
	//cubeVao.Bind();
	//
	//cube.Draw(1);
}

int TestPhysicsScene::genRandNum() {
	// random generation
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, instanceCount - 1);
	return dis(gen);
}

// TODO:
void TestPhysicsScene::updatePosition() {
}
