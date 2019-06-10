#pragma once

#include "PhysXServerApp.h"
#include <vector>

namespace defgame {
	const PxReal SWEEP_RADIUS = .2f;
	const PxReal SWEEP_DIST = 3.0f;
	const PxReal SWEEP_FORCE = 40.0f;

	const PxVec3 PLAYER1_START = PxVec3(0.0f, 0.5f, 4.75f);
	const PxVec3 PLAYER2_START = PxVec3(0.0f, 0.5f, -4.75f);
}

static void AddSweepPushForce(
	const PxScene* scene, // TODO: change this to just passing in m_actors from the snapshot
	PxVec3 sweepDir,
	PxVec3 sweepPos,
	PxReal sweepRadius = defgame::SWEEP_RADIUS,
	PxReal sweepDist = defgame::SWEEP_DIST,
	PxReal sweepForce = defgame::SWEEP_FORCE
) {
	sweepDir.normalize();
	PxTransform sweepTransform(sweepPos);
	PxSweepHit hitInfo;
	PxHitFlags hitFlags = PxHitFlag::ePOSITION;
	PxReal inflation = 0.0f;
	PxSphereGeometry sweepSphere = PxSphereGeometry(sweepRadius);

	std::cerr << "sweep Dir: " << sweepDir.x << sweepDir.y << sweepDir.z << std::endl;
	std::cerr << "sweep Pos: " << sweepPos.x << sweepPos.y << sweepPos.z << std::endl;
	std::cerr << "sweep Radius" << sweepRadius << std::endl;
	std::cerr << "sweep dist" << sweepDist << std::endl;

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
				//const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				const PxTransform shapePose = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);
				std::cerr << "shape pos: " << shapePose.p.x << " " << shapePose.p.y << " " << shapePose.p.z << std::endl;
				PxTransform tm = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);
				const PxGeometry geom = shapes[j]->getGeometry().any();
				switch (geom.getType()) {
				case PxGeometryType::eSPHERE:
				case PxGeometryType::eBOX: {
					PxU32 hitcount = PxGeometryQuery::sweep(sweepDir, sweepDist, sweepSphere, sweepTransform, geom, tm, hitInfo, hitFlags, 0.2f);
					std::cerr << hitcount << std::endl;
					std::cerr << hitInfo.distance << std::endl;
					if (hitcount)
						reinterpret_cast<PxRigidBody*>(actors[i])->addForce(sweepDir * sweepForce);
					// TODO: add in early break
					break;
				}
				//case PxGeometryType::eSPHERE: {
				//	PxU32 hitcount = PxGeometryQuery::sweep(sweepDir, sweepDist, sweepSphere, sweepTransform, geom, tm, hitInfo, hitFlags);
				//	//std::cerr << hitcount << std::endl;
				//	if (hitcount)
				//		reinterpret_cast<PxRigidBody*>(actors[i])->addForce(sweepDir * sweepForce);
				//	// TODO: add in early break
				//	break;
				//}
				default:
					break;
				}
			}
		}
	}
	
}

