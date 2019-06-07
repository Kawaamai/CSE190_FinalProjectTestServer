#include "ControllerHandler.h"
#include "oglShaderAttributes.h"

ControllerHandler::ControllerHandler(const ovrSession & s) :
	_session(s),
	instanceCount(0),
	sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(0.01, 18, 12)),
	lighting(false)
{
	using namespace oglplus;
	try {
		// attach the shaders to the program
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile(fragShader))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile(vertexShader))))
			.Compile()
		);
		prog.Link();
	}
	catch (ProgramBuildError & err) {
		throw std::runtime_error((const char*)err.what());
	}

	// link and use it
	prog.Use();

	vao = sphere.VAOForProgram(prog);
	vao.Bind();

	// color
	Context::Bound(Buffer::Target::Array, colors).Data(instance_colors);
	GLuint stride = sizeof(glm::vec4);
	VertexArrayAttrib instance_attr(prog, Attribute::Color);
	instance_attr.Pointer(4, DataType::Float, false, stride, (void*)0);
	instance_attr.Divisor(1);
	instance_attr.Enable();
}

ControllerHandler::ControllerHandler(const ovrSession & s, Lighting light) :
	_session(s),
	instanceCount(0),
	sphere({ "Position", "Normal" }, oglplus::shapes::Sphere(0.01, 18, 12)),
	sceneLight(light),
	lighting(true)
{
	using namespace oglplus;
	try {
		// attach the shaders to the program
		prog.AttachShader(
			FragmentShader()
			.Source(GLSLSource(String(::Shader::openShaderFile(lightFragShader))))
			.Compile()
		);
		prog.AttachShader(
			VertexShader()
			.Source(GLSLSource(String(::Shader::openShaderFile(lightVertexShader))))
			.Compile()
		);
		prog.Link();
	}
	catch (ProgramBuildError & err) {
		throw std::runtime_error((const char*)err.what());
	}

	// link and use it
	prog.Use();

	vao = sphere.VAOForProgram(prog);
	vao.Bind();

	// color
	Context::Bound(Buffer::Target::Array, colors).Data(instance_colors);
	GLuint stride = sizeof(glm::vec4);
	VertexArrayAttrib instance_attr(prog, Attribute::Color);
	instance_attr.Pointer(4, DataType::Float, false, stride, (void*)0);
	instance_attr.Divisor(1);
	instance_attr.Enable();
}


ControllerHandler::~ControllerHandler()
{
}

void ControllerHandler::renderHands(const glm::mat4 & projection, const glm::mat4 & modelview)
{
	std::vector<glm::mat4> instance_positions;
	// update

	// render hands
	if (handStatus[ovrHand_Left]) {
		//renderHand(projection, modelview, handPoses[0].Position);
		glm::vec3 handPosition = ovr::toGlm(handPoses[ovrHand_Left].Position);
		glm::mat4 transform = glm::translate(glm::mat4(1.0), handPosition);
		instance_positions.push_back(transform);
	}
	if (handStatus[ovrHand_Right]) {
		glm::vec3 handPosition = ovr::toGlm(handPoses[ovrHand_Right].Position);
		glm::mat4 transform = glm::translate(glm::mat4(1.0), handPosition);
		instance_positions.push_back(transform);
	}

	// render the hands
	// link and use it
	prog.Use();
	vao.Bind();
	oglplus::Uniform<glm::mat4>(prog, "ProjectionMatrix").Set(projection);
	oglplus::Uniform<glm::mat4>(prog, "CameraMatrix").Set(modelview);
	if (lighting) {
		oglplus::Uniform<glm::vec3>(prog, "lightPos").Set(sceneLight.lightPos);
		oglplus::Uniform<glm::vec3>(prog, "lightColor").Set(sceneLight.lightColor);
	}

	// hand positions
	oglplus::Context::Bound(oglplus::Buffer::Target::Array, instances).Data(instance_positions);
	instanceCount = (GLuint)instance_positions.size();
	int stride = sizeof(glm::mat4);
	for (int i = 0; i < 4; ++i) {
		oglplus::VertexArrayAttrib instance_attr(prog, Attribute::InstanceTransform + i);
		size_t offset = sizeof(glm::vec4) * i;
		instance_attr.Pointer(4, oglplus::DataType::Float, false, stride, (void*)offset);
		instance_attr.Divisor(1);
		instance_attr.Enable();
	}

	sphere.Draw(instanceCount);
}

void ControllerHandler::updateHandState()
{
	updateHands();
	buttonHandler();
	smoothingIdx = incRingIdx(smoothingBuffer, smoothingIdx);
}

// Important: make sure this is only called one time each frame
void ControllerHandler::updateHands()
{
	double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, 0);
	ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);
	
	handStatus[0] = trackState.HandStatusFlags[0];
	handStatus[1] = trackState.HandStatusFlags[1];

	// Display status for debug purposes:
	//std::cerr << "handStatus[left]  = " << handStatus[ovrHand_Left] << std::endl;
	//std::cerr << "handStatus[right] = " << handStatus[ovrHand_Right] << std::endl;

	lastHandPoses[0] = handPoses[0];
	lastHandPoses[1] = handPoses[1];
	handPoses[0] = trackState.HandPoses[0].ThePose;
	handPoses[1] = trackState.HandPoses[1].ThePose;
	//ovrVector3f handPosition[2];
	//handPosition[0] = handPoses[0].Position;
	//handPosition[1] = handPoses[1].Position;
}

// assumes updated hand poses
void ControllerHandler::buttonHandler()
{
	ovrInputState inputState;

	if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState))) {
		prevInputState = currInputState;
		currInputState = inputState;
		if (debug) {
			// buttons
			if (inputState.Buttons & ovrButton_A) {
				std::cerr << "a button pressed" << std::endl;
			}
			if (inputState.Buttons & ovrButton_B) {
				std::cerr << "b button pressed" << std::endl;
			}
			if (inputState.Buttons & ovrButton_X) {
				std::cerr << "x button pressed" << std::endl;
			}
			if (inputState.Buttons & ovrButton_Y) {
				std::cerr << "y button pressed" << std::endl;
			}

			// triggeres
			if (inputState.HandTrigger[ovrHand_Right] > 0.5f) {
				std::cerr << "right hand trigger pressed" << std::endl;
			}
			if (inputState.HandTrigger[ovrHand_Left] > 0.5f) {
				std::cerr << "left hand trigger pressed" << std::endl;
			}
			if (inputState.IndexTrigger[ovrHand_Right] > 0.5f) {
				std::cerr << "right index trigger pressed" << std::endl;
			}
			if (inputState.IndexTrigger[ovrHand_Left] > 0.5f) {
				std::cerr << "left index trigger pressed" << std::endl;
			}
		}
	}
}

glm::vec3 ControllerHandler::calcSmoothPos(unsigned int hand) {
	if (smoothing == 0)
		return getRingAt(smoothingBuffer, smoothingIdx - lag)[hand];

	glm::vec3 total;
	for (int i = 0; i < smoothing; i++) {
		total += getRingAt(smoothingBuffer, smoothingIdx - i - lag)[hand];
	}

	return total / (float) smoothing;
}

//void ControllerHandler::renderHand(
//	const glm::mat4 & projection,
//	const glm::mat4 & modelview,
//	const ovrVector3f & handPosition)
//{
//	glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(handPosition.x, handPosition.y, handPosition.z));
//
//	//shader.use();
//	//shader.setMat4("ProjectionMatrix", projection);
//	//shader.setMat4("CameraMatrix", modelview);
//	//shader.setMat4("InstanceTransform", transform);
//	//shader.setMat4("ModelMatrix", glm::scale(glm::mat4(1.0), scale));
//	//handPointer.Draw(shader);
//}
