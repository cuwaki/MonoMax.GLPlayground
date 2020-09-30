#include "CRayComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
#include "CSphereComponent.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_RayComponent::SGRefl_RayComponent(TReflectionClass& rc) : Super(rc), size_(rc.size_), direction_(rc.direction_), sg_transform_(rc, rc), outerRayCompo_(rc)
	{
	}
	//SGRefl_RayComponent::SGRefl_RayComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_RayComponent(*uptr.get())
	//{
	//}

	void SGRefl_RayComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_RayComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += _TO_REFL(float, size_);
		ret += _TO_REFL(glm::vec3, direction_);

		return ret;
	}

	SGReflection& SGRefl_RayComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(size_, variableSplitted);
		_FROM_REFL(direction_, variableSplitted);

		return *this;
	}

	CRayComponent::CRayComponent(CObject* outer, float size, const glm::vec3& direction) : CBoundComponent(outer)
	{
		//classRTTIName_ = "SMGE::CRayComponent";

		SetBoundData(size, direction);
		Ctor();
	}

	void CRayComponent::SetBoundData(float size, const glm::vec3& direction)
	{
		size_ = size;
		direction_ = glm::normalize(direction);
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

	SGReflection& CRayComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflRayCompo_.get();
	}

	void CRayComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:ray";

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if(rsm == nullptr)
			rsm = new nsRE::RayRSM(size_, direction_);

		// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(rsm));

		rsm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CRayComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		if (checkTarget->GetBoundType() == EBoundType::SPHERE)
		{
			CSphereComponent* sphere = DCast<CSphereComponent*>(checkTarget);

			this->RecalcMatrix();
			sphere->RecalcMatrix();

			auto rayLoc = this->GetWorldPosition(), sphereLoc = sphere->GetWorldPosition();
			
			auto ray2sphere = sphereLoc - rayLoc;
			float r2sLen = glm::distance(sphereLoc, rayLoc);

			// https://m.blog.naver.com/PostView.nhn?blogId=hermet&logNo=68084286&proxyReferer=https:%2F%2Fwww.google.com%2F
			// 내용을 기준으로 조금 변형 하였다

			// 사이즈가 택도 없을 경우를 먼저 걸러낸다
			float minSize = r2sLen - sphere->GetRadius();
			if (size_ < minSize)
				goto RET_FALSE;

			if (r2sLen <= sphere->GetRadius())	// 접하는 것 포함
			{	// 레이 원점과 원 중심의 거리가 원의 반지름보다 작다 - 레이가 원의 안에서 발사된 것임
				outCollidingPoint = rayLoc;
				return true;
			}

			//auto normR2S = glm::normalize(ray2sphere);

			float cosTheta = glm::dot(ray2sphere, this->direction_) / r2sLen;	// 이거랑
			//float cosTheta = glm::dot(normR2S, this->direction_);	// 이거랑
			if (cosTheta <= 0.f)	// 레이의 방향이 원쪽과 반대방향 또는 직각임
				return false;

			float hypo = r2sLen;
			//float base = glm::dot(this->direction_, ray2sphere);	// 이거를 똑같이 써도 cosTheta 논리가 동일하지 않을까? 그러면 dot 한번 줄일 수 있다.
			float base = cosTheta * r2sLen;

			float radiusSQ = sphere->GetRadius() * sphere->GetRadius();
			float heightSQ = hypo * hypo - base * base;

			if (heightSQ <= radiusSQ)
			{	// 광선과 구의 거리가 구의 반지름보다 작거나 같으면 거리 검사
				float intersectBase = std::sqrtf(radiusSQ - heightSQ);
				float distToCollidePoint = (base - intersectBase);

				if (distToCollidePoint <= size_)
				{	//  충돌한 지점과의 거리가 크기보다 작으면 충돌
					outCollidingPoint = rayLoc + (direction_ * distToCollidePoint);
					return true;
				}
			}
		}

RET_FALSE:
		outCollidingPoint = nsRE::TransformConst::Vec3_Zero;
		return false;
	}
};
