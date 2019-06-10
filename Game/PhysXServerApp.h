#pragma once
#include "PxPhysicsAPI.h"
#include "../snippets/snippetutils/SnippetUtils.h"
#include <vector>
#include <functional>
#include <iostream>

using namespace physx;

#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 128

class PhysXServerApp {
public:


protected:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics = NULL;

	PxDefaultCpuDispatcher*	gDispatcher = NULL;
	PxScene*				gScene = NULL;

	PxMaterial*				gMaterial = NULL;

	PxPvd*                  gPvd = NULL;

	//PxReal stackZ = 2.0f;
	PxReal stackZ = -1.0f;

	PxU32 m_numActors = 0;
	std::vector<PxRigidActor*> m_actors;

	void initPhysics(bool interactive) {
		printf("init physics\n");

		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

		gPvd = PxCreatePvd(*gFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

		initScene(interactive);
	}

	void stepPhysics(bool /*interactive*/) {
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
	}

	void cleanupPhysics(bool /*interactive*/) {
		PX_RELEASE(gScene);
		PX_RELEASE(gDispatcher);
		PX_RELEASE(gPhysics);
		if (gPvd)
		{
			PxPvdTransport* transport = gPvd->getTransport();
			gPvd->release();	gPvd = NULL;
			PX_RELEASE(transport);
		}
		PX_RELEASE(gFoundation);

		printf("Physics Cleanup done.\n");
	}
	
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0)) {
		PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
		dynamic->setAngularDamping(0.5f);
		dynamic->setLinearVelocity(velocity);
		gScene->addActor(*dynamic);
		return dynamic;
	}

	void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent) {
		PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
		for (PxU32 i = 0; i < size; i++)
		{
			for (PxU32 j = 0; j < size - i; j++)
			{
				PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
				PxTransform forwardOffset(PxVec3(0.0f, 0.0f, 0.5));
				PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm).transform(forwardOffset));
				body->attachShape(*shape);
				//PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				PxRigidBodyExt::updateMassAndInertia(*body, 5.0f);
				gScene->addActor(*body);
			}
		}
		shape->release();
	}

	void addBall(PxTransform tm = PxTransform(PxVec3(0.0))) {
		PxShape* shape = gPhysics->createShape(PxSphereGeometry(.2f), *gMaterial);
		PxTransform localTm(PxVec3(0.0));
		PxRigidDynamic* body = gPhysics->createRigidDynamic(tm.transform(localTm));
		body->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		gScene->addActor(*body);
		shape->release();
	}

	virtual void initScene(bool interactive) {
		printf("init physics scene \n");

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		//sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f); // No Gravity!!
		gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		gScene = gPhysics->createScene(sceneDesc);

		PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		//gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		gMaterial = gPhysics->createMaterial(0.0f, 0.3f, 1.0f);

		// testing
		//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
		//gScene->addActor(*groundPlane);

		// TODO: determine if we need to use a different material for the walls
		// create side walls
		PxReal sideWallx = 0.2f, sideWally = 3.0f, sideWallz = 12.0f;
		PxReal sideWallRadius = 7.24264f / 2.0f;
		PxShape* sideWallShape = gPhysics->createShape(PxBoxGeometry(sideWallx / 2.0f, sideWally / 2.0f, sideWallz / 2.0f), *gMaterial);
		for (PxU32 i = 0; i < 8; i++) {
			//if (i == 0) {
			//	PxRigidStatic* plane = PxCreatePlane(*gPhysics, PxPlane(-1, 0, 0, sideWallRadius - sideWallx / 2.0f), *gMaterial);
			//	gScene->addActor(*plane);
			//	continue;
			//}
			//if (i == 4) {
			//	PxRigidStatic* plane = PxCreatePlane(*gPhysics, PxPlane(1, 0, 0, sideWallRadius + sideWallx / 2.0f), *gMaterial);
			//	gScene->addActor(*plane);
			//	continue;
			//}
			PxTransform offsetTm(PxVec3(sideWallRadius + sideWallx / 2.0f, 0.0f, 0.0f));
			PxReal angle = PxPiDivFour;
			PxQuat localRot(angle * i, PxVec3(0.0f, 0.0f, 1.0f));
			PxTransform rotTm(localRot);
			PxRigidStatic* sideWallBody = gPhysics->createRigidStatic(rotTm.transform(offsetTm));
			sideWallBody->attachShape(*sideWallShape);
			gScene->addActor(*sideWallBody);
		}

		// create end walls
		PxShape * endWallShape = gPhysics->createShape(PxBoxGeometry(sideWallRadius, sideWallRadius, 0.1f), *gMaterial);
		for (int i = 0; i < 2; i++) {
			int flip = 1;
			if (i == 1)
				flip = -1;
			PxTransform endWallTm(PxVec3(0.0f, 0.0f, sideWallz / 2.0f * flip));
			PxRigidStatic* endWallBody = gPhysics->createRigidStatic(endWallTm);
			endWallBody->attachShape(*endWallShape);
			gScene->addActor(*endWallBody);
		}

		//for(PxU32 i=0;i<5;i++)
		//	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);
		//for(PxU32 i=0;i<5;i++)
		//	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, .5f); // unit cubes instead
		//for(PxU32 i=0;i<5;i++)
		//	createStack(PxTransform(PxVec3(0,0,stackZ-=1.0f)), 10, .1f); // small cubes instead
		//for(PxU32 i=0;i<3;i++)
		//	createStack(PxTransform(PxVec3(0,0,stackZ-=1.0f)), 10, .1f); // small cubes instead
		// TODO: the sizes are kinda wrong on the rigid bodies
		//createStack(PxTransform(PxVec3(0, 0, 3.0f)), 3, .1f); // small cubes instead
		//createStack(PxTransform(PxVec3(0, 0, 3.0f)), 3, .2f); // small cubes instead
		createStack(PxTransform(PxVec3(0, -0.5f, 3.0f)), 5, .2f); // small cubes instead
		createStack(PxTransform(PxVec3(0, -0.0f, -3.0f)), 5, .2f); // small cubes instead
		addBall();

		if(!interactive)
			createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
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

public:
	void ForEachActor(std::function<void(int, PxRigidActor const * const)> f) {
		for (PxU32 i = 0; i < m_numActors; i++) {
			f(i, m_actors.at(i));
		}
	}
};

