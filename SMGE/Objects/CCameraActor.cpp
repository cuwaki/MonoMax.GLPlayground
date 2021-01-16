#include "CCameraActor.h"
#include "../Components/CSegmentComponent.h"
#include "../Components/CQuadComponent.h"
#include "../Components/CPlaneComponent.h"
#include "../Components/CCubeComponent.h"
#include "../Components/CMovementComponent.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"
#include "../../MonoMax.EngineCore/common/controls.hpp"
#include "../CBoundCheck.h"

#define DRAW_FRUSTUM

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_CameraActor::SGRefl_CameraActor(CCameraActor& actor) : 
		cameraName_(actor.cameraName_), fovDegrees_(actor.fovDegrees_), zNear_(actor.zNear_), zFar_(actor.zFar_),
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
		ret += _TO_REFL(float, fovDegrees_);
		ret += _TO_REFL(float, zNear_);
		ret += _TO_REFL(float, zFar_);

		return ret;
	}

	SGReflection& SGRefl_CameraActor::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(cameraName_, variableSplitted);
		_FROM_REFL(fovDegrees_, variableSplitted);
		_FROM_REFL(zNear_, variableSplitted);
		_FROM_REFL(zFar_, variableSplitted);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CCameraActor::CCameraActor(CObject* outer) : Super(outer)
	{
		frustumAABBCube_ = nullptr;
		std::fill(frustumQuads_.begin(), frustumQuads_.end(), nullptr);
		std::fill(frustumPlanes_.begin(), frustumPlanes_.end(), nullptr);

		Ctor();
	}

	void CCameraActor::Ctor()
	{
		// 프러스텀 컬링을 위한 것
		//auto quadCompo = MakeUniqPtr<CQuadComponent>(this);
		//getPersistentComponents().emplace_back(std::move(quadCompo));
	}

	void CCameraActor::Tick(float td)
	{
		Super::Tick(td);

		if (isCurrentCamera())
		{
			if (getTransform().IsDirty())	// 여기 - 어? 왜 더티가 안풀리지?
			{
				auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
				renderCam.SetCameraPos(getTransform().GetWorldPosition());
				renderCam.SetCameraFront(getTransform().GetWorldFront());
			}
		}
		else
		{
		}
	}

	void CCameraActor::BeginPlay()
	{
		using namespace nsRE;

		auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
		const CRenderingCamera::SFrustum frustumModel = renderCam.CalculateFrustumModel(fovDegrees_, zNear_, zFar_);

		// AABB용 큐브
		frustumAABBCube_ = SCast<CCubeComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CCubeComponent>(this))).get());
		frustumAABBCube_->Translate(frustumModel.center_);
		frustumAABBCube_->Scale({
			frustumModel.farPlane_[TransformConst::GL_RB].x - frustumModel.farPlane_[TransformConst::GL_LB].x,
			frustumModel.farPlane_[TransformConst::GL_RT].y - frustumModel.farPlane_[TransformConst::GL_RB].y,
			frustumModel.farPlane_[TransformConst::GL_RB].z - frustumModel.nearPlane_[TransformConst::GL_RB].z });
		frustumAABBCube_->SetGizmoColor({ 1.f, 0.f, 1.f });

		// 체크용 평면 만들기
		std::generate(frustumPlanes_.begin(), frustumPlanes_.end(),
			[this]()
			{
				return SCast<CPlaneComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CPlaneComponent>(this))).get());
			});

		// 평면 노멀이 안쪽을 보도록, 현재 frustumModel은 +Z를 보고 있다
		frustumPlanes_[0]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LB], frustumModel.nearPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_RT]);
		frustumPlanes_[0]->SetEditorRendering(false);
		frustumPlanes_[1]->SetBound(frustumModel.farPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_RT], frustumModel.farPlane_[TransformConst::GL_RB]);
		frustumPlanes_[1]->SetEditorRendering(false);
		frustumPlanes_[2]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LT], frustumModel.nearPlane_[TransformConst::GL_RT], frustumModel.farPlane_[TransformConst::GL_RT]);
		frustumPlanes_[2]->SetEditorRendering(false);
		frustumPlanes_[3]->SetBound(frustumModel.farPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_LB]);
		frustumPlanes_[3]->SetEditorRendering(false);
		frustumPlanes_[4]->SetBound(frustumModel.farPlane_[TransformConst::GL_RB], frustumModel.farPlane_[TransformConst::GL_RT], frustumModel.nearPlane_[TransformConst::GL_RT]);
		frustumPlanes_[4]->SetEditorRendering(false);
		frustumPlanes_[5]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_LB]);
		frustumPlanes_[5]->SetEditorRendering(false);

#ifdef DRAW_FRUSTUM
		if (getActorStaticTag() == "testCamera")	// 테스트 코드 - 프러스텀 컬링
		{
			//frustumPlanes_[0]->SetEditorRendering(true);
			//frustumPlanes_[1]->SetEditorRendering(true);
			//frustumPlanes_[2]->SetEditorRendering(true);
			//frustumPlanes_[3]->SetEditorRendering(true);
			//frustumPlanes_[4]->SetEditorRendering(true);
			//frustumPlanes_[5]->SetEditorRendering(true);

			//// 테스트 코드 - 카메라 움직이도록
			auto moveCompo = MakeUniqPtr<CMovementComponent>(this);
			getTransientComponents().emplace_back(std::move(moveCompo));

			// 이하 모든 처리는 this 와 frustumModel의 모델 좌표계에서 진행되고 있음을 참고
			CVector<CSegmentComponent*> frustumLines(4);
			std::generate(frustumLines.begin(), frustumLines.end(),
				[this]()
				{
					return SCast<CSegmentComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CSegmentComponent>(this))).get());
				});

			auto Origin2LB = frustumModel.farPlane_[TransformConst::GL_LB];
			frustumLines[0]->Scale({ 0.f, 0.f, glm::length(Origin2LB) });
			frustumLines[0]->RotateQuat(glm::normalize(Origin2LB));

			auto Origin2RB = frustumModel.farPlane_[TransformConst::GL_RB];
			frustumLines[1]->Scale({ 0.f, 0.f, glm::length(Origin2RB) });
			frustumLines[1]->RotateQuat(glm::normalize(Origin2RB));

			auto Origin2RT = frustumModel.farPlane_[TransformConst::GL_RT];
			frustumLines[2]->Scale({ 0.f, 0.f, glm::length(Origin2RT) });
			frustumLines[2]->RotateQuat(glm::normalize(Origin2RT));

			auto Origin2LT = frustumModel.farPlane_[TransformConst::GL_LT];
			frustumLines[3]->Scale({ 0.f, 0.f, glm::length(Origin2LT) });
			frustumLines[3]->RotateQuat(glm::normalize(Origin2LT));

			// 평면 그리기
			std::generate(frustumQuads_.begin(), frustumQuads_.end(),
				[this]()
				{
					return SCast<CQuadComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CQuadComponent>(this, true))).get());
				});

			// Near 평면
			frustumQuads_[0]->Translate({ 0.f, 0.f, frustumModel.nearPlane_[TransformConst::GL_LB].z });
			frustumQuads_[0]->Scale(
				{
					frustumModel.nearPlane_[TransformConst::GL_RB].x - frustumModel.nearPlane_[TransformConst::GL_LB].x,
					frustumModel.nearPlane_[TransformConst::GL_RT].y - frustumModel.nearPlane_[TransformConst::GL_RB].y,
					//12.f,
					//12.f,
					Configs::BoundEpsilon
				});
			frustumQuads_[0]->RotateEuler({ 0.f, 180.f, 0.f });	// 눈에 보이라고 일부러 반대로
			frustumQuads_[0]->SetPickingTarget(false);
			frustumQuads_[0]->SetCollideTarget(false);
			frustumQuads_[0]->SetGizmoColor({ 0.7f, 0.7f, 0.7f });

			// Far 평면
			frustumQuads_[1]->Translate({ 0.f, 0.f, frustumModel.farPlane_[TransformConst::GL_LB].z - 0.01f });	// 빼기 적당량 해주지 않으면 zFar_ 를 넘어가므로 컬링되어버린다
			frustumQuads_[1]->Scale(
				{
					frustumModel.farPlane_[TransformConst::GL_RB].x - frustumModel.farPlane_[TransformConst::GL_LB].x,
					frustumModel.farPlane_[TransformConst::GL_RT].y - frustumModel.farPlane_[TransformConst::GL_RB].y,
					//12.f,
					//12.f,
					Configs::BoundEpsilon
				});
			frustumQuads_[1]->RotateEuler({ 0.f, 180.f, 0.f });
			frustumQuads_[1]->SetPickingTarget(false);
			frustumQuads_[1]->SetCollideTarget(false);
			frustumQuads_[1]->SetGizmoColor({ 0.5f, 0.5f, 0.5f });

			//auto toUp = frustumModel.farPlane_[TransformConst::GL_LT] - frustumModel.nearPlane_[TransformConst::GL_LT];
			//toUp.x = 0;	// YZ 평면에 투영
			//frustumQuads_[2]->Translate(toUp / 2.f);
			//frustumQuads_[2]->Scale(
			//	{
			//		12.f,
			//		12.f,
			//		Configs::BoundEpsilon
			//	});
			//frustumQuads_[2]->RotateEuler({ fovDegrees_ / 2.f * +1.f, 0.f, 0.f });
			//frustumQuads_[2]->SetPickingTarget(false);
			//frustumQuads_[2]->SetCollideTarget(false);
			//frustumQuads_[2]->SetGizmoColor({ 0.f, 0.f, 1.f });

			//auto toBottom = frustumModel.farPlane_[TransformConst::GL_LB] - frustumModel.nearPlane_[TransformConst::GL_LB];
			//toBottom.x = 0;	// YZ 평면에 투영
			//frustumQuads_[3]->Translate(toBottom / 2.f);
			//frustumQuads_[3]->Scale(
			//	{
			//		12.f,
			//		12.f,
			//		Configs::BoundEpsilon
			//	});
			//frustumQuads_[3]->RotateEuler({ fovDegrees_ / 2.f * -1.f, 0.f, 0.f });
			//frustumQuads_[3]->SetPickingTarget(false);
			//frustumQuads_[3]->SetCollideTarget(false);
			//frustumQuads_[3]->SetGizmoColor({ 0.f, 0.f, 1.f });

			//// 여기 - 아래 코드에서 좌, 우 평면의 각도가 안맞는다, 무엇이 문제일까? 그리고
			//// 여기 - 이거 완전 중점 아니다, f - n, n 이거 따져야한다
			//// 이 문제도 처리해야한다, 약간의 오차가 생기고 있다 - CalculateFrustumModel( 구현 참조

			//// 좌측 평면 - 왜 좌측인데 GL_RB를 쓰냐면 프러스텀은 +Z를 바라보고 있어서 +X가 레프트인데, frustumModel.farPlane_, nearPlane_ 은 GL좌표계의 +z를 기준으로 만들어졌기 때문에 GL_LB가 -X, -Y이기 때문에 프러스텀입장에서 보면 Right bottom 이기 때문이다
			//auto farPlaneRBY0 = frustumModel.farPlane_[TransformConst::GL_RB];
			//farPlaneRBY0.y = 0;	// ZX 평면에 투영
			//const auto horizonFOVHalf = glm::degrees(std::acosf(glm::dot(TransformConst::WorldZAxis, glm::normalize(farPlaneRBY0))));

			//auto toLeft = farPlaneRBY0;
			//frustumQuads_[4]->Translate(toLeft / 2.f);	// 여기 - 이거 완전 중점 아니다, f - n, n 이거 따져야한다
			//frustumQuads_[4]->Scale(
			//	{
			//		12.f,
			//		12.f,
			//		Configs::BoundEpsilon
			//	});
			//frustumQuads_[4]->RotateEuler({ 0.f, horizonFOVHalf * -1.f, 0.f });
			//frustumQuads_[4]->SetPickingTarget(false);
			//frustumQuads_[4]->SetCollideTarget(false);
			//frustumQuads_[4]->SetGizmoColor({ 0.f, 1.f, 0.f });

			//auto farPlaneLBY0 = frustumModel.farPlane_[TransformConst::GL_LB];
			//farPlaneLBY0.y = 0;

			//auto toRight = farPlaneLBY0;
			//frustumQuads_[5]->Translate(toRight / 2.f);
			//frustumQuads_[5]->Scale(
			//	{
			//		12.f,
			//		12.f,
			//		Configs::BoundEpsilon
			//	});
			//frustumQuads_[5]->RotateEuler({ 0.f, horizonFOVHalf * +1.f, 0.f });
			//frustumQuads_[5]->SetPickingTarget(false);
			//frustumQuads_[5]->SetCollideTarget(false);
			//frustumQuads_[5]->SetGizmoColor({ 0.f, 1.f, 0.f });
		}
#endif

		Super::BeginPlay();
	}

	bool CCameraActor::isCurrentCamera() const
	{
		return isCurrent_;
	}

	void CCameraActor::onChangedCurrent(bool isCurrent)
	{
		isCurrent_ = isCurrent;

		auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
		if (isCurrent_)
		{
			renderCam.SetFOV(fovDegrees_);
			renderCam.SetZNearFar(zNear_, zFar_);
			
			renderCam.SetCameraUp(getTransform().GetWorldUp());
			renderCam.SetCameraLeft(getTransform().GetWorldLeft());

			renderCam.SetCameraPos(getTransform().GetWorldPosition());
			renderCam.SetCameraFront(getTransform().GetWorldFront());
		}
		else
		{
		}
	}

	SGReflection& CCameraActor::getReflection()
	{
		if (reflCameraActor_.get() == nullptr)
			reflCameraActor_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflCameraActor_.get();
	}

	SAABB CCameraActor::GetFrustumAABB() const
	{
		assert(frustumAABBCube_ && "never nullptr");
		return frustumAABBCube_->GetAABB();
	}

	bool CCameraActor::IsInOrIntersectWithFrustum(CBoundComponent* mainBound) const
	{
		const auto& mainBoundBound = mainBound->GetBound();

		const auto& planeBound0 = SCast<const SPlaneBound&>(frustumPlanes_[0]->GetBound());
		if (planeBound0.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound2 = SCast<const SPlaneBound&>(frustumPlanes_[2]->GetBound());
		if (planeBound2.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound3 = SCast<const SPlaneBound&>(frustumPlanes_[3]->GetBound());
		if (planeBound3.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound4 = SCast<const SPlaneBound&>(frustumPlanes_[4]->GetBound());
		if (planeBound4.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound5 = SCast<const SPlaneBound&>(frustumPlanes_[5]->GetBound());
		if (planeBound5.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		// 가장 마지막에 먼 것을 버리는게 나름 최적인 것 같다
		const auto& planeBound1 = SCast<const SPlaneBound&>(frustumPlanes_[1]->GetBound());
		if (planeBound1.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		return true;
	}
}