#pragma once

#include "GECommonIncludes.h"
#include "CGameBase.h"

namespace SMGE
{
	namespace nsGE
	{
		struct SGEEngineSettings
		{
			glm::vec2 gameScreenSize_{};
		};

		class CEngineBase
		{
		public:
			CEngineBase();
			virtual ~CEngineBase();

			virtual void Tick(float);
			virtual void Render(float);

		protected:
			CGameBase* gameBase_ = nullptr;

			SGEEngineSettings* settings_;
		};
	}
};
