#include "CCameraActor.h"
#include "../Components/CSegmentComponent.h"
#include "../Components/CQuadComponent.h"
#include "../Components/CPlaneComponent.h"
#include "../Components/CCubeComponent.h"
#include "../Components/CMovementComponent.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"
#include "../../MonoMax.EngineCore/common/controls.hpp"
#include "../CBoundCheck.h"

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
		frustumCube_ = nullptr;
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
			auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
			renderCam.SetCameraPos(getTransform().GetWorldPosition());
			renderCam.SetCameraLookAt(getTransform().GetWorldFront() * zFar_);
		}
		else
		{
		}
	}

	void CCameraActor::BeginPlay()
	{
		if (getActorStaticTag() == "testCamera")
		{
			// 테스트 코드 - 카메라 움직이도록
			//auto moveCompo = MakeUniqPtr<CMovementComponent>(this);
			//getTransientComponents().emplace_back(std::move(moveCompo));

			using namespace nsRE;

			auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
			const CRenderingCamera::SFrustum frustumModel = renderCam.CalculateFrustumModel(fovDegrees_, zNear_, zFar_);

			// 이하 모든 처리는 this 와 frustumModel의 모델 좌표계에서 진행되고 있음을 참고
			CVector<CSegmentComponent*> segs(4);
			std::generate(segs.begin(), segs.end(),
				[this]()
				{
					return SCast<CSegmentComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CSegmentComponent>(this))).get());
				});

			auto LB2LB = frustumModel.farPlane_[TransformConst::GL_LB];
			segs[0]->Scale({ 0.f, 0.f, glm::length(LB2LB) });
			segs[0]->RotateQuat(glm::normalize(LB2LB));

			auto RB2RB = frustumModel.farPlane_[TransformConst::GL_RB];
			segs[1]->Scale({ 0.f, 0.f, glm::length(RB2RB) });
			segs[1]->RotateQuat(glm::normalize(RB2RB));

			auto RT2RT = frustumModel.farPlane_[TransformConst::GL_RT];
			//RT2RT.y = 0;
			segs[2]->Scale({ 0.f, 0.f, glm::length(RT2RT) });
			segs[2]->RotateQuat(glm::normalize(RT2RT));

			auto LT2LT = frustumModel.farPlane_[TransformConst::GL_LT];
			segs[3]->Scale({ 0.f, 0.f, glm::length(LT2LT) });
			segs[3]->RotateQuat(glm::normalize(LT2LT));

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
			//const auto horizonFOVHalf = glm::degrees(std::acosf(glm::dot(TransformConst::DefaultModelFrontAxis(), glm::normalize(farPlaneRBY0))));

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

			// AABB용 큐브
			frustumCube_ = SCast<CCubeComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CCubeComponent>(this))).get());
			frustumCube_->Translate(frustumModel.center_);
			frustumCube_->Scale({
				frustumModel.farPlane_[TransformConst::GL_RB].x - frustumModel.farPlane_[TransformConst::GL_LB].x,
				frustumModel.farPlane_[TransformConst::GL_RT].y - frustumModel.farPlane_[TransformConst::GL_RB].y,
				frustumModel.farPlane_[TransformConst::GL_RB].z - frustumModel.nearPlane_[TransformConst::GL_RB].z });
			frustumCube_->SetGizmoColor({ 1.f, 0.f, 1.f });

			// 체크용 평면 만들기
			std::generate(frustumPlanes_.begin(), frustumPlanes_.end(),
				[this]()
				{
					return SCast<CPlaneComponent*>(getTransientComponents().emplace_back(std::move(MakeUniqPtr<CPlaneComponent>(this))).get());
				});

			// 평면 노멀이 안쪽을 보도록, 현재 카메라는 +Z를 보고 있다
			frustumPlanes_[0]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LB], frustumModel.nearPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_RT]);
			frustumPlanes_[1]->SetBound(frustumModel.farPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_RT], frustumModel.farPlane_[TransformConst::GL_RB]);
			frustumPlanes_[2]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LT], frustumModel.nearPlane_[TransformConst::GL_RT], frustumModel.farPlane_[TransformConst::GL_RT]);
			frustumPlanes_[3]->SetBound(frustumModel.farPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_RB], frustumModel.nearPlane_[TransformConst::GL_LB]);
			frustumPlanes_[4]->SetBound(frustumModel.farPlane_[TransformConst::GL_RB], frustumModel.farPlane_[TransformConst::GL_RT], frustumModel.nearPlane_[TransformConst::GL_RT]);
			frustumPlanes_[5]->SetBound(frustumModel.nearPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_LT], frustumModel.farPlane_[TransformConst::GL_LB]);
		}

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
			renderCam.SetCameraLookAt(getTransform().GetWorldFront() * zFar_);
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
}
