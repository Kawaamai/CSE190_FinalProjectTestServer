#pragma once

#ifndef CONTROLLER_HANDLER_H
#define CONTROLLER_HANDLER_H

#include <GL/glew.h>

//#include "Model.h"
#include "Shader.h" // has glm.hpp
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "OvrHelper.h"
#include "BasicColorGeometryScene.h"
#include "HandModel.h"
#include "Lighting.h"

class ControllerHandler
{
public:
	const bool debug = false;

	ovrSession _session;

	// Process controller status. Useful to know if controller is being used at all, and if the cameras can see it. 
	// Bits reported:
	// Bit 1: ovrStatus_OrientationTracked  = Orientation is currently tracked (connected and in use)
	// Bit 2: ovrStatus_PositionTracked     = Position is currently tracked (false if out of range)
	unsigned int handStatus[2];
	// Process controller position and orientation:
	ovrPosef handPoses[2];  // These are position and orientation in meters in room coordinates, relative to tracking origin. Right-handed cartesian coordinates.
							// ovrQuatf     Orientation;
							// ovrVector3f  Position;
	ovrPosef lastHandPoses[2];
	ovrInputState prevInputState, currInputState;
	

	//std::unique_ptr<BasicColorGeometryScene> basicShapeRenderer;
	std::unique_ptr<HandModel> handModel;
	const glm::vec3 scale = glm::vec3(0.01f);

	const float handOffset = 0.1f;

	//lighting
	Lighting sceneLight;
	bool lighting = false;

	// smoothing and lag

	ControllerHandler(const ovrSession & s, Lighting light);
	~ControllerHandler();

	void renderHands(const glm::mat4 & projection, const glm::mat4 & modelview, const glm::vec3 & eyePos);
	void updateHandState();

	unsigned int gethandStatus(unsigned int hand) {
		return handStatus[hand];
	}

	glm::vec3 getPointerPosition() {
		glm::mat4 offset
			= glm::translate(glm::mat4(), glm::vec3(glm::mat4_cast(ovr::toGlm(handPoses[ovrHand_Right].Orientation)) * glm::vec4(0, 0, -handOffset, 1)));
		return offset * glm::vec4(ovr::toGlm(handPoses[ovrHand_Right].Position), 1.0f);
	}

	// hand positions
	glm::vec3 getHandPosition(unsigned int hand) {
		if (!gethandStatus(hand))
			return glm::vec3();
		return ovr::toGlm(handPoses[hand].Position);
	}
	glm::vec3 getHandPositionChange(unsigned int hand) {
		if (!gethandStatus(hand))
			return glm::vec3();
		return ovr::toGlm(handPoses[hand].Position) - ovr::toGlm(lastHandPoses[hand].Position);
	}
	glm::quat getHandRotation(unsigned int hand) {
		if (!gethandStatus(hand))
			return glm::quat();
		return ovr::toGlm(handPoses[hand].Orientation);
	}
	// relative hand rotation from last to current rotations
	glm::quat getHandRotationChange(unsigned int hand) {
		if (!gethandStatus(hand))
			return glm::quat();
		glm::quat currHandQuat = ovr::toGlm(handPoses[hand].Orientation);
		glm::quat lastHandQuat = ovr::toGlm(lastHandPoses[hand].Orientation);

		//return glm::inverse(lastHandQuat) * currHandQuat;
		return currHandQuat * glm::inverse(lastHandQuat);
	}

	// button handlers
	bool r_HandTriggerDown() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((currInputState.HandTrigger[ovrHand_Right] > 0.5f) &&
			(prevInputState.HandTrigger[ovrHand_Right] < 0.5f));
	}
	bool l_HandTriggerDown() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((currInputState.HandTrigger[ovrHand_Left] > 0.5f) &&
			(prevInputState.HandTrigger[ovrHand_Left] < 0.5f));
	}
	bool r_IndexTriggerDown() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((currInputState.IndexTrigger[ovrHand_Right] > 0.5f) &&
			(prevInputState.IndexTrigger[ovrHand_Right] < 0.5f));
	}
	bool l_IndexTriggerDown() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((currInputState.IndexTrigger[ovrHand_Left] > 0.5f) &&
			(prevInputState.IndexTrigger[ovrHand_Left] < 0.5f));
	}
	bool r_IndexTriggerPressed() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return currInputState.IndexTrigger[ovrHand_Right] > 0.5f;
	}
	bool l_IndexTriggerPressed() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return currInputState.IndexTrigger[ovrHand_Left] > 0.5f;
	}
	bool r_AButtonDown() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((currInputState.Buttons & ovrButton_A) &&
			((prevInputState.Buttons & ovrButton_A) == 0));
	}
	bool r_AButtonPressed() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return currInputState.Buttons & ovrButton_A;
	}
	bool r_BButtonDown() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((currInputState.Buttons & ovrButton_B) &&
			((prevInputState.Buttons & ovrButton_B) == 0));
	}
	bool r_BButtonPressed() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return currInputState.Buttons & ovrButton_B;
	}
	bool l_XButtonDown() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((currInputState.Buttons & ovrButton_X) &&
			((prevInputState.Buttons & ovrButton_X) == 0));
	}
	bool l_YButtonDown() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((currInputState.Buttons & ovrButton_Y) &&
			((prevInputState.Buttons & ovrButton_Y) == 0));
	}
	bool l_YButtonPressed() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return currInputState.Buttons & ovrButton_Y;
	}
	bool r_HandTriggerUp() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((prevInputState.HandTrigger[ovrHand_Right] > 0.5f) &&
			(currInputState.HandTrigger[ovrHand_Right] < 0.5f));
	}
	bool l_HandTriggerUp() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((prevInputState.HandTrigger[ovrHand_Left] > 0.5f) &&
			(currInputState.HandTrigger[ovrHand_Left] < 0.5f));
	}
	bool r_IndexTriggerUp() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((prevInputState.IndexTrigger[ovrHand_Right] > 0.5f) &&
			(currInputState.IndexTrigger[ovrHand_Right] < 0.5f));
	}
	bool l_IndexTriggerUp() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((prevInputState.IndexTrigger[ovrHand_Left] > 0.5f) &&
			(currInputState.IndexTrigger[ovrHand_Left] < 0.5f));
	}
	bool r_AButtonUp() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((prevInputState.Buttons & ovrButton_A) &&
			((currInputState.Buttons & ovrButton_A) == 0));
	}
	bool r_BButtonUp() {
		if (!gethandStatus(ovrHand_Right))
			return false;
		return ((prevInputState.Buttons & ovrButton_B) &&
			((currInputState.Buttons & ovrButton_B) == 0));
	}
	bool l_XButtonUp() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((prevInputState.Buttons & ovrButton_X) &&
			((currInputState.Buttons & ovrButton_X) == 0));
	}
	bool l_YButtonUp() {
		if (!gethandStatus(ovrHand_Left))
			return false;
		return ((prevInputState.Buttons & ovrButton_Y) &&
			((currInputState.Buttons & ovrButton_Y) == 0));
	}
	bool isHandTriggerPressed(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.HandTrigger[hand] > 0.5f;
	}
	bool isIndexTriggerPressed(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.IndexTrigger[hand] > 0.5f;
	}

	// thumbstick
	bool isThumbstickUp(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.Thumbstick[hand].y > 0.5f;
	}
	bool isThumbstickDown(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.Thumbstick[hand].y < -0.5f;
	}
	bool isThumbstickLeft(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.Thumbstick[hand].x < -0.5f;
	}
	bool isThumbstickRight(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		return currInputState.Thumbstick[hand].x > 0.5f;
	}
	bool isThumbstickButtonDown(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		if (hand == ovrHand_Left)
			return ((currInputState.Buttons & ovrButton_LThumb) &&
				((prevInputState.Buttons & ovrButton_LThumb) == 0));
		else
			return ((currInputState.Buttons & ovrButton_RThumb) &&
				((prevInputState.Buttons & ovrButton_RThumb) == 0));
	}
	bool isThumbstickButtonUp(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		if (hand == ovrHand_Left)
			return ((prevInputState.Buttons & ovrButton_LThumb) &&
				((currInputState.Buttons & ovrButton_LThumb) == 0));
		else
			return ((prevInputState.Buttons & ovrButton_RThumb) &&
				((currInputState.Buttons & ovrButton_RThumb) == 0));
	}

	bool isTouchRestDown(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		if (hand == ovrHand_Left)
			return ((currInputState.Touches & ovrTouch_LThumbRest) &&
				((prevInputState.Touches & ovrTouch_LThumbRest) == 0));
		else
			return ((currInputState.Touches & ovrTouch_RThumbRest) &&
				((prevInputState.Touches & ovrTouch_RThumbRest) == 0));
	}
	bool isTouchThumbRestPressed(unsigned int hand) {
		if (!gethandStatus(hand))
			return false;
		if (hand == ovrHand_Left)
			return currInputState.Touches & ovrTouch_LThumbRest;
		else
			return currInputState.Touches & ovrTouch_RThumbRest;
	}

private:

	void updateHands();
	void buttonHandler();
	//void renderHand(const glm::mat4 & projection, const glm::mat4 & modelview, const ovrVector3f & handPosition);
};

#endif // !CONTROLLER_HANDLER_H
