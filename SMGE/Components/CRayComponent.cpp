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
		className_ = wtext("SMGE::CRayComponent");

		size_ = size;
		direction_ = glm::normalize(direction);

		Ctor();
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
		const auto resmKey = wtext("gizmoK:ray");

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if(rsm == nullptr)
			rsm = new nsRE::RayRSM(size_, direction_);

		// ���� ���� - �̰� CResourceModel �� �����簡, ���ӿ������� �������� �ϵ��� ����
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
			// ������ �������� ���� ���� �Ͽ���

			// ����� �õ� ���� ��츦 ���� �ɷ�����
			float minSize = r2sLen - sphere->GetRadius();
			if (size_ < minSize)
				goto RET_FALSE;

			if (r2sLen <= sphere->GetRadius())	// ���ϴ� �� ����
			{	// ���� ������ �� �߽��� �Ÿ��� ���� ���������� �۴� - ���̰� ���� �ȿ��� �߻�� ����
				outCollidingPoint = rayLoc;
				return true;
			}

			//auto normR2S = glm::normalize(ray2sphere);

			float cosTheta = glm::dot(ray2sphere, this->direction_) / r2sLen;	// �̰Ŷ�
			//float cosTheta = glm::dot(normR2S, this->direction_);	// �̰Ŷ�
			if (cosTheta <= 0.f)	// ������ ������ ���ʰ� �ݴ���� �Ǵ� ������
				return false;

			float hypo = r2sLen;
			//float base = glm::dot(this->direction_, ray2sphere);	// �̰Ÿ� �Ȱ��� �ᵵ cosTheta ���� �������� ������? �׷��� dot �ѹ� ���� �� �ִ�.
			float base = cosTheta * r2sLen;

			float radiusSQ = sphere->GetRadius() * sphere->GetRadius();
			float heightSQ = hypo * hypo - base * base;

			if (heightSQ <= radiusSQ)
			{	// ������ ���� �Ÿ��� ���� ���������� �۰ų� ������ �Ÿ� �˻�
				float intersectBase = std::sqrtf(radiusSQ - heightSQ);
				float distToCollidePoint = (base - intersectBase);

				if (distToCollidePoint <= size_)
				{	//  �浹�� �������� �Ÿ��� ũ�⺸�� ������ �浹
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
