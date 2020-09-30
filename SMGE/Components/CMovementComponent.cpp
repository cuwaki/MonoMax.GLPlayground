#include "CMovementComponent.h"
#include "../Objects/CActor.h"

// �׽�Ʈ �ڵ�
#include "CDrawComponent.h"

namespace SMGE
{
	CMovementComponent::CMovementComponent(CObject* outer) : CComponent(outer)
	{
		//classRTTIName_ = "SMGE::CMovementComponent";
	}

	SGReflection& CMovementComponent::getReflection()
	{
		if (reflTransformCompo_.get() == nullptr)
			reflTransformCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflTransformCompo_.get();
	}

	// �׽�Ʈ �ڵ�
	static glm::vec3 moveTo(5, 0, 0), moveFrom;
	static glm::vec3 rotateTo(0, 0, 45), rotateFrom;
	static glm::vec3 scaleTo(2, 2, 2), scaleFrom;
	static float TestInterpolationTime = 2000;

	void CMovementComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		// ���� ���� - ����� ���͸� ����!! ������Ʈ���� ���� �� �ְ��ؾ��Ѵ�?? �ƴϸ� ������Ʈ ���� Ʈ�������� �����Ű��? �̰� �´µ�!
		actorParent_ = SCast<CActor*>(parent);

		targetTransform_ = &actorParent_->getTransform();

		interpTranslation_.setTimer(&actorParent_->getActorTimer());
		interpRotation_.setTimer(&actorParent_->getActorTimer());
		interpScale_.setTimer(&actorParent_->getActorTimer());

		setActive(true);

		if (actorParent_->getActorStaticTag() == "this is a monkey")
		{	// �׽�Ʈ �ڵ�
			moveFrom = actorParent_->getLocation();
			interpTranslation_.setCurveType(ECurveType::Quad_Out);
			interpTranslation_.start(moveFrom, moveFrom + moveTo, TestInterpolationTime);

			rotateFrom = actorParent_->getRotation();
			interpRotation_.setCurveType(ECurveType::Cos);
			interpRotation_.start(rotateFrom, rotateFrom + rotateTo, TestInterpolationTime);

			scaleFrom = actorParent_->getScale();
			interpScale_.setCurveType(ECurveType::Sin);
			interpScale_.start(scaleFrom, scaleFrom + scaleTo, TestInterpolationTime);
		}
	}

	void CMovementComponent::OnEndPlay()
	{
		Super::OnEndPlay();

	}

	void CMovementComponent::Tick(float td)
	{
		Super::Tick(td);

		if (isActive())
		{
			// �׽�Ʈ �ڵ�
			if (actorParent_->getActorStaticTag() == "this is a monkey")
			{
				//// Translate
				//if (interpTranslation_.isRunning())
				//{
				//	targetTransform_->Translate(interpTranslation_.current());
				//}
				//else
				//{
				//	moveTo *= -1.f;
				//	interpTranslation_.start(actorParent_->getLocation(), actorParent_->getLocation() + moveTo, TestInterpolationTime);
				//}

				//// Rotate
				//if (interpRotation_.isRunning())
				//{
				//	targetTransform_->Rotate(interpRotation_.current());
				//}
				//else
				//{
				//	interpRotation_.start(rotateFrom, rotateFrom + rotateTo, TestInterpolationTime);
				//}

				//// Scale
				//if (interpScale_.isRunning())
				//{
				//	targetTransform_->Scale(interpScale_.current());
				//}
				//else
				//{
				//	interpScale_.start(scaleFrom, scaleFrom + scaleTo, TestInterpolationTime);
				//}
			}
		}
	}
}
