#pragma once

#include "Core.h"
#include "OvrHelper.h"
#include "RiftApp.h"
#include "GameClient.h"
#include "Converter.h"
#include "GameInteractionInterface.h"
#include "GameMessageFactory.h"
#include "BasicColorGeometryScene.h"

//////////////////////////////////////////////////////////////////////
//
// OGLplus is a set of wrapper classes for giving OpenGL a more object
// oriented interface
//
#define OGLPLUS_USE_GLCOREARB_H 0
#define OGLPLUS_USE_GLEW 1
#define OGLPLUS_USE_BOOST_CONFIG 0
#define OGLPLUS_NO_SITE_CONFIG 1
#define OGLPLUS_LOW_PROFILE 1

//#include "TestPhysicsScene.h"
#include "AvatarHandler.h"
#include "GridScene.h"
//#include "Player.h"
#include "PlayerClient.h"
#include "Lighting.h"
#include "TextRenderer.h"

#include <vector>
#include <array>

// An example application that renders a simple cube
class TestPhysicsApp : public RiftApp, public GameClient {
	//Player player;
	std::unique_ptr<PlayerClient> player;
	std::array<glm::vec3, 2> playerHeadPose;
	std::array<glm::vec3, 2> controllerInitPressPos;
	bool grabbing = false;
	ovrHandType grabbingHand = ovrHand_Right;

	Player otherPlayer;

	// sphere grid
	//std::unique_ptr<TestPhysicsScene> sphereScene;
	std::unique_ptr<GridScene> gridScene;
	std::unique_ptr<BasicColorGeometryScene> basicShapeRenderer;

	Lighting sceneLight = Lighting(glm::vec3(0, 0, 0), glm::vec3(1.0f));

	std::unique_ptr<TextRenderer> uiFont;
public:
	TestPhysicsApp(const yojimbo::Address& serverAddress) : GameClient(serverAddress) {}

protected:
	void initGl() override {
		RiftApp::initGl();
		glClearColor(0.9f, 0.9f, 0.9f, 0.0f); // background color
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);
		// Note: to disable lighting, don't pass in sceneLight
		player = std::make_unique<PlayerClient>(_session, sceneLight);
		player->initPlayerGL();
		gridScene = std::make_unique<GridScene>(8, 8, .1f, 0.5f, sceneLight);
		basicShapeRenderer = std::make_unique<BasicColorGeometryScene>(sceneLight);
		uiFont = std::make_unique<TextRenderer>("../Minimal/fonts/arial.ttf", 24);
		player->position = glm::vec3(0, 0.5, 0);
	}

	void shutdownGl() override {
		//sphereScene.reset();
		player->cleanupGl();
	}

	// score shown on hand
	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose, ovrEyeType eye, ovrPosef eyePose) override {
		glm::mat4 view = glm::inverse(player->toWorld() * headPose);

		player->renderControllers(projection, glm::inverse(headPose), eye, eyePose);
		player->renderHands(projection, glm::inverse(headPose), eye, eyePose);

		player->orientation = ovr::toGlm(eyePose.Orientation);
		playerHeadPose[eye] = ovr::toGlm(eyePose.Position);

		//sphereScene->render(projection, view);
		renderPxScene(projection, view, eyePose);

		std::string scoreDisplayText = "Score: 0";
		std::string gameStartText = "Right index trigger to start";

		// left hand text
		if (player->controllers->gethandStatus(ovrHand_Left)) {
			//glm::mat4 handPosTrans = glm::translate(controllers->getHandPosition(ovrHand_Left));
			glm::mat4 handPosTrans = glm::translate(player->getHandPosition(ovrHand_Left));
			glm::mat4 handRotTrans = glm::mat4_cast(player->controllers->getHandRotation(ovrHand_Left));
			//                                                                                        (right of hand, above hand, behind hand)
			glm::mat4 gameStartTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, 0.015f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
			//                                                                                    (right of hand, above hand, behind hand)
			glm::mat4 scoreTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, -0.005f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
			uiFont->renderText(projection * glm::inverse(headPose) * gameStartTextTransform, gameStartText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
			uiFont->renderText(projection * glm::inverse(headPose) * scoreTextTransform, scoreDisplayText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
		}

		// render visual of force push
		//glm::vec3 dir = glm::mat4_cast(player->controllers->getHandRotation(ovrHand_Right)) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
		////glm::vec3 pos = player->toWorld() * glm::vec4(controllers->getHandPosition(ovrHand_Right), 1.0f);
		//glm::vec3 pos = player->getHandPosition(ovrHand_Right);
		//glm::vec3 pos2 = pos + (2.0f * dir);

		//glm::mat4 _toWorld = glm::translate(pos) * glm::scale(glm::vec3(.03f)) * glm::mat4_cast(player->controllers->getHandRotation(ovrHand_Right));
		//sphereScene->renderCube(projection, view, _toWorld);
		//_toWorld = glm::translate(pos2) * glm::scale(glm::vec3(.03f));
		//sphereScene->renderCube(projection, view, _toWorld);

		if (player->controllers->r_IndexTriggerPressed()) {
			basicShapeRenderer->renderLine(projection, view, player->getHandPosition(ovrHand_Right), controllerInitPressPos.at(ovrHand_Right), eyePose, glm::vec3(1, 0, 1));
			glm::vec3 dir = glm::normalize(player->getHandPosition(ovrHand_Right) - controllerInitPressPos.at(ovrHand_Right));
			float dist = glm::distance(player->getHandPosition(ovrHand_Right), controllerInitPressPos.at(ovrHand_Right)) * defgame::SWEEP_DIST;
			std::cerr << dist << std::endl;
			glm::vec3 pos = controllerInitPressPos.at(ovrHand_Right) + (dir * dist);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			basicShapeRenderer->renderSphere(projection, view, glm::translate(pos) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS) / 2.0f), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (player->controllers->l_IndexTriggerPressed()) {
			basicShapeRenderer->renderLine(projection, view, player->getHandPosition(ovrHand_Left), controllerInitPressPos.at(ovrHand_Left), eyePose, glm::vec3(1, 0, 1));
			glm::vec3 dir = glm::normalize(player->getHandPosition(ovrHand_Left) - controllerInitPressPos.at(ovrHand_Left));
			float dist = glm::distance(player->getHandPosition(ovrHand_Left), controllerInitPressPos.at(ovrHand_Left)) * defgame::SWEEP_DIST;
			glm::vec3 pos = controllerInitPressPos.at(ovrHand_Left) + (dir * dist);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			basicShapeRenderer->renderSphere(projection, view, glm::translate(pos) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS) / 2.0f), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// show sphere hints
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//basicShapeRenderer->renderSphere(projection, view, glm::translate(player->getHandPosition(ovrHand_Right)) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS)), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
		//basicShapeRenderer->renderSphere(projection, view, glm::translate(player->getHandPosition(ovrHand_Left)) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS)), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		gridScene->renderCubeGrid(projection, view, eyePose, glm::vec3(1.0f));

		// render the other playesr TODO:
		//renderOtherPlayer(projection, view, eyePose);
	}

	void renderPxScene(const glm::mat4 & projection, const glm::mat4 & view, const ovrPosef eyePose) {
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors) {
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			renderPxActors(projection, view, &actors[0], static_cast<PxU32>(actors.size()), eyePose);
		}
	}

	void renderPxActors(const glm::mat4 & projection, const glm::mat4 & view, PxRigidActor** actors, const PxU32 numActors, const ovrPosef eyePose) {
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		for (PxU32 i = 0; i < numActors; i++) {
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes, nbShapes);
			const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

			for (PxU32 j = 0; j < nbShapes; j++) {
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				glm::mat4 toWorld;
				converter::PhysXMat4ToglmMat4(shapePose, toWorld);
				const PxGeometryHolder h = shapes[j]->getGeometry();
				//if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
				//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				//if (sleeping) {
				//	// TODO: change color
				//}
				glm::vec3 color = BasicColors::blueColor;
				if (actors[i]->getType() == PxActorTypeFlag::eRIGID_STATIC) {
					color = BasicColors::orangeColor;
				}
				renderGeometryHolder(projection, view, h, toWorld, eyePose, color);
			}
		}
	}


protected:
	
	void update() override {
		if (Step()) {
			// disconnect
			std::cerr << "connection failed" << std::endl;
			glfwapp_isrunning = false;
		}
		RiftApp::update();
		player->controllers->updateHandState();
		
		// TODO: send server hand + head transforms
		UpdatePlayerPos();
	}

	void SendSweepForce(ovrHandType hand) {
		//PxVec3 sweepDir = converter::glmVec3ToPhysXVec3(glm::mat4_cast(player->controllers->getHandRotation(ovrHand_Right)) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
		//PxVec3 sweepPos = converter::glmVec3ToPhysXVec3(player->getHandPosition(ovrHand_Right));
		PxVec3 sweepDir = converter::glmVec3ToPhysXVec3(player->getHandPosition(hand) - controllerInitPressPos.at(hand));
		PxVec3 sweepPos = converter::glmVec3ToPhysXVec3(controllerInitPressPos.at(hand));
		float dist = glm::distance(player->getHandPosition(hand), controllerInitPressPos.at(hand)) * defgame::SWEEP_DIST;

		// TODO: Determine if I want to do this or if this is redundent
		//PxScene* scene;
		//PxGetPhysics().getScenes(&scene, 1);
		//AddSweepPushForce(scene, sweepDir, sweepPos);

		if (m_client.IsConnected()) {
			std::cerr << "send sweep force" << std::endl;
			SweepForceInputMessage* message = (SweepForceInputMessage*)m_client.CreateMessage((int)GameMessageType::SWEEP_FORCE_INPUT);
			NetVec3 net_sweepDir, net_sweepPos;
			converter::PhysXVec3ToNetVec3(sweepDir, net_sweepDir);
			converter::PhysXVec3ToNetVec3(sweepPos, net_sweepPos);

			message->m_data.sweepDir = net_sweepDir;
			message->m_data.sweepPos = net_sweepPos;
			message->m_data.sweepDistance = dist;
			m_client.SendMessage((int)GameChannel::RELIABLE, message);
		}
	}

	void handleInput() override {
		if (player->controllers->r_IndexTriggerDown()) {
			controllerInitPressPos.at(ovrHand_Right) = player->getHandPosition(ovrHand_Right);
		}
		if (player->controllers->l_IndexTriggerDown()) {
			controllerInitPressPos.at(ovrHand_Left) = player->getHandPosition(ovrHand_Left);
		}
		if (player->controllers->r_IndexTriggerUp()) {
			SendSweepForce(ovrHand_Right);
		}
		if (player->controllers->l_IndexTriggerUp()) {
			SendSweepForce(ovrHand_Left);
		}

		if (player->controllers->r_HandTriggerDown()) {
			if (gridScene->nearGridPoint(player->getHandPosition(ovrHand_Right))) {
				grabbing = true;
				grabbingHand = ovrHand_Right;
			}
		}
		if (player->controllers->l_HandTriggerDown()) {
			if (gridScene->nearGridPoint(player->getHandPosition(ovrHand_Left))) {
				grabbing = true;
				grabbingHand = ovrHand_Left;
			}
		}
		if (player->controllers->r_HandTriggerUp() && grabbingHand == ovrHand_Right) {
			grabbing = false;
		}
		if (player->controllers->l_HandTriggerUp() && grabbingHand == ovrHand_Left) {
			grabbing = false;
		}
	}

	void UpdatePlayerPos() {
		if (grabbing) {
			glm::vec3 delta = player->controllers->getHandPositionChange(grabbingHand);
			player->position = glm::translate(-delta) * glm::vec4(player->position, 1.0f);
		}
	}

	PX_FORCE_INLINE void renderGeometryHolder(const glm::mat4& projection, const glm::mat4& view, const PxGeometryHolder& h, const glm::mat4& toWorld, const ovrPosef eyePose, glm::vec3 color) {
		renderGeometry(projection, view, h.any(), toWorld, eyePose, color);
	}

	void renderGeometry(const glm::mat4& projection, const glm::mat4& view,const PxGeometry& geom, const glm::mat4& toWorld, const ovrPosef eyePose, glm::vec3 color) {
		switch (geom.getType()) {
		case PxGeometryType::eBOX: {
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			glm::mat4 scale = glm::scale(2.0f * glm::vec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z));
			//sphereScene->renderCube(projection, view, toWorld * scale);
			basicShapeRenderer->renderCube(projection, view, toWorld * scale, eyePose, color);
			break;
		}

		case PxGeometryType::eSPHERE:
		case PxGeometryType::eCAPSULE:
		case PxGeometryType::eCONVEXMESH:
		case PxGeometryType::eTRIANGLEMESH:
		case PxGeometryType::eINVALID:
		case PxGeometryType::eHEIGHTFIELD:
		case PxGeometryType::eGEOMETRY_COUNT:
		case PxGeometryType::ePLANE:
			break;
		}
	}

	void renderOtherPlayer(const glm::mat4& projection, const glm::mat4& view, const ovrPosef eyePose) {
		basicShapeRenderer->renderCube(projection, view, glm::translate(otherPlayer.position) * glm::mat4_cast(otherPlayer.orientation) * glm::scale(glm::vec3(0.2)), eyePose);
	}

	// message stuff --------------------------------------------------------------

	void SendEveryUpdate() override {
		PlayerUpdateMessage* message = (PlayerUpdateMessage*)m_client.CreateMessage((int)GameMessageType::PLAYER_UPDATE);
		//message->m_data.transform.position = converter::glmVec3ToNetVec3(players.at(i).position);
		glm::vec3 headPose = (playerHeadPose[0] + playerHeadPose[1]) / 2.0f;
		message->m_data.transform.position = converter::glmVec3ToNetVec3(player->toWorld() * glm::vec4(headPose, 1.0f));
		message->m_data.transform.orientation = converter::glmQuatToNetQuat(player->orientation);
		m_client.SendMessage((int)GameChannel::UNRELIABLE, message);
	}

	void ProcessTransformMessage(TransformMessage * message) override {
		// TODO: timestamp
		unsigned int actorId = message->m_data.int_uniqueGameObjectId;
		if (actorId >= m_numActors || actorId >= m_actors.size()) {
			std::cerr << "ERROR: transform gameobjec id " << actorId << " is not in scene (" << m_numActors << ")" << std::endl;
			return;
		}
		PxTransform tm;
		converter::NetTmToPhysXTm(message->m_data.transform, tm);
		// Do not do anything with this for now
		//m_actors.at(actorId)->setGlobalPose(tm);
	}

	void ProcessRigidbodyMessage(RigidbodyMessage * message) override {
		// TODO: timestamp
		unsigned int actorId = message->m_data.int_uniqueGameObjectId;
		if (actorId >= m_numActors || actorId >= m_actors.size()) {
			std::cerr << "ERROR: transform gameobjec id " << actorId << " is not in scene (" << m_numActors << ")" << std::endl;
			return;
		}
		PxTransform tm;
		PxVec3 linVel, angVel;
		converter::NetTmToPhysXTm(message->m_data.transform, tm);
		converter::NetVec3ToPhysXVec3(message->m_data.linVel, linVel);
		converter::NetVec3ToPhysXVec3(message->m_data.angVel, angVel);
		m_actors.at(actorId)->setGlobalPose(tm);
		//PxRigidBody* rb = reinterpret_cast<PxRigidActor*>(m_actors.at(actorId));
		PxRigidBody* rb = reinterpret_cast<PxRigidBody*>(m_actors.at(actorId));
		rb->setLinearVelocity(linVel);
		rb->setAngularVelocity(angVel);
	}

	// TODO:
	void ProcessPlayerUpdateMessage(PlayerUpdateMessage * message) override {
		otherPlayer.position = converter::NetVec3ToglmVec3(message->m_data.transform.position);
		otherPlayer.orientation = converter::NetQuatToglmQuat(message->m_data.transform.orientation);
	}
};

