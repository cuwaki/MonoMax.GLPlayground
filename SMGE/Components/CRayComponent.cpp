#include "CRayComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
#include "CSphereComponent.h"
#include "CCubeComponent.h"

namespace SMGE
{
	CRayComponent::CRayComponent(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	// 콤포넌트의 트랜스폼을 바꾼다, 보통은 이걸 안쓰고 부모의 트랜스폼이 반영된 것을 쓰게 될 것이다
	void CRayComponent::SetBoundDataComponent(float size, const glm::vec3& direction)
	{
		Scale(nsRE::TransformConst::DefaultAxis_Front, size);
		RotateQuat(glm::normalize(direction));

		RecalcMatrix();
	}

	float CRayComponent::getRayLength() const
	{
		return GetWorldScales()[nsRE::TransformConst::DefaultAxis_Front];
	}

	glm::vec3 CRayComponent::getRayDirection() const
	{
		return GetWorldFront();
	}

	void CRayComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::RAY;
	}

	void CRayComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CRayComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	class CCubeComponent* CRayComponent::GetOBB()
	{
		if (weakOBB_ == nullptr)
		{
			weakOBB_ = CreateOBB();

			// 레이는 0에서 앞으로 뻗지만, OBB 는 큐브라서 중점에서 만들어지므로 Z축을 앵커로 잡아야한다
			assert(nsRE::TransformConst::DefaultAxis_Front == nsRE::TransformConst::ETypeAxis::Z);
			weakOBB_->Translate({ 0, 0, 0.5f });	// 단위크기니까 0.5로 하면 된다
		}

		return weakOBB_;
	}

	SGReflection& CRayComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflRayCompo_.get();
	}

	void CRayComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:ray";

		auto gizmorm = GetRenderingEngine()->GetResourceModel(resmKey);
		if (gizmorm == nullptr)
		{
			gizmorm = new nsRE::RayRM();
			GetRenderingEngine()->AddResourceModel(resmKey, std::move(gizmorm));	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		}

		gizmorm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CRayComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		if (checkTarget->GetBoundType() == EBoundType::SPHERE)
		{
			outCollidingPoint = nsRE::TransformConst::Vec3_Zero;

			CSphereComponent* sphere = DCast<CSphereComponent*>(checkTarget);

			this->RecalcMatrix();
			sphere->RecalcMatrix();

			auto rayLoc = this->GetWorldPosition(), sphereLoc = sphere->GetWorldPosition();
			
			auto ray2sphere = sphereLoc - rayLoc;
			float r2sLen = glm::distance(sphereLoc, rayLoc);

			// https://m.blog.naver.com/PostView.nhn?blogId=hermet&logNo=68084286&proxyReferer=https:%2F%2Fwww.google.com%2F
			// 내용을 기준으로 조금 변형 하였다

			const auto rayLength = this->getRayLength();
			const auto sphereRadius = sphere->GetRadius();
			const auto rayDirection = this->getRayDirection();

			// 사이즈가 택도 없을 경우를 먼저 걸러낸다
			float minSize = r2sLen - sphereRadius;
			if (rayLength < minSize)
				return false;

			if (r2sLen <= sphereRadius)	// 접하는 것 포함
			{	// 레이 원점과 원 중심의 거리가 원의 반지름보다 작다 - 레이가 원의 안에서 발사된 것임
				outCollidingPoint = rayLoc;
				return true;
			}

			//auto normR2S = glm::normalize(ray2sphere);

			float cosTheta = glm::dot(ray2sphere, rayDirection) / r2sLen;	// 이거랑
			//float cosTheta = glm::dot(normR2S, this->direction_);	// 이거랑
			if (cosTheta <= 0.f)	// 레이의 방향이 원쪽과 반대방향 또는 직각임
				return false;

			float hypo = r2sLen;
			//float base = glm::dot(this->direction_, ray2sphere);	// 이거를 똑같이 써도 cosTheta 논리가 동일하지 않을까? 그러면 dot 한번 줄일 수 있다.
			float base = cosTheta * r2sLen;

			float radiusSQ = sphereRadius * sphereRadius;
			float heightSQ = hypo * hypo - base * base;

			if (heightSQ <= radiusSQ)
			{	// 광선과 구의 거리가 구의 반지름보다 작거나 같으면 거리 검사
				float intersectBase = std::sqrtf(radiusSQ - heightSQ);
				float distToCollidePoint = (base - intersectBase);

				if (distToCollidePoint <= rayLength)
				{	//  충돌한 지점과의 거리가 크기보다 작으면 충돌
					outCollidingPoint = rayLoc + (rayDirection * distToCollidePoint);
					return true;
				}
			}
		}

		return false;
	}
};
