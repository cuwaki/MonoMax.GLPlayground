#include "CCameraActor.h"
#include "../Components/CSegmentComponent.h"
#include "../Components/CQuadComponent.h"
#include "../Components/CPlaneComponent.h"
#include "../Components/CCubeComponent.h"
#include "../Components/CMovementComponent.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"
#include "../../MonoMax.EngineCore/common/controls.hpp"
#include "../CBoundCheck.h"

//#define DRAW_FRUSTUM
// 테스트 코드 ㅡ 트랜스폼 리팩토링 점검을 위해
//#define ENABLE_FRUSTUM_CULLING

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_CameraActor::SGRefl_CameraActor(CCameraActor& actor) : 
		cameraName_(actor.cameraName_), fovDegrees_(actor.fovDegrees_), zNear_(actor.zNear_), zFar_(actor.zFar_),
		SGRefl_Actor(actor)
	{
	}

	SGRefl_CameraActor::SGRefl_CameraActor(const std::shared_ptr<CCameraActor>& actorPtr) : SGRefl_CameraActor(*actorPtr)
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
		std::fill(frustumPlanes_.begin(), frustumPlanes_.end(), nullptr);

		Ctor();
	}

	void CCameraActor::Ctor()
	{
		// 프러스텀 컬링을 위한 것
		//auto quadCompo = std::make_unique<CQuadComponent>(this);
		//getPersistentComponents().emplace_back(std::move(quadCompo));
	}

	void CCameraActor::Tick(float td)
	{
		Super::Tick(td);

		if (isCurrentCamera())
		{
			// 최적화 - 카메라 트랜스폼이 더티일 때만 하면 된다

			// 테스트 코드 - 여기 리턴하면 카메라 파일럿 모드
			//return;

			auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
			renderCam.SetCameraPos(getTransform().GetFinalPosition());
			renderCam.SetCameraFront(getTransform().GetFinalFront());
		}
		else
		{
		}
	}

	void CCameraActor::BeginPlay()
	{
#ifdef ENABLE_FRUSTUM_CULLING
#else
		Super::BeginPlay();
		return;
#endif

		using namespace nsRE;

		auto& renderCam = GetRenderingEngine()->GetRenderingCamera();
		const CRenderingCamera::SFrustum frustumModel = renderCam.CalculateFrustumModel(fovDegrees_, zNear_, zFar_);

#ifdef ENABLE_FRUSTUM_CULLING
		// AABB용 큐브
		frustumAABBCube_ = static_cast<CCubeComponent*>(getTransientComponents().emplace_back(std::move(std::make_unique<CCubeComponent>(this))).get());
		frustumAABBCube_->Translate(frustumModel.center_);
		frustumAABBCube_->Scale(
			{
				frustumModel.farPlane_[TransformConst::GL_RB].x - frustumModel.farPlane_[TransformConst::GL_LB].x,
				frustumModel.farPlane_[TransformConst::GL_RT].y - frustumModel.farPlane_[TransformConst::GL_RB].y,
				frustumModel.farPlane_[TransformConst::GL_RB].z - frustumModel.nearPlane_[TransformConst::GL_RB].z
			});
		frustumAABBCube_->SetEditorRendering(false);
		// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumAABBCube_->RecalcFinal();
		frustumAABBCube_->SetGizmoColor({ 1.f, 0.f, 1.f });

		// 체크용 평면 만들기
		std::generate(frustumPlanes_.begin(), frustumPlanes_.end(),
			[this]()
			{
				return static_cast<CPlaneComponent*>(getTransientComponents().emplace_back(std::move(std::make_unique<CPlaneComponent>(this))).get());
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
#endif

#ifdef DRAW_FRUSTUM
		// 최적화 - 게임에서는 카메라 및 그의 기즈모들은 그려질 필요가 없다
		//if (getActorStaticTag() == "testCamera")	// 테스트 코드 - 프러스텀 컬링 시각화
		if (getActorStaticTag() == "mainCamera")
		{
			//frustumPlanes_[0]->SetEditorRendering(true);
			//frustumPlanes_[1]->SetEditorRendering(true);
			//frustumPlanes_[2]->SetEditorRendering(true);
			//frustumPlanes_[3]->SetEditorRendering(true);
			//frustumPlanes_[4]->SetEditorRendering(true);
			//frustumPlanes_[5]->SetEditorRendering(true);

			// 테스트 코드 - 카메라 움직이도록
			//auto moveCompo = std::make_unique<CMovementComponent>(this);
			//getTransientComponents().emplace_back(std::move(moveCompo));

			// 이하 모든 처리는 this 와 frustumModel의 모델 좌표계에서 진행되고 있음을 참고
			// 프러스텀 세그먼트 그리기
			CVector<CSegmentComponent*> frustumLines(4);
			std::generate(frustumLines.begin(), frustumLines.end(),
				[this]()
				{
					return static_cast<CSegmentComponent*>(getTransientComponents().emplace_back(std::move(std::make_unique<CSegmentComponent>(this))).get());
				});

			auto Origin2LB = frustumModel.farPlane_[TransformConst::GL_LB];
			frustumLines[0]->Scale({ 0.f, 0.f, glm::length(Origin2LB) });
#ifdef REFACTORING_TRNASFORM
			frustumLines[0]->RotateDirection(glm::normalize(Origin2LB));
#else
			frustumLines[0]->RotateQuat(glm::normalize(Origin2LB));
#endif
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumLines[0]->RecalcFinal();

			auto Origin2RB = frustumModel.farPlane_[TransformConst::GL_RB];
			frustumLines[1]->Scale({ 0.f, 0.f, glm::length(Origin2RB) });
#ifdef REFACTORING_TRNASFORM
			frustumLines[1]->RotateDirection(glm::normalize(Origin2RB));
#else
			frustumLines[1]->RotateQuat(glm::normalize(Origin2RB));
#endif
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumLines[1]->RecalcFinal();

			auto Origin2RT = frustumModel.farPlane_[TransformConst::GL_RT];
			frustumLines[2]->Scale({ 0.f, 0.f, glm::length(Origin2RT) });
#ifdef REFACTORING_TRNASFORM
			frustumLines[2]->RotateDirection(glm::normalize(Origin2RT));
#else
			frustumLines[2]->RotateQuat(glm::normalize(Origin2RT));
#endif
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumLines[2]->RecalcFinal();

			auto Origin2LT = frustumModel.farPlane_[TransformConst::GL_LT];
			frustumLines[3]->Scale({ 0.f, 0.f, glm::length(Origin2LT) });
#ifdef REFACTORING_TRNASFORM
			frustumLines[3]->RotateDirection(glm::normalize(Origin2LT));
#else
			frustumLines[3]->RotateQuat(glm::normalize(Origin2LT));
#endif
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumLines[3]->RecalcFinal();

#ifdef DRAW_FRUSTUM_QUADS
			// 프러스텀 쿼드 그리기
			std::array<class CQuadComponent*, 2> frustumQuads;	// 0 = near, 1 = far, 2 = up, 3 = bottom, 4 = left, 5 = right
			std::fill(frustumQuads.begin(), frustumQuads.end(), nullptr);

			std::generate(frustumQuads.begin(), frustumQuads.end(),
				[this]()
				{
					return static_cast<CQuadComponent*>(getTransientComponents().emplace_back(std::move(std::make_unique<CQuadComponent>(this, true))).get());
				});

			// Near 평면
			frustumQuads[0]->Translate({ 0.f, 0.f, frustumModel.nearPlane_[TransformConst::GL_LB].z });
			frustumQuads[0]->Scale(
				{
					frustumModel.nearPlane_[TransformConst::GL_RB].x - frustumModel.nearPlane_[TransformConst::GL_LB].x,
					frustumModel.nearPlane_[TransformConst::GL_RT].y - frustumModel.nearPlane_[TransformConst::GL_RB].y,
					//12.f,
					//12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[0]->RotateEuler({ 0.f, 180.f, 0.f });	// 테스트 코드 - 프러스텀 컬링 시각화 - 눈에 보이라고 일부러 반대로
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumQuads[0]->RecalcFinal();
			frustumQuads[0]->SetPickingTarget(false);
			frustumQuads[0]->SetCollideTarget(false);
			frustumQuads[0]->SetGizmoColor({ 0.7f, 0.7f, 0.7f });

			// Far 평면
			frustumQuads[1]->Translate({ 0.f, 0.f, frustumModel.farPlane_[TransformConst::GL_LB].z - 0.01f });	// 빼기 적당량 해주지 않으면 zFar_ 를 넘어가므로 컬링되어버린다
			frustumQuads[1]->Scale(
				{
					frustumModel.farPlane_[TransformConst::GL_RB].x - frustumModel.farPlane_[TransformConst::GL_LB].x,
					frustumModel.farPlane_[TransformConst::GL_RT].y - frustumModel.farPlane_[TransformConst::GL_RB].y,
					//12.f,
					//12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[1]->RotateEuler({ 0.f, 180.f, 0.f });	// 테스트 코드 - 프러스텀 컬링 시각화 - 눈에 보이라고 일부러 반대로
			// 테스트 코드 - 리칼크파이널 코드 재검토 - frustumQuads[1]->RecalcFinal();
			frustumQuads[1]->SetPickingTarget(false);
			frustumQuads[1]->SetCollideTarget(false);
			frustumQuads[1]->SetGizmoColor({ 0.5f, 0.5f, 0.5f });

			/* 이하 버그 있는 코드 - 쓰려면 RecalcFinal 도 불러줘야한다
			auto toUp = frustumModel.farPlane_[TransformConst::GL_LT] - frustumModel.nearPlane_[TransformConst::GL_LT];
			toUp.x = 0;	// YZ 평면에 투영
			frustumQuads[2]->Translate(toUp / 2.f);
			frustumQuads[2]->Scale(
				{
					12.f,
					12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[2]->RotateEuler({ fovDegrees_ / 2.f * +1.f, 0.f, 0.f });
			frustumQuads[2]->SetPickingTarget(false);
			frustumQuads[2]->SetCollideTarget(false);
			frustumQuads[2]->SetGizmoColor({ 0.f, 0.f, 1.f });

			auto toBottom = frustumModel.farPlane_[TransformConst::GL_LB] - frustumModel.nearPlane_[TransformConst::GL_LB];
			toBottom.x = 0;	// YZ 평면에 투영
			frustumQuads[3]->Translate(toBottom / 2.f);
			frustumQuads[3]->Scale(
				{
					12.f,
					12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[3]->RotateEuler({ fovDegrees_ / 2.f * -1.f, 0.f, 0.f });
			frustumQuads[3]->SetPickingTarget(false);
			frustumQuads[3]->SetCollideTarget(false);
			frustumQuads[3]->SetGizmoColor({ 0.f, 0.f, 1.f });

			// 여기 - 아래 코드에서 좌, 우 평면의 각도가 안맞는다, 무엇이 문제일까? 그리고
			// 여기 - 이거 완전 중점 아니다, f - n, n 이거 따져야한다
			// 이 문제도 처리해야한다, 약간의 오차가 생기고 있다 - CalculateFrustumModel( 구현 참조

			// 좌측 평면 - 왜 좌측인데 GL_RB를 쓰냐면 프러스텀은 +Z를 바라보고 있어서 +X가 레프트인데, frustumModel.farPlane_, nearPlane_ 은 GL좌표계의 +z를 기준으로 만들어졌기 때문에 GL_LB가 -X, -Y이기 때문에 프러스텀입장에서 보면 Right bottom 이기 때문이다
			auto farPlaneRBY0 = frustumModel.farPlane_[TransformConst::GL_RB];
			farPlaneRBY0.y = 0;	// ZX 평면에 투영
			const auto horizonFOVHalf = glm::degrees(std::acosf(glm::dot(TransformConst::WorldZAxis, glm::normalize(farPlaneRBY0))));

			auto toLeft = farPlaneRBY0;
			frustumQuads[4]->Translate(toLeft / 2.f);	// 여기 - 이거 완전 중점 아니다, f - n, n 이거 따져야한다
			frustumQuads[4]->Scale(
				{
					12.f,
					12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[4]->RotateEuler({ 0.f, horizonFOVHalf * -1.f, 0.f });
			frustumQuads[4]->SetPickingTarget(false);
			frustumQuads[4]->SetCollideTarget(false);
			frustumQuads[4]->SetGizmoColor({ 0.f, 1.f, 0.f });

			auto farPlaneLBY0 = frustumModel.farPlane_[TransformConst::GL_LB];
			farPlaneLBY0.y = 0;

			auto toRight = farPlaneLBY0;
			frustumQuads[5]->Translate(toRight / 2.f);
			frustumQuads[5]->Scale(
				{
					12.f,
					12.f,
					Configs::BoundEpsilon
				});
			frustumQuads[5]->RotateEuler({ 0.f, horizonFOVHalf * +1.f, 0.f });
			frustumQuads[5]->SetPickingTarget(false);
			frustumQuads[5]->SetCollideTarget(false);
			frustumQuads[5]->SetGizmoColor({ 0.f, 1.f, 0.f });
			*/
#endif	// DRAW_FRUSTUM_QUADS
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
			
			renderCam.SetCameraUp(getTransform().GetFinalUp());
			renderCam.SetCameraLeft(getTransform().GetFinalLeft());

			renderCam.SetCameraPos(getTransform().GetFinalPosition());
			renderCam.SetCameraFront(getTransform().GetFinalFront());
		}
		else
		{
		}
	}

	SGReflection& CCameraActor::getReflection()
	{
		if (reflCameraActor_.get() == nullptr)
			reflCameraActor_ = std::make_unique<TReflectionStruct>(*this);
		return *reflCameraActor_.get();
	}

	SAABB CCameraActor::GetFrustumAABB() const
	{
		assert(frustumAABBCube_ && "never nullptr");
		return frustumAABBCube_->GetAABB();
	}

	bool CCameraActor::IsInOrIntersectWithFrustum(CBoundComponent* mainBound) const
	{
		if (mainBound == nullptr)
			return true;	// 여기 - 판단 불가 - 일단 true 로 해둔다 for 카메라 프러스텀 테스트

		const auto& mainBoundBound = mainBound->GetBound();

		const auto& planeBound0 = static_cast<const SPlaneBound&>(frustumPlanes_[0]->GetBound());
		if (planeBound0.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound2 = static_cast<const SPlaneBound&>(frustumPlanes_[2]->GetBound());
		if (planeBound2.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound3 = static_cast<const SPlaneBound&>(frustumPlanes_[3]->GetBound());
		if (planeBound3.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound4 = static_cast<const SPlaneBound&>(frustumPlanes_[4]->GetBound());
		if (planeBound4.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		const auto& planeBound5 = static_cast<const SPlaneBound&>(frustumPlanes_[5]->GetBound());
		if (planeBound5.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		// 가장 마지막에 먼 것을 버리는게 나름 최적인 것 같다
		const auto& planeBound1 = static_cast<const SPlaneBound&>(frustumPlanes_[1]->GetBound());
		if (planeBound1.isInFrontOrIntersect(mainBoundBound) == false)
			return false;

		return true;
	}
}
