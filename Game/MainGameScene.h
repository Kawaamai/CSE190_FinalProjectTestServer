#pragma once

#include "PhysXServerApp.h"
#include "GameInteractionInterface.h"
#include "networking/SweepForceInput.h"
#include "Minimal/Converter.h"
#include "GameConstants.h"
#include "Player.h"
#include <array>

class MainGameScene : public PhysXServerApp {
public:
	MainGameScene() {}
	~MainGameScene() {
		//cleanupPhysics(true);
	}

	void InitScene() {
		initPhysics(true);
	}

	void Run() {
		while (m_running) {
			Update();
		}
	}

	void Update() {
		stepPhysics(true);
		UpdateSnapshot();
	}

	void Cleanup() {
		cleanupPhysics(true);
	}

	void SetRunning(bool running) {
		m_running = running;
	}

	void ProcessSweepForceInputMessage(SweepForceInputMessageData data) {
		PxVec3 sweepDir, sweepPos;
		converter::NetVec3ToPhysXVec3(data.sweepDir, sweepDir);
		converter::NetVec3ToPhysXVec3(data.sweepPos, sweepPos);
		PxReal dist = data.sweepDistance;
		if (dist == 0.0f)
			dist = defgame::SWEEP_DIST;
		//std::cerr << "dir: " << sweepDir.x << ", " << sweepDir.y << ", " << sweepDir.z << std::endl;
		//std::cerr << "pos: " << sweepPos.x << ", " << sweepPos.y << ", " << sweepPos.z << std::endl;
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		AddSweepPushForce(scene, sweepDir, sweepPos, defgame::SWEEP_RADIUS, dist);
	}

	void UpdatePlayer(int clientIdx, TransformMessageData data) {
		players.at(clientIdx).position = converter::NetVec3ToglmVec3(data.transform.position);
		players.at(clientIdx).orientation = converter::NetQuatToglmQuat(data.transform.orientation);
	}

	const std::array<Player, MAX_PLAYERS>& getPlayers() {
		return players;
	}

protected:
	bool m_running = true;
	//std::unique_ptr<std::vector<PxRigidActor*>> m_actorsPtr;
	// snapshot of dynamic actors
	std::array<Player, MAX_PLAYERS> players;
};
