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

	static glm::vec3 moveTo(10, -10, 0);

	void CTransformComponent::OnBeginPlay(class CObject* parent)
	{
		Super::OnBeginPlay(parent);

		actorParent_ = static_cast<CActor*>(parent);

		targetTransform_ = &actorParent_->getTransform();
		curveTranslation_.setTimer(&actorParent_->getActorTimer());

		setActive(true);

		if (actorParent_->getActorStaticTag() == "this is a monkey")
		{	// 테스트 코드
			auto loc = actorParent_->getLocation();
			curveTranslation_.setCurveType(ECurveType::Quad);
			curveTranslation_.start(loc, loc + moveTo, 500);
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
			if (actorParent_->getActorStaticTag() == "this is a monkey")
			{	// 테스트 코드
				if (curveTranslation_.isRunning())
				{
					targetTransform_->Translate(curveTranslation_.current());
				}
				else
				{
					moveTo *= -1.f;

					auto loc = actorParent_->getLocation();
					curveTranslation_.start(loc, loc + moveTo, 500);
				}
			}
		}
	}
}
