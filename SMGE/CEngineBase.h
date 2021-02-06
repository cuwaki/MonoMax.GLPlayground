#pragma once

#include "GECommonIncludes.h"
#include "CGameBase.h"
#include "CSystemBase.h"
#include "CUserInput.h"

namespace SMGE
{
	namespace nsRE
	{
		class CRenderingEngine;
	}

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
		class nsRE::CRenderingEngine* GetRenderingEngine() const;

		HWND HasWindowFocus() const;
		CUserInput& GetUserInput();

		CSystemBase* GetSystem() const;

		virtual void Tick(float);

	protected:
		CGameBase* gameBase_ = nullptr;
		class nsRE::CRenderingEngine* renderingEngine_ = nullptr;

		SGEEngineSettings* settings_;

		CUserInput userInput_;

		CUniqPtr<CSystemBase> system_;
	};
};
