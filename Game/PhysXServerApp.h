#pragma once
#include "PxPhysicsAPI.h"
#include "../snippets/snippetutils/SnippetUtils.h"
#include <vector>
#include <functional>
#include <iostream>
#include "GameInteractionInterface.h"

using namespace physx;

#define PVD_HOST "127.0.0.1"

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

	PxRigidDynamic* mainball;
	PxShape* mainballShape;
	PxRigidStatic* p1Base;
	PxShape* p1BaseShape;
	PxRigidStatic* p2Base;
	PxShape* p2BaseShape;

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
			//for (PxU32 j = 0; j < size - i; j++)
			for (PxU32 j = 0; j < size; j++)
			{
				//PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
				PxTransform localTm(PxVec3(PxReal(j * 2), PxReal(i * 2 + 1), 0) * halfExtent);
				PxTransform centerTm(PxVec3(-PxReal(size / 2.f), -PxReal(size), 0.0f) * halfExtent);

				PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(centerTm).transform(localTm));
				body->attachShape(*shape);
				body->setLinearDamping(.0f);
				body->setAngularDamping(.0f);
				//PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				PxRigidBodyExt::updateMassAndInertia(*body, 4.2f);
				gScene->addActor(*body);
			}
		}
		shape->release();
	}

	void addBall(PxTransform tm = PxTransform(PxVec3(0.0))) {
		PxMaterial* ballMaterial = gPhysics->createMaterial(0.0f, 0.05f, 1.0f);
		ballMaterial->setFrictionCombineMode(PxCombineMode::eMIN);
		PxShape* shape = gPhysics->createShape(PxSphereGeometry(.3f), *ballMaterial);
		PxTransform localTm(PxVec3(0.0));
		PxRigidDynamic* body = gPhysics->createRigidDynamic(tm.transform(localTm));
		body->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*body, 3.2f);
		body->setLinearDamping(.0f);
		gScene->addActor(*body);
		mainball = body;
		mainball->setName("mainball");
		mainballShape = shape;
		//shape->release();
	}

	virtual void initScene(bool interactive) {
		printf("init physics scene \n");

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.bounceThresholdVelocity = 0.f;
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
		gMaterial = gPhysics->createMaterial(0.2f, 0.2f, 1.0f);
		gMaterial->setFrictionCombineMode(PxCombineMode::eMIN);

		// testing
		//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
		//gScene->addActor(*groundPlane);

		// TODO: determine if we need to use a different material for the walls
		// create side walls
		PxReal sideWallx = 0.5f, sideWally = 3.0f, sideWallz = 12.0f;
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
		PxShape * endWallShape = gPhysics->createShape(PxBoxGeometry(sideWallRadius, sideWallRadius, sideWallx / 2.0f), *gMaterial);
		for (int i = 0; i < 2; i++) {
			int flip = 1;
			if (i == 1)
				flip = -1;
			PxTransform endWallTm(PxVec3(0.0f, 0.0f, ((sideWallz / 2.0f) + (sideWallx / 2.0f)) * flip));
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
		//createStack(PxTransform(PxVec3(0, -0.5f, 3.0f)), 5, .2f); // small cubes instead
		//createStack(PxTransform(PxVec3(0, -0.0f, -3.0f)), 5, .2f); // small cubes instead
		createStack(PxTransform(PxVec3(0, 0, 3.0f)), 3, .2f); // small cubes instead
		createStack(PxTransform(PxVec3(0, 0, -3.0f)), 3, .2f); // small cubes instead
		addBall();

		// create the bases
		PxShape* baseShape = gPhysics->createShape(PxBoxGeometry(PxVec3(1.2f, 1.2f, .2f)), *gMaterial);
		PxTransform p1BaseTm(defgame::PLAYER1_START + PxVec3(0.f, 0.f, 1.f));
		p1Base = gPhysics->createRigidStatic(p1BaseTm);
		p1Base->attachShape(*baseShape);
		p1Base->setName("p1Base");
		gScene->addActor(*p1Base);
		PxTransform p2BaseTm(defgame::PLAYER2_START + PxVec3(0.f, 0.f, -1.f));
		p2Base = gPhysics->createRigidStatic(p2BaseTm);
		p2Base->attachShape(*baseShape);
		p2Base->setName("p2Base");
		gScene->addActor(*p2Base);
		p1BaseShape = baseShape;
		p2BaseShape = baseShape;

		if(!interactive)
			createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
	}

	int CheckIfScored() {
		PxSphereGeometry ballGeo;
		mainballShape->getSphereGeometry(ballGeo);
		ballGeo.radius += .03f;
		bool p1BaseOverlapping = PxGeometryQuery::overlap(
			ballGeo, mainball->getGlobalPose(),
			p1BaseShape->getGeometry().box(), p1Base->getGlobalPose());
		if (p1BaseOverlapping) {
			mainball->setGlobalPose(PxTransform(PxVec3(0.f), PxQuat(PxIdentity)));
			mainball->setAngularVelocity(PxVec3(0.f));
			mainball->setLinearVelocity(PxVec3(0.f));
			std::cerr << "p2 scored!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
			return 2;
		}
		bool p2BaseOverlapping = PxGeometryQuery::overlap(
			ballGeo, mainball->getGlobalPose(),
			p2BaseShape->getGeometry().box(), p2Base->getGlobalPose());
		if (p2BaseOverlapping) {
			mainball->setGlobalPose(PxTransform(PxVec3(0.f), PxQuat(PxIdentity)));
			mainball->setAngularVelocity(PxVec3(0.f));
			mainball->setLinearVelocity(PxVec3(0.f));
			std::cerr << "p1 scored!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
			return 1;
		}

		return 0;
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

