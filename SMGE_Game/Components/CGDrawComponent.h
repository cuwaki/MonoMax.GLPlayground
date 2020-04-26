#pragma once

#include "../GCommonIncludes.h"
#include "CGComponent.h"

class CGDrawComponent : public CGComponent
{
	glm::mat4& getTransform();

	glm::vec3& getLocation();
	glm::vec3& getDirection();
	glm::vec3& getScale();

protected:
	glm::mat4 objectTransform_{ 1 };

	glm::vec3 objectLocation_{ 0 };
	glm::vec3 objectDirection_{ 1, 0, 0 };
	glm::vec3 objectScale_{ 1 };
};
