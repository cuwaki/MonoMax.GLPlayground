#pragma once

#include "GECommonIncludes.h"
#include "CGameBase.h"

namespace SMGE
{
	namespace nsRE
	{
		class CRenderingEngine;
	}

	namespace nsGE
	{
		struct SGEEngineSettings
		{
			glm::vec2 gameScreenSize_{};
		};

		class CEngineBase
		{
		public:
			CEngineBase(CGameBase* gameBase);
			virtual ~CEngineBase();

			void SetRenderingEngine(class nsRE::CRenderingEngine* re);
			class nsRE::CRenderingEngine* GetRenderingEngine();

			virtual void Tick(float);
			virtual void Render(float);

		protected:
			CGameBase* gameBase_ = nullptr;
			class nsRE::CRenderingEngine* renderingEngine_ = nullptr;

			SGEEngineSettings* settings_;
		};
	}
};
