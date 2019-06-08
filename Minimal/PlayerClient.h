#pragma once

#include "Player.h"
#include "RiftApp.h"
#include "ControllerHandler.h"
#include "AvatarHandler.h"
#include "Lighting.h"

class PlayerClient : public Player {
public:
	std::unique_ptr<ControllerHandler> controllers;
	PlayerClient(ovrSession s, Lighting l) : _session(s), sceneLight(l) {}

	void initPlayerGL() {
		controllers = std::make_unique<ControllerHandler>(_session, sceneLight);
		av = std::make_unique<AvatarHandler>(_session);
	}

	void renderControllers(const glm::mat4& proj, const glm::mat4& view, ovrEyeType eye, ovrPosef eyePose) {
		controllers->renderHands(proj, view, eyePose);
	}

	void renderHands(const glm::mat4& proj, const glm::mat4& view, ovrEyeType eye, ovrPosef eyePose) {
		// hand avatar rendering
		ovrVector3f eyePosition = eyePose.Position;
		//glm::vec3 eyeWorld = toWorld() * glm::vec4(ovr::toGlm(eyePosition), 1.0f);
		glm::vec3 eyeWorld = ovr::toGlm(eyePosition);
		av->updateAvatar(proj, view, eyeWorld);
	}

	glm::vec3 getHandPosition(ovrHandType hand) {
		return glm::vec3(toWorld() * glm::vec4(controllers->getHandPosition(hand), 1.0f));
	}

	void cleanupGl() {
		//av->avatar_shutdown();
	}

private:
	ovrSession _session;
	std::unique_ptr<AvatarHandler> av;
	Lighting sceneLight;
};
