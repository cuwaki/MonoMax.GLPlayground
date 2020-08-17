#include "CTransformComponent.h"
#include "../Objects/CActor.h"

// 테스트 코드
#include "CDrawComponent.h"

namespace SMGE
{
	CTransformComponent::CTransformComponent(CObject* outer) : CComponent(outer)
	{
		className_ = wtext("SMGE::CTransformComponent");
	}

	SGReflection& CTransformComponent::getReflection()
	{
		if (reflTransformCompo_.get() == nullptr)
			reflTransformCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflTransformCompo_.get();
	}

	static glm::vec3 moveTo(5, 0, 0), moveFrom;
	static glm::vec3 rotateTo(0, 0, 45), rotateFrom;
	static glm::vec3 scaleTo(2, 2, 2), scaleFrom;
	static float TestInterpolationTime = 2000;

	void CTransformComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		// 여기 수정 - 현재는 액터만 가능!! 콤포넌트에도 붙일 수 있게해야한다?? 아니면 오브젝트 말고 트랜스폼을 연결시키자? 이게 맞는듯!
		actorParent_ = SCast<CActor*>(parent);

		targetTransform_ = &actorParent_->getTransform();

		interpTranslation_.setTimer(&actorParent_->getActorTimer());
		interpRotation_.setTimer(&actorParent_->getActorTimer());
		interpScale_.setTimer(&actorParent_->getActorTimer());

		setActive(true);

		if (actorParent_->getActorStaticTag() == "this is a plane")
		{	// 테스트 코드
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

	void CTransformComponent::OnEndPlay()
	{
		Super::OnEndPlay();

	}

	void CTransformComponent::Tick(float td)
	{
		Super::Tick(td);

		if (isActive())
		{
			// 테스트 코드
			if (actorParent_->getActorStaticTag() == "this is a plane")
			{
				// Translate
				if (interpTranslation_.isRunning())
				{
					targetTransform_->Translate(interpTranslation_.current());
				}
				else
				{
					moveTo *= -1.f;
					interpTranslation_.start(actorParent_->getLocation(), actorParent_->getLocation() + moveTo, TestInterpolationTime);
				}

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
