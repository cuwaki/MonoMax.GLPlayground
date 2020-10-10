#include "CMovementComponent.h"
#include "../Objects/CActor.h"

// 테스트 코드
#include "CDrawComponent.h"

namespace SMGE
{
	CMovementComponent::CMovementComponent(CObject* outer) : CComponent(outer)
	{
	}

	SGReflection& CMovementComponent::getReflection()
	{
		if (reflTransformCompo_.get() == nullptr)
			reflTransformCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflTransformCompo_.get();
	}

	// 테스트 코드
	static glm::vec3 moveTo(5, 0, 0), moveFrom;
	static glm::vec3 rotateTo(0, 0, 45), rotateFromEuler;
	static glm::vec3 scaleTo(2, 2, 2), scaleFrom;
	static float TestInterpolationTime = 2000;

	void CMovementComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		// 여기 수정 - 현재는 액터만 가능!! 콤포넌트에도 붙일 수 있게해야한다?? 아니면 오브젝트 말고 트랜스폼을 연결시키자? 이게 맞는듯!
		actorParent_ = SCast<CActor*>(parent);

		targetTransform_ = &actorParent_->getTransform();

		interpTranslation_.setTimer(&actorParent_->getActorTimer());
		interpRotation_.setTimer(&actorParent_->getActorTimer());
		interpScale_.setTimer(&actorParent_->getActorTimer());

		setActive(true);

		{	// 테스트 코드
			//moveFrom = actorParent_->getLocation();
			//interpTranslation_.setCurveType(ECurveType::Quad_Out);
			//interpTranslation_.start(moveFrom, moveFrom + moveTo, TestInterpolationTime);

			//rotateFromEuler = actorParent_->getRotationEuler();
			//interpRotation_.setCurveType(ECurveType::Cos);
			//interpRotation_.start(rotateFromEuler, rotateFromEuler + rotateTo, TestInterpolationTime);

			//scaleFrom = actorParent_->getScale();
			//interpScale_.setCurveType(ECurveType::Sin);
			//interpScale_.start(scaleFrom, scaleFrom + scaleTo, TestInterpolationTime);
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
			// 테스트 코드
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

				// Rotate
				//if (interpRotation_.isRunning())
				//{
				//	targetTransform_->RotateEuler(interpRotation_.current());
				//}
				//else
				//{
				//	interpRotation_.start(rotateFromEuler, rotateFromEuler + rotateTo, TestInterpolationTime);
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
