#include "CGDrawComponent.h"

glm::mat4& CGDrawComponent::getTransform()
{
	return objectTransform_;
}
glm::vec3& CGDrawComponent::getLocation()
{
	return objectLocation_;
}
glm::vec3& CGDrawComponent::getDirection()
{
	return objectDirection_;
}
glm::vec3& CGDrawComponent::getScale()
{
	return objectScale_;
}
