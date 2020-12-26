#include "CCameraActor.h"
#include "../Components/CQuadComponent.h"
#include "../Components/CMovementComponent.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_CameraActor::SGRefl_CameraActor(CCameraActor& actor) : 
		cameraName_(actor.cameraName_), fov_(actor.fov_), zNear_(actor.zNear_), zFar_(actor.zFar_),
		SGRefl_Actor(actor)
	{
	}

	SGRefl_CameraActor::SGRefl_CameraActor(const CSharPtr<CCameraActor>& actorPtr) : SGRefl_CameraActor(*actorPtr)
	{
	}

	SGRefl_CameraActor::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(CString, cameraName_);
		ret += _TO_REFL(float, fov_);
		ret += _TO_REFL(float, zNear_);
		ret += _TO_REFL(float, zFar_);

		return ret;
	}

	SGReflection& SGRefl_CameraActor::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(cameraName_, variableSplitted);
		_FROM_REFL(fov_, variableSplitted);
		_FROM_REFL(zNear_, variableSplitted);
		_FROM_REFL(zFar_, variableSplitted);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CCameraActor::CCameraActor(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CCameraActor::Ctor()
	{
		//cameraName_ = "templateCamera";
		//fov_ = 45.f;
		//zNear_ = 1.f;
		//zFar_ = 100.f;

		// 프러스텀 컬링을 위한 것
		//auto quadCompo = MakeUniqPtr<CQuadComponent>(this);
		//getPersistentComponents().emplace_back(std::move(quadCompo));
	}

	void CCameraActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	SGReflection& CCameraActor::getReflection()
	{
		if (reflCameraActor_.get() == nullptr)
			reflCameraActor_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflCameraActor_.get();
	}
}
