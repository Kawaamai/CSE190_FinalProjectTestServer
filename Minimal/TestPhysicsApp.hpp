#pragma once

#include "Core.h"
#include "OvrHelper.h"
#include "RiftApp.h"
#include "GameClient.h"
#include "Converter.h"

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

#pragma warning( disable : 4068 4244 4267 4065)
#pragma warning( default : 4068 4244 4267 4065)

#include "TestPhysicsScene.h"
#include "AvatarHandler.h"
#include "Player.h"
#include "Lighting.h"
#include "TextRenderer.h"

#include <vector>

// An example application that renders a simple cube
class TestPhysicsApp : public RiftApp, public GameClient {

	Player player;

	// hands
	std::unique_ptr<ControllerHandler> controllers;

	// sphere grid
	std::unique_ptr<TestPhysicsScene> sphereScene;

	//std::unique_ptr<AvatarHandler> av;

	Lighting sceneLight = Lighting(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f));

	//TextRenderer uiFont = TextRenderer("fonts/arial.ttf");
	std::unique_ptr<TextRenderer> uiFont;
public:
	TestPhysicsApp(const yojimbo::Address& serverAddress) : GameClient(serverAddress) {}

protected:
	void initGl() override {
		RiftApp::initGl();
		//glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glClearColor(0.9f, 0.9f, 0.9f, 0.0f); // background color
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);
		// Note: to disable lighting, don't pass in sceneLight
		sphereScene = std::make_unique<TestPhysicsScene>(sceneLight);
		controllers = std::make_unique<ControllerHandler>(_session, sceneLight);
		uiFont = std::make_unique<TextRenderer>("../Minimal/fonts/arial.ttf", 24);
		//av = std::make_unique<AvatarHandler>(_session);
		player.position = glm::translate(glm::vec3(0, 0.5, 0));
	}

	void shutdownGl() override {
		sphereScene.reset();
	}

	// score shown on hand
	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) override {
		glm::mat4 view = glm::inverse(player.position * headPose);
		//controllers->renderHands(projection, view);
		controllers->renderHands(projection, glm::inverse(headPose)); // hack should have player pos transform too.

		sphereScene->render(projection, view);
		renderPxScene(projection, view);

		std::string scoreDisplayText = "Score: 0";
		std::string gameStartText = "Right index trigger to start";

		// left hand
		if (controllers->gethandStatus(ovrHand_Left)) {
			glm::mat4 handPosTrans = glm::translate(controllers->getHandPosition(ovrHand_Left));
			glm::mat4 handRotTrans = glm::mat4_cast(controllers->getHandRotation(ovrHand_Left));
			//                                                                                        (right of hand, above hand, behind hand)
			glm::mat4 gameStartTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, 0.015f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
			//                                                                                    (right of hand, above hand, behind hand)
			glm::mat4 scoreTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, -0.005f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
			uiFont->renderText(projection * glm::inverse(headPose) * gameStartTextTransform, gameStartText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
			uiFont->renderText(projection * glm::inverse(headPose) * scoreTextTransform, scoreDisplayText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
		}
	}

	void renderPxScene(const glm::mat4 & projection, const glm::mat4 & view) {
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors) {
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			renderPxActors(projection, view, &actors[0], static_cast<PxU32>(actors.size()));
		}
	}

	void renderPxActors(const glm::mat4 & projection, const glm::mat4 & view, PxRigidActor** actors, const PxU32 numActors) {
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

				if (sleeping) {
					// TODO: change color
				}
				renderGeometryHolder(projection, view, h, toWorld);

			}
		}
		glm::vec3 dir = glm::mat4_cast(controllers->getHandRotation(ovrHand_Right)) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
		glm::vec3 pos = player.position * glm::vec4(controllers->getHandPosition(ovrHand_Right), 1.0f);
		glm::vec3 pos2 = pos + (2.0f * dir);

		glm::mat4 _toWorld = glm::translate(pos) * glm::scale(glm::vec3(.03f)) * glm::mat4_cast(controllers->getHandRotation(ovrHand_Right));
		sphereScene->renderCube(projection, view, _toWorld);
		_toWorld = glm::translate(pos2) * glm::scale(glm::vec3(.03f));
		sphereScene->renderCube(projection, view, _toWorld);

	}


private:
	
	void update() override {
		Step();
		RiftApp::update();
		controllers->updateHandState();
	}

	void handleInput() override {
		if (controllers->r_IndexTriggerDown()) {
			// loop through actors. if close enough and in direction
			
			PxVec3 sweepDir = converter::glmVec3ToPhysXVec3(glm::mat4_cast(controllers->getHandRotation(ovrHand_Right)) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
			PxVec3 sweepPos = converter::glmVec3ToPhysXVec3(player.position * glm::vec4(controllers->getHandPosition(ovrHand_Right), 1.0f));
			//std::cerr << "sweepDir: " << sweepDir.x << " " << sweepDir.y << " " << sweepDir.z << std::endl;
			//std::cerr << "sweepPos: " << sweepPos.x << " " << sweepPos.y << " " << sweepPos.z << std::endl;
			PxTransform sweepTransform(sweepPos);
			sweepDir.normalize();
			PxSweepHit hitInfo;
			PxHitFlags hitFlags = PxHitFlag::ePOSITION;
			PxReal inflation = 0.0f;
			PxSphereGeometry sweepSphere = PxSphereGeometry(0.2f);
			//PxU32 hitcount = PxGeometryQuery::sweep(sweepDir, 1.0f, sweepSphere, )
			PxScene* scene;
			PxGetPhysics().getScenes(&scene, 1);
			PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
			if (nbActors) {
				std::vector<PxRigidActor*> actors(nbActors);
				scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
				PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
				for (PxU32 i = 0; i < nbActors; i++) {
					const PxU32 nbShapes = actors[i]->getNbShapes();
					PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
					actors[i]->getShapes(shapes, nbShapes);

					// only have one shape
					for (PxU32 j = 0; j < nbShapes; j++) {
						const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
						PxTransform tm = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);
						const PxGeometry geom = shapes[j]->getGeometry().any();
						switch (geom.getType()) {
						case PxGeometryType::eBOX: {
							PxU32 hitcount = PxGeometryQuery::sweep(sweepDir, 2.0f, sweepSphere, sweepTransform, geom, tm, hitInfo, hitFlags);
							//std::cerr << hitcount << std::endl;
							if (hitcount)
								reinterpret_cast<PxRigidBody*>(actors[i])->addForce(sweepDir * 50.0f);
							// TODO: add in early break
							break;
						}
						default:
							break;
						}
					}
				}
			}

		}
	}

	PX_FORCE_INLINE void renderGeometryHolder(const glm::mat4& projection, const glm::mat4& view, const PxGeometryHolder& h, const glm::mat4& toWorld) {
		renderGeometry(projection, view, h.any(), toWorld);
	}

	void renderGeometry(const glm::mat4& projection, const glm::mat4& view,const PxGeometry& geom, const glm::mat4& toWorld) {
		switch (geom.getType()) {
		case PxGeometryType::eBOX: {
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			glm::mat4 scale = glm::scale(2.0f * glm::vec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z));
			sphereScene->renderCube(projection, view, toWorld * scale);
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
};

