#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"
#include "NetTypes.h"
#include <glm/gtc/quaternion.hpp>

using namespace physx;

namespace converter {
	/** Convert glm::vec3 to Physx::PxVec3
	 @param[in] vec3 The glm::vec3
	 @return Converted Physx::PxVec3 */
	static PxVec3 glmVec3ToPhysXVec3(const glm::vec3& vec3) {
		return PxVec3(vec3.x, vec3.y, vec3.z);
	}
	/** Convert glm::quat to Physx::PxQuat
	 @param[in] quat The glm::quat
	 @return Converted Physx::PxQuat */
	static PxQuat glmQuatToPhysXQuat(const glm::quat& quat) {
		return PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	/** Convert Physx::PxVec3 to glm::vec3
	 @param[in] vec3 The Physx::PxVec3
	 @return Converted glm::vec3 */
	static glm::vec3 PhysXVec3ToglmVec3(const PxVec3& vec3) {
		return glm::vec3(vec3.x, vec3.y, vec3.z);
	}
	/** Convert Physx::PxQuat to glm::quat
	 @param[in] quat The Physx::PxQuat
	 @return Converted glm::quat */
	static glm::quat PhysXQuatToglmQuat(const PxQuat& quat) {
		return glm::quat(quat.w, quat.x, quat.y, quat.z);
	}

	/** Convert glm::mat4 to PhysX::PxMat44
	 @param[in] mat4 The glm::mat4
	 @return Converted PhysX::PxMat44 */
	static PxMat44 glmMat4ToPhysxMat4(const glm::mat4& mat4) {
		PxMat44 newMat;

		newMat[0][0] = mat4[0][0];
		newMat[0][1] = mat4[0][1];
		newMat[0][2] = mat4[0][2];
		newMat[0][3] = mat4[0][3];

		newMat[1][0] = mat4[1][0];
		newMat[1][1] = mat4[1][1];
		newMat[1][2] = mat4[1][2];
		newMat[1][3] = mat4[1][3];

		newMat[2][0] = mat4[2][0];
		newMat[2][1] = mat4[2][1];
		newMat[2][2] = mat4[2][2];
		newMat[2][3] = mat4[2][3];

		newMat[3][0] = mat4[3][0];
		newMat[3][1] = mat4[3][1];
		newMat[3][2] = mat4[3][2];
		newMat[3][3] = mat4[3][3];


		return newMat;
	}

	/** Convert PhysX::PxMat44 to glm::mat4
	 @param[in] mat4 The PhysX::PxMat44
	 @param[ou] Converted glm::mat4
	 */
	static void PhysXMat4ToglmMat4(const PxMat44& mat4, glm::mat4& newMat) {

		newMat[0][0] = mat4[0][0];
		newMat[0][1] = mat4[0][1];
		newMat[0][2] = mat4[0][2];
		newMat[0][3] = mat4[0][3];

		newMat[1][0] = mat4[1][0];
		newMat[1][1] = mat4[1][1];
		newMat[1][2] = mat4[1][2];
		newMat[1][3] = mat4[1][3];

		newMat[2][0] = mat4[2][0];
		newMat[2][1] = mat4[2][1];
		newMat[2][2] = mat4[2][2];
		newMat[2][3] = mat4[2][3];

		newMat[3][0] = mat4[3][0];
		newMat[3][1] = mat4[3][1];
		newMat[3][2] = mat4[3][2];
		newMat[3][3] = mat4[3][3];

	}

	static void NetTmToPhysXTm(const NetTransform& netTm, PxTransform& newTm) {
		newTm.p.x = netTm.position.x;
		newTm.p.y = netTm.position.y;
		newTm.p.z = netTm.position.z;
		newTm.q.w = netTm.orientation.w;
		newTm.q.x = netTm.orientation.x;
		newTm.q.y = netTm.orientation.y;
		newTm.q.z = netTm.orientation.z;
	}

	static void NetVec3ToPhysXVec3(const NetVec3& netVec, PxVec3& newVec) {
		newVec.x = netVec.x;
		newVec.y = netVec.y;
		newVec.z = netVec.z;
	}

	static void NetQuatToPhysXQuat(const NetQuat& netQuat, PxQuat& newQuat) {
		newQuat.w = netQuat.w;
		newQuat.x = netQuat.x;
		newQuat.y = netQuat.y;
		newQuat.z = netQuat.z;
	}

	static void PhysXTmToNetTm(const PxTransform& tm, NetTransform& newTm) {
		newTm.position.x = tm.p.x;
		newTm.position.y = tm.p.y;
		newTm.position.z = tm.p.z;
		newTm.orientation.w = tm.q.w;
		newTm.orientation.x = tm.q.x;
		newTm.orientation.y = tm.q.y;
		newTm.orientation.z = tm.q.z;
	}

	static void PhysXVec3ToNetVec3(const PxVec3& vec, NetVec3& newVec) {
		newVec.x = vec.x;
		newVec.y = vec.y;
		newVec.z = vec.z;
	}

	static void PhysXQuatToNetQuat(const PxQuat& quat, NetQuat& newQuat) {
		newQuat.w = quat.w;
		newQuat.x = quat.x;
		newQuat.y = quat.y;
		newQuat.z = quat.z;
	}
}
