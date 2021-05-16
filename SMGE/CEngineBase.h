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

	enum class ERenderingPass : unsigned int
	{
		NONE = 0,

		WORLD,
		WORLD_MAX = 6,

		POSTPROCESS,
		POSTPROCESS_MAX = 11,

		EDITOR,
		EDITOR_MAX = 16,

		UI,
		UI_MAX = 21,

		MAX,
	};

	class CEngineBase
	{
	public:
		CEngineBase(CGameBase* gameBase);
		virtual ~CEngineBase();

		void SetRenderingEngine(class nsRE::CRenderingEngine* re);
		class nsRE::CRenderingEngine* GetRenderingEngine() const;

		HWND GetFocusingWindow() const;
		CUserInput& GetUserInput();

		CSystemBase* GetSystem() const;

		virtual void Tick(float);

	protected:
		CGameBase* gameBase_ = nullptr;
		class nsRE::CRenderingEngine* renderingEngine_ = nullptr;

		SGEEngineSettings* settings_;

		CUserInput userInput_;

		std::unique_ptr<CSystemBase> system_;
	};
};
