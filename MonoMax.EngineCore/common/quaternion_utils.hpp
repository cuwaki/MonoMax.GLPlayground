#ifndef QUATERNION_UTILS_H
#define QUATERNION_UTILS_H

namespace OpenGL_Tutorials
{
	glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

	glm::quat LookAt(glm::vec3 direction, glm::vec3 desiredUp, const glm::vec3 defaultDirectionAxis, const glm::vec3 defaultUpAxis);

	glm::quat RotateTowards(glm::quat q1, glm::quat q2, float maxAngle);

	void Quat2Euler(glm::quat& q, float& pitch, float& yaw, float& roll);
}


#endif // QUATERNION_UTILS_H