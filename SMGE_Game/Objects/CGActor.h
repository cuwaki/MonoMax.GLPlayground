#pragma once

#include "CGObject.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "../Interfaces/CGInterf_Component.h"

class CGActor;

struct SGRefl_Actor : public SGReflection
{
	SGRefl_Actor(CGActor& actor);

	glm::mat4& worldTransform_;
};

class CGActor : public CGObject,
	public CGInterf_Reflection, public CGInterf_Component
{
	friend struct SGRefl_Actor;

	using Super = CGObject;
	using ReflectionStruct = SGRefl_Actor;

public:
	CGActor();

	virtual void MakeDefault() override;

	glm::mat4& getWorldTransform();
	glm::vec3& getWorldLocation();
	glm::vec3& getWorldDirection();
	glm::vec3& getWorldScale();

public:
	virtual SGReflection& getReflection() override;
	virtual CVector<CUniqPtr<CGComponent>> &getComponentList() override;

protected:
	glm::mat4 worldTransform_;
	glm::vec3 worldLocation_;
	glm::vec3 worldDirection_;
	glm::vec3 worldScale_;

	class CGDrawComponent* cachedMainDrawCompo_;

protected:
	CUniqPtr<SGRefl_Actor> reflActor_;
	CVector<CUniqPtr<CGComponent>> components_;
};
