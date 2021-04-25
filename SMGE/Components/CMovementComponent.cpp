#include "CMovementComponent.h"
#include "../Objects/CActor.h"

// 테스트 코드 - 무브먼트 디버깅용
#include "CDrawComponent.h"

namespace SMGE
{
	CMovementComponent::CMovementComponent(CObject* outer) : CComponent(outer)
	{
	}

	SGReflection& CMovementComponent::getReflection()
	{
		if (reflTransformCompo_.get() == nullptr)
			reflTransformCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflTransformCompo_.get();
	}

	// 테스트 코드 - 무브먼트 테스트용
	static glm::vec3 moveTo(0, 0, -20), moveFrom;
	static glm::vec3 rotateTo(0, 0, 0), rotateFromEuler(0, 0, 0);
	static glm::vec3 scaleTo(2, 2, 2), scaleFrom;
	static float TestInterpolationTime = 400;

	void CMovementComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		CTimer* timer = nullptr;
		auto actorParent = dynamic_cast<CActor*>(parent);
		if (actorParent)
		{
			targetTransform_ = &actorParent->getTransform();
			timer = &actorParent->getActorTimer();
		}
		else
		{
			targetTransform_ = dynamic_cast<nsRE::Transform*>(parent);
			
			myTimer_ = std::make_unique<CTimer>();
			myTimer_->start();

			timer = myTimer_.get();
		}

		// 테스트 코드 - 인터폴레이션
		timer->setRatio(0.2f);
		interpTranslation_.setTimer(timer);
		interpRotation_.setTimer(timer);
		interpScale_.setTimer(timer);
	}

	void CMovementComponent::Tick(float td)
	{
		Super::Tick(td);

		if (isActive() == false)
		{	// 테스트 코드 - 인터폴레이션
			setActive(true);
		
			moveFrom = targetTransform_->GetPendingPosition();
			interpTranslation_.setCurveType(ECurveType::Quad_Out);
			interpTranslation_.start(moveFrom, moveFrom + moveTo, TestInterpolationTime);

			rotateTo = rotateFromEuler = targetTransform_->GetPendingRotationEulerDegreesWorld();
			interpRotation_.setCurveType(ECurveType::Cos);
			rotateTo[1] += 45;
			interpRotation_.start(rotateFromEuler, rotateTo, TestInterpolationTime);

			scaleFrom = targetTransform_->GetPendingScales();
			interpScale_.setCurveType(ECurveType::Linear);
			interpScale_.start(scaleFrom, scaleFrom + scaleTo, TestInterpolationTime);
		}

		if (isActive())
		{
			// 테스트 코드 - 인터폴레이션
			{
				// Translate
				if (interpTranslation_.isRunning())
				{
					targetTransform_->Translate(interpTranslation_.current());
				}
				else
				{
					moveTo *= -1.f;
					interpTranslation_.start(targetTransform_->GetPendingPosition(), targetTransform_->GetPendingPosition() + moveTo, TestInterpolationTime);
				}

				// Rotate
				if (interpRotation_.isRunning())
				{
#ifdef REFACTORING_TRNASFORM
					targetTransform_->RotateEuler(interpRotation_.current(), true);
#else
					targetTransform_->RotateEuler(interpRotation_.current());
#endif
				}
				else
				{
					interpRotation_.start(rotateFromEuler, rotateTo, TestInterpolationTime);
				}

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
