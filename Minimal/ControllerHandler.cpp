#include "ControllerHandler.h"
#include "oglShaderAttributes.h"

ControllerHandler::ControllerHandler(const ovrSession & s, Lighting light) :
	_session(s),
	sceneLight(light)
{
	basicShapeRenderer = std::make_unique<BasicColorGeometryScene>(light);
}


ControllerHandler::~ControllerHandler()
{
}

void ControllerHandler::renderHands(const glm::mat4 & projection, const glm::mat4 & modelview, const ovrPosef& eyePose)
{
	// render hands
	if (handStatus[ovrHand_Left]) {
		//renderHand(projection, modelview, handPoses[0].Position);
		glm::vec3 handPosition = ovr::toGlm(handPoses[ovrHand_Left].Position);
		glm::mat4 transform = glm::translate(glm::mat4(1.0), handPosition);
		glm::mat4 rotTm = glm::mat4_cast(ovr::toGlm(handPoses[ovrHand_Left].Orientation));
		glm::mat4 scaleTm = glm::scale(glm::vec3(scale));
		basicShapeRenderer->renderCube(projection, modelview, transform * rotTm * scaleTm, eyePose, glm::vec3(1.0, 0.0, 1.0));
	}
	if (handStatus[ovrHand_Right]) {
		glm::vec3 handPosition = ovr::toGlm(handPoses[ovrHand_Right].Position);
		glm::mat4 transform = glm::translate(glm::mat4(1.0), handPosition);
		glm::mat4 rotTm = glm::mat4_cast(ovr::toGlm(handPoses[ovrHand_Right].Orientation));
		glm::mat4 scaleTm = glm::scale(glm::vec3(scale));
		basicShapeRenderer->renderCube(projection, modelview, transform * rotTm * scaleTm, eyePose, glm::vec3(1.0, 0.0, 1.0));
	}

}

void ControllerHandler::updateHandState()
{
	updateHands();
	buttonHandler();
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

