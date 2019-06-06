#pragma once

#include "PhysXServerApp.h"
#include "GameInteractionInterface.h"
#include "networking/SweepForceInput.h"
#include "Minimal/Converter.h"

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
		//std::cerr << "dir: " << sweepDir.x << ", " << sweepDir.y << ", " << sweepDir.z << std::endl;
		//std::cerr << "pos: " << sweepPos.x << ", " << sweepPos.y << ", " << sweepPos.z << std::endl;
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		AddSweepPushForce(scene, sweepDir, sweepPos);
	}

protected:
	bool m_running = true;
	//std::unique_ptr<std::vector<PxRigidActor*>> m_actorsPtr;
	// snapshot of dynamic actors
};
