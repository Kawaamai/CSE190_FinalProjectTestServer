#pragma once

#include "PhysXServerApp.h"
#include <vector>
#include <functional>

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

	void UpdateSnapshot() {
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
		if (m_numActors != nbActors) {
			m_actors.resize(nbActors);
			m_numActors = nbActors;
			std::cerr << "resize actor snapshot" << std::endl;
		}
		if (nbActors) {
			//std::vector<PxRigidActor*> actors(nbActors);
			//std::unique_ptr<std::vector<PxRigidActor*>> p_actors = std::make_unique<std::vector<PxRigidActor*>>(nbActors);
			//scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(p_actors->data()), nbActors);
			//m_actorsPtr = std::move(p_actors);
			//std::cerr << m_actorsPtr->size() << std::endl;
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(&m_actors[0]), nbActors);
			//std::cerr << m_actors.size() << std::endl;
		}
	}

	void ForEachActor(std::function<void(int, PxRigidActor const * const)> f) {
		for (PxU32 i = 0; i < m_numActors; i++) {
			f(i, m_actors.at(0));
		}
	}

protected:
	bool m_running = true;
	//std::unique_ptr<std::vector<PxRigidActor*>> m_actorsPtr;
	// snapshot of dynamic actors
	PxU32 m_numActors = 0;
	std::vector<PxRigidActor*> m_actors;
};
