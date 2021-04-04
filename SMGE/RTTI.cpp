#include "RTTI.hpp"
#include "Objects/CObject.h"
#include "Components/CComponent.h"
#include "Components/CPointComponent.h"
#include "Components/CPlaneComponent.h"
#include "Components/CSegmentComponent.h"
#include "Components/CQuadComponent.h"
#include "Components/CCircleComponent.h"
#include "Components/CCubeComponent.h"
#include "Components/CSphereComponent.h"
#include "Components/CMeshComponent.h"
#include "Components/CMovementComponent.h"

#include "Objects/CActor.h"
#include "Objects/CCollideActor.h"
#include "Objects/CStaticMeshActor.h"
#include "Objects/CCameraActor.h"
#include "Objects/CGizmoActor.h"

#include "Objects/CMap.h"

#include "Assets/CResourceModel.h"

namespace SMGE
{
	// CObject RTTI 
	RTTI_CObject::NewFunctorDefaultT RTTI_CObject::NewClassDefaults_;
	RTTI_CObject::NewFunctorVarietyT RTTI_CObject::NewClassVarieties_;

	// 이렇게 모으지 않으면 RTTI 정적변수들의 초기화가 띄엄띄엄 이뤄지게 된다

	DEFINE_RTTI_CObject_DEFAULT(CComponent)

	DEFINE_RTTI_CObject_DEFAULT(CPointComponent);
	DEFINE_RTTI_CObject_VARIETY(CPointComponent, CObject*);
	DEFINE_RTTI_CObject_DEFAULT(CPlaneComponent);
	DEFINE_RTTI_CObject_DEFAULT(CQuadComponent);
	DEFINE_RTTI_CObject_DEFAULT(CCircleComponent);
	DEFINE_RTTI_CObject_DEFAULT(CCubeComponent);
	DEFINE_RTTI_CObject_DEFAULT(CSegmentComponent);
	DEFINE_RTTI_CObject_DEFAULT(CSphereComponent);

	DEFINE_RTTI_CObject_DEFAULT(CMeshComponent);
	DEFINE_RTTI_CObject_VARIETY(CMeshComponent, CObject*, const CWString&);

	DEFINE_RTTI_CObject_DEFAULT(CMovementComponent);
	DEFINE_RTTI_CObject_VARIETY(CMovementComponent, CObject*);

	DEFINE_RTTI_CObject_DEFAULT(CCollideActor);
	DEFINE_RTTI_CObject_VARIETY(CCollideActor, CObject*);
	DEFINE_RTTI_CObject_DEFAULT(CStaticMeshActor);
	DEFINE_RTTI_CObject_VARIETY(CStaticMeshActor, CObject*);
	DEFINE_RTTI_CObject_DEFAULT(CCameraActor);
	DEFINE_RTTI_CObject_VARIETY(CCameraActor, CObject*);
	DEFINE_RTTI_CObject_DEFAULT(CGizmoActorTranslate);
	DEFINE_RTTI_CObject_VARIETY(CGizmoActorTranslate, CObject*);
	DEFINE_RTTI_CObject_DEFAULT(CActor);
	DEFINE_RTTI_CObject_VARIETY(CActor, CObject*);

	DEFINE_RTTI_CObject_DEFAULT(CMap);
	DEFINE_RTTI_CObject_VARIETY(CMap, CObject*);

	DEFINE_RTTI_CObject_DEFAULT(CResourceModel);
	DEFINE_RTTI_CObject_VARIETY(CResourceModel, CObject*, const CString);
}
