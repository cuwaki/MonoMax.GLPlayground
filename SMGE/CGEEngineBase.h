#pragma once

#include "GECommonIncludes.h"
#include "CGEGameBase.h"

namespace MonoMaxGraphics
{
	struct SGEEngineSettings
	{
		glm::vec2 gameScreenSize_{};
	};

	class CGEEngineBase
	{
	public:
		CGEEngineBase();
		virtual ~CGEEngineBase();

		virtual void Tick(float);
		virtual void Render(float);

	protected:
		CGEGameBase* gameBase_ = nullptr;

		SGEEngineSettings* settings_;
	};
};
