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
#include "Skybox.h"
#include "BasicShader.h"
#include "HeadModel.h"
#include "BallModel.h"
#include "HandModel.h"
#include "TexturedPlane.h"

#include <vector>
#include <array>

// openal
#include "AL/al.h"
#include "AL/alc.h"

// handle wav files
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define NUM_SOURCES 8

// TODO: remove
#include <glm/gtx/string_cast.hpp>

static void list_audio_devices(const ALCchar *devices) {
	const ALCchar* device = devices, *next = devices + 1;
	size_t len = 0;

	std::cout << "Devices list:\n";
	std::cout << "----------\n";
	while (device && *device != '\0' && next && *next != '\0') {
		std::cout << device << '\n';
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

static inline ALenum to_al_format(short channels, short samples) {
	bool stereo = (channels > 1);
	std::cerr << "stereo " << (stereo ? "true" : "false") << std::endl;

	switch (samples) {
	case 16:
		if (stereo)
			return AL_FORMAT_STEREO16;
		else
			return AL_FORMAT_MONO16;
	case 8:
		if (stereo)
			return AL_FORMAT_STEREO8;
		else
			return AL_FORMAT_MONO8;
	default:
		return -1;
	}
}


#include "ShadowRenderingHeader.h"
// Stuff for shadow mapping
GLuint depthMapFbo;
const unsigned int SHADOW_WIDTH = 2048 * 2, SHADOW_HEIGHT = 2048 * 2;
float near_plane = .001f, far_plane = 12.0f;
GLuint depthMap;
glm::mat4 lightProjection = glm::ortho(-9.0f, 9.0f, -9.0f, 9.0f, near_plane, far_plane);
glm::mat4 lightView = glm::lookAt(-1.5f * glm::vec3(-3, -1, 1), // taken from oglBlinnPhongColor.frag's light direction
	glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;

// An example application that renders a simple cube
class TestPhysicsApp : public RiftApp, public GameClient {
private:
	//Player player;
	std::unique_ptr<PlayerClient> player;
	std::array<glm::vec3, 2> playerHeadPose;
	glm::quat playerHeadOrientation;
	glm::vec3 getPlayerHeadWorldPosition() {
		return player->toWorld()
			* glm::vec4((playerHeadPose.at(ovrEye_Left) + playerHeadPose.at(ovrEye_Right)) / 2.0f, 1.0f);
	}
	std::array<glm::vec3, 2> controllerInitPressPos;
	bool grabbing = false;
	ovrHandType grabbingHand = ovrHand_Right;

	// arrowing
	std::array<bool, 2> fired;
	bool arrowing = false;
	ovrHandType arrowingHand = ovrHand_Right;

	PlayerServer otherPlayer;

	// sphere grid
	//std::unique_ptr<TestPhysicsScene> sphereScene;
	std::unique_ptr<GridScene> gridScene, otherGridScene;
	std::unique_ptr<BasicColorGeometryScene> basicShapeRenderer;
	std::unique_ptr<Skybox> skybox;
	GLuint skyboxShaderId;

	Lighting sceneLight = Lighting(glm::vec3(5.0f, 1.5f, -2.0f), glm::vec3(1.0f));

	std::unique_ptr<TextRenderer> uiFont;

	std::unique_ptr<HeadModel> headModel;
	std::unique_ptr<BallModel> ballModel;
	std::unique_ptr<HandModel> handModel;

	// scoring
	unsigned int p1Score = 0, p2Score = 0;

	// debug
	std::unique_ptr<TexturedPlane> plane;

public:
	TestPhysicsApp(const yojimbo::Address& serverAddress) : GameClient(serverAddress) {}

protected:
	void initGl() override {
		RiftApp::initGl();
		//glClearColor(0.9f, 0.9f, 0.9f, 0.0f); // background color
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f); // background color
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);
		// Note: to disable lighting, don't pass in sceneLight
		player = std::make_unique<PlayerClient>(_session, sceneLight);
		player->initPlayerGL();
		gridScene = std::make_unique<GridScene>(7, 7, .1f, 0.45f, sceneLight);
		otherGridScene = std::make_unique<GridScene>(7, 7, .1f, 0.45f, sceneLight);
		basicShapeRenderer = std::make_unique<BasicColorGeometryScene>(sceneLight);
		skybox = std::make_unique<Skybox>("../Minimal/space_skybox");
		skybox->toWorld = glm::scale(glm::vec3(15.0f)) * glm::rotate(90.0f, glm::vec3(0, 1, 0));
		skyboxShaderId = LoadShaders("../Minimal/skybox.vert", "../Minimal/skybox.frag");
		uiFont = std::make_unique<TextRenderer>("../Minimal/fonts/arial.ttf", 128);

		headModel = std::make_unique<HeadModel>();
		ballModel = std::make_unique<BallModel>();
		handModel = std::make_unique<HandModel>();

		// init fbo for shadow mapping
		glGenFramebuffers(1, &depthMapFbo);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// debug
		plane = std::make_unique<TexturedPlane>();
		plane->toWorld = glm::translate(glm::vec3(0,0,4)) * glm::scale(glm::vec3(2));

		initAl();
	}

	ALuint buffer;
	ALuint source[NUM_SOURCES];
	int currentSource = 0;

	void initAl() {
		ALCdevice *device;
		ALCcontext *ctx;
		ALenum error;

		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

		// init OpenAL
		device = alcOpenDevice(NULL); // select the "preferred dev" 

		if (device) {
			ctx = alcCreateContext(device, NULL);
			if (ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE) {
				if (ctx != NULL)
					alcDestroyContext(ctx);
				alcCloseDevice(device);
				throw std::runtime_error("could not set context1");
			}
		} else {
			throw std::runtime_error("failed device making");
		}

		buffer = LoadSound();
		if (!buffer) {
			CloseAL();
			throw std::runtime_error("sound buffer failed to create");
		}

		// create the source to play the sound with
		//source = 0;
		alGenSources(NUM_SOURCES, source);
		for (int i = 0; i < NUM_SOURCES; i++) {
			alSourcei(source[i], AL_BUFFER, buffer);
			assert(alGetError() == AL_NO_ERROR && "Failed to setup soiund source");
		}

		// play the sound
		//alSourcePlay(source);
	}

	ALuint LoadSound() {
		ALuint _buffer;
		ALenum error;
		ALenum format;

		// open the audio file
		drwav* pWav = drwav_open_file("../Sounds/Woosh-Mark_DiAngelo-4778593.wav");
		if (pWav == NULL) {
			throw std::runtime_error("failed to read wav file");
		}

		// figure out the format
		format = to_al_format(pWav->channels, pWav->bitsPerSample);
		if (format == -1) {
			throw std::runtime_error("unsupported format " + std::to_string(pWav->channels) + " " + std::to_string(pWav->bitsPerSample));
		}

		// readthe audio file
		size_t dataSize = (size_t)pWav->totalPCMFrameCount * pWav->channels * sizeof(int32_t);
		int32_t* pSampleData = (int32_t*)malloc(dataSize);
		drwav_read_s32(pWav, pWav->totalPCMFrameCount, pSampleData);

		// buffer the audio data into a new buffer object,
		// then free the data and close the file
		_buffer = 0;
		alGenBuffers(1, &_buffer);
		alBufferData(_buffer, format, pSampleData, dataSize, pWav->sampleRate);

		// check for errors
		error = alGetError();
		if (error != AL_NO_ERROR) {
			std::cerr << "OpenAL ERROR: " << alGetString(error) << std::endl;
			alDeleteBuffers(1, &_buffer);
			throw std::runtime_error("OpenAL Error");
		}

		// end of load buffer
		// can return buffer here if within a function
		// cleanup wav data
		//free(pSampleData); // yes like a good boi
		drwav_close(pWav);

		return _buffer;
	}

	void shutdownGl() override {
		//sphereScene.reset();
		player->cleanupGl();
		CloseAL();
	}

	void CloseAL() {
		alDeleteSources(NUM_SOURCES, source);
		alDeleteBuffers(1, &buffer);

		// shutdown openAl
		ALCdevice* device;
		ALCcontext* ctx;
		ctx = alcGetCurrentContext();
		if (ctx == NULL)
			return;

		device = alcGetContextsDevice(ctx);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(ctx);
		alcCloseDevice(device);
	}

	void prerenderScene() override {
		glm::mat4 projection = lightSpaceMatrix;
		glm::mat4 view = glm::mat4(1.0f);

		//// render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		// render scene

		// TODO: make a shadow version
		//player->renderControllers(projection, view, eye, eyePose);

		renderPxScene(projection, view, ovrPosef(), true);

		gridScene->shadowrenderSphereGrid(projection, view);
		otherGridScene->shadowrenderSphereGrid(projection, view);


		//// render the other playesr TODO:
		//shadowrenderOtherPlayer(projection, view);

		// cleanup
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		returnToFbo();
	}

	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose, ovrEyeType eye, ovrPosef eyePose) override {
		//glm::mat4 view = glm::inverse((player->toWorld() * glm::mat4_cast(player->baseOrientation)) * headPose);
		glm::mat4 view = glm::inverse(player->toWorld() * headPose);
		skybox->draw(skyboxShaderId, projection, view);

		player->renderControllers(projection, glm::inverse(headPose), eye, eyePose);
		player->renderHands(projection, glm::inverse(headPose), eye, eyePose);

		player->orientation = ovr::toGlm(eyePose.Orientation);
		playerHeadPose[eye] = ovr::toGlm(eyePose.Position);

		//sphereScene->render(projection, view);
		renderPxScene(projection, view, eyePose);


		glm::vec3 eyePos = player->toWorld() * vec4(ovr::toGlm(eyePose.Position), 1.0f);
		if (player->controllers->r_IndexTriggerPressed() && !fired.at(ovrHand_Right) && (!arrowing || arrowingHand == ovrHand_Right)) {
			glm::vec3 dir = glm::normalize(player->getHandPosition(ovrHand_Right) - controllerInitPressPos.at(ovrHand_Right));
			float dist = glm::distance(player->getHandPosition(ovrHand_Right), controllerInitPressPos.at(ovrHand_Right)) * defgame::SWEEP_DIST;
			glm::vec3 pos = controllerInitPressPos.at(ovrHand_Right) + (dir * dist);
			//basicShapeRenderer->renderLine(projection, view, player->getHandPosition(ovrHand_Right), controllerInitPressPos.at(ovrHand_Right), eyePos, glm::vec3(1, 0, 1));
			basicShapeRenderer->renderLine(projection, view, pos, controllerInitPressPos.at(ovrHand_Right), eyePos, glm::vec3(1, 0, 1));
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			basicShapeRenderer->renderSphere(projection, view, glm::translate(pos) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS) / 2.0f), eyePos, glm::vec3(1.0f, 0.0f, 0.0f));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (player->controllers->l_IndexTriggerPressed() && !fired.at(ovrHand_Left) && (!arrowing || arrowingHand == ovrHand_Left)) {
			glm::vec3 dir = glm::normalize(player->getHandPosition(ovrHand_Left) - controllerInitPressPos.at(ovrHand_Left));
			float dist = glm::distance(player->getHandPosition(ovrHand_Left), controllerInitPressPos.at(ovrHand_Left)) * defgame::SWEEP_DIST;
			glm::vec3 pos = controllerInitPressPos.at(ovrHand_Left) + (dir * dist);
			//basicShapeRenderer->renderLine(projection, view, player->getHandPosition(ovrHand_Left), controllerInitPressPos.at(ovrHand_Left), eyePos, glm::vec3(1, 0, 1));
			basicShapeRenderer->renderLine(projection, view, pos, controllerInitPressPos.at(ovrHand_Left), eyePos, glm::vec3(1, 0, 1));
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			basicShapeRenderer->renderSphere(projection, view, glm::translate(pos) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS) / 2.0f), eyePos, glm::vec3(1.0f, 0.0f, 0.0f));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// show sphere hints
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//basicShapeRenderer->renderSphere(projection, view, glm::translate(player->getHandPosition(ovrHand_Right)) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS)), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
		//basicShapeRenderer->renderSphere(projection, view, glm::translate(player->getHandPosition(ovrHand_Left)) * glm::scale(glm::vec3(defgame::SWEEP_RADIUS)), eyePose, glm::vec3(1.0f, 0.0f, 0.0f));
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//gridScene->renderCubeGrid(projection, view, eyePos, glm::vec3(0.3f));
		gridScene->renderSphereGrid(projection, view, eyePos, glm::vec3(0.3f, 0.3f, 0.5f));
		otherGridScene->renderSphereGrid(projection, view, eyePos, glm::vec3(0.5f, 0.3f, 0.3f));

		// render the other playesr TODO:
		renderOtherPlayer(projection, view, eyePos);

		// update headOrientation
		//playerHeadOrientation = ovr::toGlm(eyePose.Orientation);
		//headModel->render(projection, view, eyePos, sceneLight->lightPos, );

		// game score
		std::string scoreDisplayText = std::to_string(p2Score) + " : " + std::to_string(p1Score);
		glm::mat4 scoreDisplayTm = glm::translate(glm::vec3(3.2f, 0, 0)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0, 1, 0));
		//glm::mat4 scoreDisplayTm = glm::mat4(1.0f);
		uiFont->renderText(projection * view * scoreDisplayTm, scoreDisplayText, glm::vec3(0.0f), .005f, glm::vec3(1.0f, 1.0f, 1.0f));

		// left hand text
		//if (player->controllers->gethandStatus(ovrHand_Left)) {
		////	//glm::mat4 handPosTrans = glm::translate(controllers->getHandPosition(ovrHand_Left));
		//	glm::mat4 handPosTrans = glm::translate(player->getHandPosition(ovrHand_Left));
		//	glm::mat4 handRotTrans = glm::mat4_cast(player->controllers->getHandRotation(ovrHand_Left));
		////	//                                                                                        (right of hand, above hand, behind hand)
		////	glm::mat4 gameStartTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, 0.015f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
		////	//                                                                                    (right of hand, above hand, behind hand)
		//	glm::mat4 scoreTextTransform = handPosTrans * (handRotTrans * glm::translate(glm::vec3(0.03f, -0.005f, 0.1f))) * glm::rotate(glm::radians(70.0f), glm::vec3(0, 1, 0));
		////	uiFont->renderText(projection * glm::inverse(headPose) * gameStartTextTransform, gameStartText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
		//	uiFont->renderText(projection * view * scoreTextTransform, scoreDisplayText, glm::vec3(0.0f), .001f, glm::vec3(1.0f, 0.2f, 0.2f));
		//}

		// debug
		//plane->draw(projection, view, depthMap);
	}

	void renderPxScene(const glm::mat4 & projection, const glm::mat4 & view, const ovrPosef eyePose, bool shadow = false) {
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors) {
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			renderPxActors(projection, view, &actors[0], static_cast<PxU32>(actors.size()), eyePose, shadow);
		}
	}

	void renderPxActors(const glm::mat4 & projection, const glm::mat4 & view, PxRigidActor** actors, const PxU32 numActors, const ovrPosef eyePose, bool shadow) {
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		for (PxU32 i = 0; i < numActors; i++) {
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes, nbShapes);
			//const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;
			const bool isStatic = actors[i]->is<PxRigidStatic>();

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
				//glm::vec3 color = BasicColors::blueColor;
				//if (actors[i]->getType() == PxActorTypeFlag::eRIGID_STATIC) {
				//	color = BasicColors::orangeColor;
				//}
				glm::vec3 color = BasicColors::blueColor;
				if (isStatic) {
					if (actors[i]->getName() == "p1Base" || actors[i]->getName() == "p2Base") {
						color = glm::vec3(1.f);
						renderGeometryHolder(projection, view, h, toWorld, eyePose, color, shadow);
					} else {
						glm::mat4 toWorldscaled = toWorld * glm::scale(glm::vec3(1.0, 0.5, 0.5));
						renderGeometryHolder(projection, view, h, toWorldscaled, eyePose, color, shadow);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						renderGeometryHolder(projection, view, h, toWorld, eyePose, color, shadow);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
				} else {
					color = BasicColors::orangeColor;
					renderGeometryHolder(projection, view, h, toWorld, eyePose, color, shadow);
				}
			}
		}
	}


protected:
	
	void update() override {
		if (Step()) {
			// disconnect
			glfwapp_isrunning = false;
		}
		RiftApp::update();
		player->controllers->updateHandState();
		
		// TODO: send server hand + head transforms
		UpdatePlayerPos();
		PxScene* scene;
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
		//AddSweepPushForce(scene, sweepDir, sweepPos, defgame::SWEEP_RADIUS, dist);

		if (m_client.IsConnected()) {
			SweepForceInputMessage* message = (SweepForceInputMessage*)m_client.CreateMessage((int)GameMessageType::SWEEP_FORCE_INPUT);
			NetVec3 net_sweepDir, net_sweepPos;
			converter::PhysXVec3ToNetVec3(sweepDir, net_sweepDir);
			converter::PhysXVec3ToNetVec3(sweepPos, net_sweepPos);

			message->m_data.sweepDir = net_sweepDir;
			message->m_data.sweepPos = net_sweepPos;
			message->m_data.sweepRadius = defgame::SWEEP_RADIUS;
			message->m_data.sweepDistance = dist;
			m_client.SendMessage((int)GameChannel::RELIABLE, message);
			//m_client.SendMessage((int)GameChannel::UNRELIABLE, message);
		}
	}

	void handleInput() override {
		if (player->controllers->r_IndexTriggerDown() && !arrowing) {
			controllerInitPressPos.at(ovrHand_Right) = player->getHandPosition(ovrHand_Right);
		}
		if (player->controllers->l_IndexTriggerDown() && !arrowing) {
			controllerInitPressPos.at(ovrHand_Left) = player->getHandPosition(ovrHand_Left);
		}

		// arrowing
		if (player->controllers->r_IndexTriggerPressed() && player->controllers->l_YButtonDown() && (!arrowing || arrowingHand == ovrHand_Right)) {
			controllerInitPressPos.at(ovrHand_Right) = player->getHandPosition(ovrHand_Left);
			arrowing = true;
			arrowingHand = ovrHand_Right;
			fired.at(ovrHand_Right) = false;
		}
		if (player->controllers->l_IndexTriggerPressed() && player->controllers->r_BButtonDown() && (!arrowing || arrowingHand == ovrHand_Left)) {
			controllerInitPressPos.at(ovrHand_Left) = player->getHandPosition(ovrHand_Right);
			arrowing = true;
			arrowingHand = ovrHand_Left;
			fired.at(ovrHand_Left) = false;
		}
		if (player->controllers->r_IndexTriggerPressed() && player->controllers->l_YButtonPressed() && arrowing) {
			controllerInitPressPos.at(ovrHand_Right) = player->getHandPosition(ovrHand_Left);
		}
		if (player->controllers->l_IndexTriggerPressed() && player->controllers->r_BButtonPressed() && arrowing) {
			controllerInitPressPos.at(ovrHand_Left) = player->getHandPosition(ovrHand_Right);
		}
		// arrowing fire
		if (player->controllers->r_IndexTriggerPressed() && player->controllers->l_YButtonUp() && arrowingHand == ovrHand_Right) {
			SendSweepForce(ovrHand_Right);
			fired.at(ovrHand_Right) = true;
			alSourcePlay(source[currentSource]);
			currentSource = (currentSource + 1) % NUM_SOURCES;
		}
		if (player->controllers->l_IndexTriggerPressed() && player->controllers->r_BButtonUp() && arrowingHand == ovrHand_Left) {
			SendSweepForce(ovrHand_Left);
			fired.at(ovrHand_Left) = true;
			alSourcePlay(source[currentSource]);
			currentSource = (currentSource + 1) % NUM_SOURCES;
		}

		// not arrowing
		if (player->controllers->r_IndexTriggerUp() && !fired.at(ovrHand_Right) && !arrowing) {
			SendSweepForce(ovrHand_Right);
			//alSource3f(source[currentSource], AL_POSITION, controllerInitPressPos.at(ovrHand_Right).x, controllerInitPressPos.at(ovrHand_Right).y, controllerInitPressPos.at(ovrHand_Right).z);
			alSourcePlay(source[currentSource]);
			currentSource = (currentSource + 1) % NUM_SOURCES;
		}
		if (player->controllers->l_IndexTriggerUp() && !fired.at(ovrHand_Left) && !arrowing) {
			SendSweepForce(ovrHand_Left);
			//alSource3f(source[currentSource], AL_POSITION, controllerInitPressPos.at(ovrHand_Left).x, controllerInitPressPos.at(ovrHand_Left).y, controllerInitPressPos.at(ovrHand_Left).z);
			alSourcePlay(source[currentSource]);
			currentSource = (currentSource + 1) % NUM_SOURCES;
		}

		if (player->controllers->r_IndexTriggerUp() && (fired.at(ovrHand_Right) || (arrowing && arrowingHand == ovrHand_Right))) {
			fired.at(ovrHand_Right) = false;
			arrowing = false;
		}
		if (player->controllers->l_IndexTriggerUp() && (fired.at(ovrHand_Left) || (arrowing && arrowingHand == ovrHand_Left))) {
			fired.at(ovrHand_Left) = false;
			arrowing = false;
		}

		//if (player->controllers->r_IndexTriggerPressed()) {
		//	SendSweepForce(ovrHand_Right);
		//}
		//if (player->controllers->l_IndexTriggerPressed()) {
		//	SendSweepForce(ovrHand_Left);
		//}

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
			//player->position = glm::translate(-delta) * glm::vec4(player->position, 1.0f);
			player->position = player->toWorld() * glm::translate(-delta) * glm::vec4(0, 0, 0, 1);
		}

		// update for audio purposes
		//glm::vec3 headWorldPosition = getPlayerHeadWorldPosition();
		//alListener3f(AL_POSITION, headWorldPosition.x, headWorldPosition.y, headWorldPosition.z);
		//glm::vec3 up = glm::mat4_cast(playerHeadOrientation) * glm::vec4(0, 1, 0, 1);
		//ALfloat listenerOri[] = { headWorldPosition.x, headWorldPosition.y, headWorldPosition.z, up.x, up.y, up.z };
		//alListenerfv(AL_ORIENTATION, listenerOri);
	}

	PX_FORCE_INLINE void renderGeometryHolder(const glm::mat4& projection, const glm::mat4& view, const PxGeometryHolder& h, const glm::mat4& toWorld, const ovrPosef eyePose, glm::vec3 color, bool shadow) {
		renderGeometry(projection, view, h.any(), toWorld, eyePose, color, shadow);
	}

	void renderGeometry(const glm::mat4& projection, const glm::mat4& view,const PxGeometry& geom, const glm::mat4& toWorld, const ovrPosef eyePose, glm::vec3 color, bool shadow) {
		glm::vec3 eyePos = player->toWorld() * vec4(ovr::toGlm(eyePose.Position), 1.0f);
		switch (geom.getType()) {
		case PxGeometryType::eBOX: {
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			glm::mat4 scale = glm::scale(2.0f * glm::vec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z));
			//glm::mat4 scale = glm::scale(glm::vec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z));
			if (!shadow)
				basicShapeRenderer->renderCube(projection, view, toWorld * scale, eyePos, color);
			else
				basicShapeRenderer->shadowrenderCube(projection, view, toWorld * scale);
			break;
		}

		case PxGeometryType::eSPHERE: {
			// update scene light position. a little hacky since it is in the render loop
			gridScene->sceneLight.lightPos = glm::vec3(toWorld * glm::vec4(0, 0, 0, 1));
			otherGridScene->sceneLight.lightPos = glm::vec3(toWorld * glm::vec4(0, 0, 0, 1));
			basicShapeRenderer->sceneLight.lightPos = glm::vec3(toWorld * glm::vec4(0, 0, 0, 1));
			player->controllers->sceneLight.lightPos = glm::vec3(toWorld * glm::vec4(0, 0, 0, 1));
			sceneLight.lightPos = glm::vec3(toWorld * glm::vec4(0, 0, 0, 1));
			//const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
			//glm::mat4 scale = glm::scale(glm::vec3(sphereGeom.radius));

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//basicShapeRenderer->renderSphere(projection, view, toWorld * scale, eyePos, glm::vec3(1.0f));
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (!shadow)
				ballModel->render(projection, view, eyePos, sceneLight.lightPos, toWorld);
			else
				ballModel->shadowRender(projection, view, toWorld);
			break;
		}
		case PxGeometryType::ePLANE: {
			//const PxPlaneGeometry& planeGeom = static_cast<const PxPlaneGeometry&>(geom);
			//glm::mat4 scale = glm::scale(glm::vec3(0.2f, 3.0f, 12.0f));
			////glm::mat4 scale = glm::scale(glm::vec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z));
			//basicShapeRenderer->renderCube(projection, view, toWorld * scale, eyePos, color);
			break;
		}
		case PxGeometryType::eCAPSULE:
		case PxGeometryType::eCONVEXMESH:
		case PxGeometryType::eTRIANGLEMESH:
		case PxGeometryType::eINVALID:
		case PxGeometryType::eHEIGHTFIELD:
		case PxGeometryType::eGEOMETRY_COUNT:
			break;
		}
	}

	void renderOtherPlayer(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& eyePos) {
		//glm::vec3 eyePos = player->toWorld() * vec4(ovr::toGlm(eyePose.Position), 1.0f);
		//basicShapeRenderer->renderCube(projection, view, glm::translate(otherPlayer.position) * glm::mat4_cast(otherPlayer.orientation) * glm::scale(glm::vec3(0.2)), eyePos);
		//headModel->render(projection, view, eyePos, sceneLight.lightPos, otherPlayer.toWorld() * glm::mat4_cast(otherPlayer.orientation));
		headModel->render(projection, view, eyePos, sceneLight.lightPos, glm::translate(otherPlayer.position) * glm::mat4_cast(otherPlayer.orientation) * glm::mat4_cast(otherPlayer.baseOrientation));
		handModel->render(projection, view, eyePos, sceneLight.lightPos, glm::translate(otherPlayer.lhandPosition) * glm::mat4_cast(otherPlayer.lhandOrientation) * glm::mat4_cast(otherPlayer.baseOrientation), ovrHand_Left);
		handModel->render(projection, view, eyePos, sceneLight.lightPos, glm::translate(otherPlayer.rhandPosition) * glm::mat4_cast(otherPlayer.rhandOrientation) * glm::mat4_cast(otherPlayer.baseOrientation), ovrHand_Right);
	}

	void shadowrenderOtherPlayer(const glm::mat4& projection, const glm::mat4& view) {
		headModel->shadowRender(projection, view, glm::translate(otherPlayer.position) * glm::mat4_cast(otherPlayer.orientation));
		handModel->shadowRender(projection, view, glm::translate(otherPlayer.lhandPosition) * glm::mat4_cast(otherPlayer.lhandOrientation), ovrHand_Left);
		handModel->shadowRender(projection, view, glm::translate(otherPlayer.rhandPosition) * glm::mat4_cast(otherPlayer.rhandOrientation), ovrHand_Right);
	}

	// message stuff --------------------------------------------------------------

	void SendEveryUpdate() override {
		PlayerUpdateMessage* message = (PlayerUpdateMessage*)m_client.CreateMessage((int)GameMessageType::PLAYER_UPDATE);
		//message->m_data.transform.position = converter::glmVec3ToNetVec3(players.at(i).position);
		glm::vec3 headPose = (playerHeadPose[0] + playerHeadPose[1]) / 2.0f;
		message->p_data.transform.position = converter::glmVec3ToNetVec3(player->toWorld() * glm::vec4(headPose, 1.0f));
		message->p_data.transform.orientation = converter::glmQuatToNetQuat(player->orientation);
		message->l_data.transform.position = converter::glmVec3ToNetVec3(player->getHandPosition(ovrHand_Left));
		message->r_data.transform.position = converter::glmVec3ToNetVec3(player->getHandPosition(ovrHand_Right));
		message->l_data.transform.orientation = converter::glmQuatToNetQuat(player->controllers->getHandRotation(ovrHand_Left));
		message->r_data.transform.orientation = converter::glmQuatToNetQuat(player->controllers->getHandRotation(ovrHand_Right));
		m_client.SendMessage((int)GameChannel::UNRELIABLE, message);
	}

	void ProcessClientConnectedMessage(ClientConnectedMessage* message) override {
		if (m_client.GetClientIndex() == 0) {
			glm::vec3 p = converter::PhysXVec3ToglmVec3(defgame::PLAYER1_START);
			//glm::vec3 p = glm::vec3(0);
			player->position = p;
			gridScene->toWorld = glm::translate(p);
			otherGridScene->toWorld = glm::translate(converter::PhysXVec3ToglmVec3(defgame::PLAYER2_START));
			otherPlayer.baseOrientation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));
		} else {
			std::cerr << "hit client 1 index set position" << std::endl;
			glm::vec3 p = converter::PhysXVec3ToglmVec3(defgame::PLAYER2_START);
			player->position = p;
			player->baseOrientation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0)); // turn player 2 around
			gridScene->toWorld = glm::translate(p);
			otherGridScene->toWorld = glm::translate(converter::PhysXVec3ToglmVec3(defgame::PLAYER1_START));
		}
		std::cerr << m_client.GetClientIndex() << std::endl;
		// shift up a bit
		player->position += glm::vec3(0.0f, 0.5f, 0.0f);
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
		otherPlayer.position = converter::NetVec3ToglmVec3(message->p_data.transform.position);
		otherPlayer.orientation = converter::NetQuatToglmQuat(message->p_data.transform.orientation);
		otherPlayer.lhandPosition = converter::NetVec3ToglmVec3(message->l_data.transform.position);
		otherPlayer.rhandPosition = converter::NetVec3ToglmVec3(message->r_data.transform.position);
		otherPlayer.lhandOrientation = converter::NetQuatToglmQuat(message->l_data.transform.orientation);
		otherPlayer.rhandOrientation = converter::NetQuatToglmQuat(message->r_data.transform.orientation);
	}

	void ProcessUpdateScoreMessage(UpdateScoreMessage * message) override {
		p1Score = message->p1Score;
		p2Score = message->p2Score;
		std::cerr << p1Score << " " << p2Score << std::endl;
	}
};

