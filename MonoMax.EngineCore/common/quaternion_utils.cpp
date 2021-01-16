#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "quaternion_utils.hpp"

namespace OpenGL_Tutorials
{
	using namespace glm;

	// Returns a quaternion such that q*start = dest
	quat RotationBetweenVectors(vec3 start, vec3 dest)
	{
		start = normalize(start);	// 최적화 - 이거 보통 노멀라이즈 되서 들어온다
		dest = normalize(dest);

		float cosTheta = dot(start, dest);
		vec3 rotationAxis;

		// 원래의 코드 - 여기서 엡실론을 작게 하면 할 수록 튀는 시간이 짧다
		//const auto Epsilon = 0.00001f;	// 잠깐 튀고 돌아온다
		const auto Epsilon = 0.000001f;	// 이 정도로 하니 튀지 않는다, 하지만 플롯 정밀도가 받쳐주나? cpu 마다 또는 최적화 옵션마다 다른 결과가 날 수 있을 듯 하다
		if (cosTheta < (-1.f + Epsilon))
		{
			// special case when vectors in opposite directions :
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			// This implementation favors a rotation around the Up axis,
			// since it's often what you want to do.
			rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
			if (length2(rotationAxis) < 0.001f) // bad luck, they were parallel, try again!
				rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = normalize(rotationAxis);
			return angleAxis(3.141592f, rotationAxis);
		}

		// Implementation from Stan Melax's Game Programming Gems 1 article
		rotationAxis = cross(start, dest);

		float s = sqrt((1.f + cosTheta) * 2.f);
		float invs = 1.f / s;

		return quat(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);
	}

	static constexpr float BoundCheckEpsilon = 0.0001f;	// 통합해라

	// Returns a quaternion that will make your object looking towards 'direction'.
	// Similar to RotationBetweenVectors, but also controls the vertical orientation.
	// This assumes that at rest, the object faces +Z.
	// Beware, the first parameter is a direction, not the target point !
	quat LookAt(vec3 direction, vec3 desiredUp, vec3 defaultDirectionAxis, vec3 defaultUpAxis)
	{
		if (length2(direction) < BoundCheckEpsilon)
			return quat();

		// Recompute desiredUp so that it's perpendicular to the direction
		// You can skip that part if you really want to force desiredUp
		vec3 right = cross(direction, desiredUp);
		desiredUp = cross(right, direction);

		// Find the rotation between the front of the object (that we assume towards +Z,
		// but this depends on your model) and the desired direction
		//quat rot1 = RotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
		quat rot1 = RotationBetweenVectors(defaultDirectionAxis, direction);
		// Because of the 1rst rotation, the up is probably completely screwed up. 
		// Find the rotation between the "up" of the rotated object, and the desired up
		//vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
		vec3 newUp = rot1 * defaultUpAxis;
		quat rot2 = RotationBetweenVectors(newUp, desiredUp);

		// Apply them
		return rot2 * rot1; // remember, in reverse order.
	}



	// Like SLERP, but forbids rotation greater than maxAngle (in radians)
	// In conjunction to LookAt, can make your characters 
	quat RotateTowards(quat q1, quat q2, float maxAngle) {

		if (maxAngle < 0.001f) {
			// No rotation allowed. Prevent dividing by 0 later.
			return q1;
		}

		float cosTheta = dot(q1, q2);

		// q1 and q2 are already equal.
		// Force q2 just to be sure
		if (cosTheta > 0.9999f) {
			return q2;
		}

		// Avoid taking the long path around the sphere
		if (cosTheta < 0) {
			q1 = q1 * -1.0f;
			cosTheta *= -1.0f;
		}

		float angle = acos(cosTheta);

		// If there is only a 2?difference, and we are allowed 5?
		// then we arrived.
		if (angle < maxAngle) {
			return q2;
		}

		// This is just like slerp(), but with a custom t
		float t = maxAngle / angle;
		angle = maxAngle;

		quat res = (sin((1.0f - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
		res = normalize(res);
		return res;

	}




















	void tests() {

		glm::vec3 Xpos(+1.0f, 0.0f, 0.0f);
		glm::vec3 Ypos(0.0f, +1.0f, 0.0f);
		glm::vec3 Zpos(0.0f, 0.0f, +1.0f);
		glm::vec3 Xneg(-1.0f, 0.0f, 0.0f);
		glm::vec3 Yneg(0.0f, -1.0f, 0.0f);
		glm::vec3 Zneg(0.0f, 0.0f, -1.0f);

		// Testing standard, easy case
		// Must be 90?rotation on X : 0.7 0 0 0.7
		quat X90rot = RotationBetweenVectors(Ypos, Zpos);

		// Testing with v1 = v2
		// Must be identity : 0 0 0 1
		quat id = RotationBetweenVectors(Xpos, Xpos);

		// Testing with v1 = -v2
		// Must be 180?on +/-Y axis : 0 +/-1 0 0
		quat Y180rot = RotationBetweenVectors(Xpos, Xneg);

		// Testing with v1 = -v2, but with a "bad first guess"
		// Must be 180?on +/-Y axis : 0 +/-1 0 0
		quat X180rot = RotationBetweenVectors(Zpos, Zneg);


	}
}
