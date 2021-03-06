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
			reflTransformCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflTransformCompo_.get();
	}

	// 테스트 코드
	static glm::vec3 moveTo(0, 0, -20), moveFrom;
	static glm::vec3 rotateTo(0, 55, 0), rotateFromEuler;
	static glm::vec3 scaleTo(2, 2, 2), scaleFrom;
	static float TestInterpolationTime = 400;

	void CMovementComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		// 여기 수정 - 현재는 액터만 가능!! 콤포넌트에도 붙일 수 있게해야한다?? 아니면 오브젝트 말고 트랜스폼을 연결시키자? 이게 맞는듯!
		actorParent_ = static_cast<CActor*>(parent);

		targetTransform_ = &actorParent_->getTransform();

		// 테스트 코드 - 인터폴레이션
		actorParent_->getActorTimer().setRatio(0.1f);

		interpTranslation_.setTimer(&actorParent_->getActorTimer());
		interpRotation_.setTimer(&actorParent_->getActorTimer());
		interpScale_.setTimer(&actorParent_->getActorTimer());

		setActive(true);

		{	// 테스트 코드 - 인터폴레이션
			moveFrom = actorParent_->getLocation();
			interpTranslation_.setCurveType(ECurveType::Quad_Out);
			interpTranslation_.start(moveFrom, moveFrom + moveTo, TestInterpolationTime);

			rotateFromEuler = actorParent_->getRotationEulerDegrees();
			interpRotation_.setCurveType(ECurveType::Cos);
			interpRotation_.start({ 0, 0, 0 }, rotateTo, TestInterpolationTime);

			scaleFrom = actorParent_->getScales();
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
					interpTranslation_.start(actorParent_->getLocation(), actorParent_->getLocation() + moveTo, TestInterpolationTime);
				}

				// Rotate
				if (interpRotation_.isRunning())
				{
					targetTransform_->RotateEuler(rotateFromEuler + interpRotation_.current());
				}
				else
				{
					interpRotation_.start({ 0, 0, 0 }, rotateTo, TestInterpolationTime);
				}

				// Scale
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
