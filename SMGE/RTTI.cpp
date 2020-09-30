#include "RTTI.hpp"
#include "Objects/CObject.h"
#include "Components/CComponent.h"
#include "Components/CPointComponent.h"
#include "Components/CRayComponent.h"
#include "Components/CCubeComponent.h"
#include "Components/CSphereComponent.h"
#include "Components/CMeshComponent.h"
#include "Components/CMovementComponent.h"

#include "Objects/CActor.h"
#include "Objects/CMap.h"

#include "Assets/CResourceModel.h"

namespace SMGE
{
	// CObject RTTI 
	RTTI_CObject::NewFunctorDefaultT RTTI_CObject::NewClassDefaults_;
	RTTI_CObject::NewFunctorVarietyT RTTI_CObject::NewClassVarieties_;

	DEFINE_RTTI_CObject_DEFAULT(CComponent)
	DEFINE_RTTI_CObject_DEFAULT(CPointComponent);
	DEFINE_RTTI_CObject_DEFAULT(CCubeComponent);
	DEFINE_RTTI_CObject_VARIETY(CCubeComponent, CObject*, const glm::vec3&, const glm::vec3&);
	DEFINE_RTTI_CObject_DEFAULT(CRayComponent);
	DEFINE_RTTI_CObject_VARIETY(CRayComponent, CObject*, float, const glm::vec3&);
	DEFINE_RTTI_CObject_DEFAULT(CMeshComponent);
	DEFINE_RTTI_CObject_VARIETY(CMeshComponent, CObject*, const CWString&);
	DEFINE_RTTI_CObject_DEFAULT(CSphereComponent);
	DEFINE_RTTI_CObject_DEFAULT(CMovementComponent);

	DEFINE_RTTI_CObject_DEFAULT(CActor);
	DEFINE_RTTI_CObject_VARIETY(CActor, CObject*, const CActor&);
	DEFINE_RTTI_CObject_DEFAULT(CPointActor);
	DEFINE_RTTI_CObject_DEFAULT(CRayCollideActor);
	DEFINE_RTTI_CObject_VARIETY(CRayCollideActor, CObject*, ECheckCollideRule, bool, const DELEGATE_OnCollide&, float, const glm::vec3&);

	DEFINE_RTTI_CObject_DEFAULT(CMap);
	DEFINE_RTTI_CObject_VARIETY(CMap, const CMap&);

	DEFINE_RTTI_CObject_DEFAULT(CResourceModel);
}
